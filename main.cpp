#include "HTTPServer.h"



int main()
{
    HTTPServer server;
    HTTPServer::Router router;
    router.fAddStaticRoute("/","./WWW_ROOT/");
    router.fAddRoute("/myname/",METHOD::GET,[] (const HTTPRequest& req,HTTPResponse& res) {
        string name,surname;
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