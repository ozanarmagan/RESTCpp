
# RESTC++
RestC++ is a micro web framework developed with modern C++. Currently supports Windows and Linux systems.
## Contents
- [How to build](#how-to-build)
- [Components of RESTC++](#components-of-restc)
  - [Server](#server)
	  - [Multithreading](#multithreading)
  - [Router](#router)
	  - [Static File Serving](#static-file-serving)
	  - [Dynamic Route](#dynamic-route)
		  - [Parsing URL Parameters](#parsing-url-parameters)
  - [HTTPRequest](#httprequest)
  - [HTTPResponse](#httpresponse)
  - [FormData](#formdata)
  - [Proxy](#proxy)
  - [Cookie](#cookie)
  - [Session](#session)
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
For RESTC++, you can call appropriate function of `Server` class to create a dynamic route

```
Server& get(const std::string& URLPath, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack);
Server& post(const std::string& URLPath, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack);
Server& put(const std::string& URLPath, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack);
Server& patch(const std::string& URLPath, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack);
Server& del(const std::string& URLPath, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack);
```
#### Static File Serving
RESTC++ allows you to  directly serve files from a folder (and its subfolders automatically) to a URL path with file name  at the end of the URL.
To add dynamic route we will call `serveStatic` function class `Server`class.
```
restcpp::Server server(6005);
server.serveStatic("/files/","./WWW_ROOT/");
```
With this example we can directly reach files a under `WWW_ROOT` folder (and its subfolders) by `localhost:6005/files/` path.
#### Dynamic route
A dynamic route is a more complex routing type than static file serving.In dynamic routing, we generally don't directly send files over path.Instead, we parse fields from `HTTPRequest` object (like form data or file sent with request) or we parse.
To parse request and mutate `HTTPResponse` object, we need user defined callback functions.
To add dynamic route we need to use  route methods which attached to instances of `Server` class.
There two ways to accomplish this, first one is define function seperately and pass it as a parameter.
```
void  test(const restcpp::HTTPRequest&  req, restcpp::HTTPResponse&  res)
{
	res.setBodyText("Greetings!");
}
server.get("/greeting/", test);
```
Second way, you can declare it as lambda function with `std::function`
```
server.get("/greeting2/",restcpp::METHOD::GET, 
	[](const restcpp::HTTPRequest&  req, restcpp::HTTPResponse&  res) 
		{ res.setBodyText("Greetings!"); });
```
Another example with `POST` method
```
server.post("/",[](const restcpp::HTTPRequest&  req, restcpp::HTTPResponse&  res) {
	res.setBodyText("You've sent a POST request!");
})
```
##### Parsing URL Parameters
To parse URL parameters and store them as variables in `HTTPRequest` object, we need to specify it with surrounding with curly brackets in URL Path parameter of routing function
Here is an example.
```
server.get("/greeting/{name}/{surname}",  testParams);
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
We can serialize `HTTPResponse` objects into proper `std::string` via `serializeResponse` function.
### FormData
This class used to store every member of forms sent with request.
It can store either text or binary data.
### Proxy
We can use RESTC++ as a HTTP proxy as well.All we need to use dynamic routes to proxy a specific path into an address.
Here is an example.
```
void  proxyTest(const restcpp::HTTPRequest&  req, restcpp::HTTPResponse&  res)
{
	res  = restcpp::Proxy("google.com").getResponse();
}
```
### Cookie
RESTC++ supports HTTP cookies.We can use `Cookie` class to create HTTP cookies and add them to `HTTPResponse` objects by `addCookie` function.
```
void testSetCookie(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
	restcpp::Cookie cookie('test','test');
	res.addCookie(cookie);
}
```
### Session
In RESTC++ HTTP sessions wraps `Cookie`s with `Session` class and stores key-value pairs with `std::string` as a key and `std::any` as a value (which makes you can store object of any class or primitive type as a value).

You need to call `setData` function to store key-value pairs in instance of `Session` class.

```
restcpp::Session session(restcpp::generateUUID());
session.setData("test","test");
```


To store sessions on the server, you have instantiate a `SessionManager` object and add sessions to them. `addSession` function of `SessionManager` class returns `Session*` to the session if you successfully add the session to the manager or create session by providing a session ID (you can use `restcpp::generateUUID` function to crate unique ids for sessions).
```
restcpp::sessionManager.addSession(session); // Add an existing session to the manager
restcpp::Session* newSession = sessionManager(restcpp::generateUUID()); // Creates a new session
```

To set a new cookie over response call `toCookie` function of `Session` instance to get `Session` as a `Cookie` to set.
```
restcpp::session session("test");
res.addCookie(session.toCookie());
```

### Notes
No note currently.Feel free to contribute this project or start any discussion about the project.