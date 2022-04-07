
# RESTC++
RestC++ is a micro web framework developed with modern C++. Currently supports Windows and Linux systems.
## Contents
- [How to build](#how-to-build)
- [Components of RESTC++](#components-of-restc)
  - [Server](#server)
	  - [Multithreading](#multithreading)
  - [Router](#router)
	  - [Static Route](#static-route)
	  - [Dynamic Route](#dynamic-route)
		  - [Parsing URL Parameters](#parsing-url-parameters)
  - [HTTPRequest](#httprequest)
  - [HTTPResponse](#httpresponse)
  - [FormData](#formdata)
  - [Proxy](#proxy)
- [Notes](#notes)
## How to build
You can easily build by CMake or Makefile included to repo for both Windows and Linux
## Components of RESTC++
### Server
This is wrapper class for whole framework, to be able run the server.
Just create an instance of `restcpp::Server` class and call `run` method.
```
restcpp::Server server(80);
server.run();
```
#### Multithreading
Server uses a custom implementation of thread pool design to create a multithreaded server to serve multiple client connections at the same time.Number of threads that are in thread pool are controlled by `THREAD_COUNT` constant which is in `Common.h` file (default is 3).
### Router
To have a proper web server and/or REST API, you have to define `route`s.
Idea behind routes is parsing URL path and calling a defined callback function to create a response to that request depends on URL path and HTTP method type.
For RESTC++, HTTP method types are specified by a strongly typed enum called `restcpp::METHOD`.

```
enum class METHOD {
GET,POST,PUT,PATCH,DEL,HEAD,OPTIONS,INVALID
};
```
#### Static Route
A static route is a route to directly serve files from a folder to a URL path with file name at the end of the URL.
To add dynamic route we will call `addStaticRoute` function class `Server`class,it may be confusing since we are talking about `Router` class, but instance of router is a part of `Server` class, so every instance of `Server` has exactly one `Router`
```
restcpp::Server server(6005);
server.addStaticRoute("/files/","./WWW_ROOT/");
```
With this example we can directly reach files under `WWW_ROOT` folder by `localhost:6005/files/` path.
#### Dynamic route
A dynamic route is a more complex routing type than static route.In dynamic routing, we generally don't directly send files over path.Instead, we parse fields from `HTTPRequest` object (like form data or file sent with request) or we parse.
To parse request and mutate `HTTPResponse` object, we need user defined callback functions.
To add dynamic route we will call `addRoute` function class `Server`class.
There two ways to accomplish this, first one is define function seperately and pass it as a parameter.
```
void  test(const restcpp::HTTPRequest&  req, restcpp::HTTPResponse&  res)
{
	res.setBodyText("Greetings!");
}
server.addRoute("/greeting/", restcpp::METHOD::GET, test);
```
Second way, you can declare it as lambda function with `std::function`
```
server.addRoute("/greeting2/",restcpp::METHOD::GET, 
	[](const restcpp::HTTPRequest&  req, restcpp::HTTPResponse&  res) 
		{ res.setBodyText("Greetings!"); });
```
##### Parsing URL Parameters
To parse URL parameters and store them as variables in `HTTPRequest` object, we need to specify it with surrounding with curly brackets in URL Path parameter of `addRoute` function.
Here is an example.
```
server.addRoute("/greeting/{name}/{surname}", restcpp::METHOD::GET, testParams);
```
Now you can access `name` and `surname` as variable in `HTTPRequest` parameter of callback function via `getParam` member function of `HTTPRequest` class.
```
void  testParams(const restcpp::HTTPRequest&  req, restcpp::HTTPResponse&  res)
{
	res.setBodyText("Greetings, Mr./Mrs. "  +  req.getParam("name") +  "  "  +  req.getParam("surname"));
}
```
### HTTPRequest
This is part of framework which handles data that stored in HTTP requests, such as different headers and body types.
Such as:
```
Query
Path parameters
General headers (HTTP version,User-Agent,host,URL path,time etc.)
```
Also it parses form body of request if exists, and parses into objects of `FormData` class.

Constructor of `HTTPRequest` class takes raw string read from socket and parses it.Also it has setters and getters so you can modify it,but since callback functions of dynamic routes gets `const HTTPRequest&` you can not do that in call them.
### HTTPResponse 
Class of framework to used create and store responses for requests of clients.Most important of this class is it has different member functions for storing data in body:
```
setBodyText //Set content type of response to text/plain
setBodyJSON //Set content type of response to application/json
setBodyHTML //Set content type of response to text/html
setBodyFile //Send file in response
```
You can serialize `HTTPResponse` objects into proper `std::string` via `serializeResponse` function.
### FormData
This class used to store every member of forms sent with request.
It can store either text or binary data.
### Proxy
You can use RESTC++ as a HTTP proxy as well.All you need to use dynamic routes to proxy a specific path into an address.
Here is an example.
```
void  proxyTest(const restcpp::HTTPRequest&  req, restcpp::HTTPResponse&  res)
{
	res  = restcpp::Proxy("google.com").getResponse();
}
```
### Notes
No note currently.Feel free to contribute this project or start any discussion about the project.