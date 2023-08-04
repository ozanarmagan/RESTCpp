#include "../include/Server.h"
#include "routes.h"



int main()
{
    restcpp::Server server(6005);
    server.serveStatic("/","./root/");

    server.get("/greeting/{name}/{surname}", [](const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res) {res.setBodyText("Greetings, Mr./Mrs. " + req.getParam("name") +  " " + req.getParam("surname"));})
          .get("/proxytest/", [](const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res) {res = restcpp::Proxy("www.w3.org").getResponse();})
          .get("/cookietest/", testSetCookie)
          .get("/sessiontest/",  sessionTest)
          .get("/querytest/",  testQuery)
          .post("/fileupload/",  testFileUpload)
          .get("/fileform/",  testFileUploadForm)
          .get("/redirect/", testRedirect);
          


    server.run();

}