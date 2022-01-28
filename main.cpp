#include "HTTPServer.h"



int main()
{
    HTTPServer server;
    HTTPServer::Router router;
    router.fAddStaticRoute("/","./WWW_ROOT/");
    router.fAddRoute("/myname/",METHOD::GET,[] (const HTTPRequest& req,HTTPResponse& res) {
        auto name = req.fGetQueries().at("name");
        res.fSetBodyText("{name:\"" + name + "\",surname:\"" + req.fGetQueries().at("surname") + "\"}");
    });
    server.fConnectRouter(&router);
    server.fRun();
}