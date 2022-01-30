#include "HTTPServer.h"



int main()
{
    
    restcpp::HTTPServer server(6005);
    restcpp::HTTPServer::Router router;
    router.fAddStaticRoute("/","./WWW_ROOT/");

    router.fAddRoute("/myname/", restcpp::METHOD::GET, [] (const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res) {
        std::string name,surname;
        auto query = req.fGetQueries();
        if(query.find("name") != query.end())
            name = query.at("name");
        if(query.find("surname") != query.end())
            surname = query.at("surname");
        res.fSetBodyText("{name:" + ((name!= "") ? ( "\"" +  name + "\"") : "null") + ",surname:" + ((surname != "") ? ( "\"" + surname +  "\"") : "null") + "}");
    });

    server.fConnectRouter(&router);
    server.fRun();
}