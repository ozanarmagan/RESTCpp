#include "../include/Server.h"


void sendQueries(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    std::string resp = "{";
    for(auto& [key,val] : req.fGetQueries())
    {
        resp += key + ":\"" + val + "\",";
    }
    if(resp.length() > 1)
        resp[resp.length() - 1] = '}';
    else
        resp += "}";
    res.fSetBodyJSON(resp);
}


int main()
{
    
    restcpp::Server server;
    restcpp::Server::Router router;
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

    router.fAddRoute("/querytest/", restcpp::METHOD::GET, sendQueries);

    server.fConnectRouter(&router);
    server.fRun();
}