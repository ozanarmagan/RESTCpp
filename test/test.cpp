#include "../include/Server.h"
#include "dbConnection.h"
#include "routes.h"
#include "../include/Proxy.h"




int main()
{
    


    restcpp::Server server(6005);
    server.addStaticRoute("/","./WWW_ROOT/");

    server.addRoute("/products/", restcpp::METHOD::GET, getProducts);
    server.addRoute("/greeting/{name}/{surname}", restcpp::METHOD::GET, testParams);
    std::thread t1(&restcpp::Server::run,&server);
    restcpp::Proxy("46.31.79.30:6005/products/");
    t1.join();

}