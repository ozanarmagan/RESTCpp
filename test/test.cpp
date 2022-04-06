#include "../include/Server.h"
//#include "dbConnection.h"
#include "routes.h"
#include "../include/Proxy.h"




int main()
{
    


    restcpp::Server server(6005);
    server.addStaticRoute("/","./WWW_ROOT/");

    //server.addRoute("/products/", restcpp::METHOD::GET, getProducts);
    server.addRoute("/greeting/{name}/{surname}", restcpp::METHOD::GET, testParams);
    server.addRoute("/greeting2/",restcpp::METHOD::GET,[](const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res) { res.setBodyText("Greetings!"); });
    server.addRoute("/proxytest/", restcpp::METHOD::GET, proxyTest);
    std::thread t1(&restcpp::Server::run,&server);
    t1.join();

}