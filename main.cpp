#include "HTTPServer.h"



int main()
{
    HTTPServer server;
    HTTPServer::Router router;
    router.fAddStaticRoute("/","./WWW_ROOT/");
    router.fAddRoute("/myname/",METHOD::GET,[] (HTTPRequest& req,HTTPResponse& res) {
        auto name = req.fGetQueries().at("name");
        res.fSetBody("{name:\"" + name + "\",surname:\"" + req.fGetQueries().at("surname") + "\"}");
    });
    server.fConnectRouter(&router);
    server.fRun();
}