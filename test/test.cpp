#include "../include/Server.h"
//#include "dbConnection.h"
#include "routes.h"



int main()
{
    restcpp::Server server(6005);
    server.addStaticRoute("/","./root/");

    //server.addRoute("/products/", restcpp::METHOD::GET, getProducts);
    server.addRoute("/greeting/{name}/{surname}", restcpp::METHOD::GET, testParams);
    server.addRoute("/greeting2/",restcpp::METHOD::GET,[](const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res) { res.setBodyText("Greetings!"); });
    server.addRoute("/proxytest/", restcpp::METHOD::GET, proxyTest);
    server.addRoute("/cookietest/", restcpp::METHOD::GET, testSetCookie);
    server.addRoute("/sessiontest/", restcpp::METHOD::GET, sessionTest);
    server.addRoute("/querytest/", restcpp::METHOD::GET, testQuery);
    server.addRoute("/fileupload/", restcpp::METHOD::POST, testFileUpload);
    server.addRoute("/fileform/", restcpp::METHOD::GET, testFileUploadForm);
    server.run();

}