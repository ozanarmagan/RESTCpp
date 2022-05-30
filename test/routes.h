#pragma once

#include<iostream>

#include "../include/Server.h"
#include "../include/Proxy.h"
#include "../include/SessionManager.h"


restcpp::SessionManager sessionManager;

bool isNumber(const std::string& str)
{
    for(auto& c: str)
    {
        if(!isdigit(c) && c != '.')
            return false;
    }

    return true;
}



void testParams(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    res.setBodyText("Greetings, Mr./Mrs. " + req.getParam("name") +  " " + req.getParam("surname"));
}

void testSetCookie(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    res.addCookie({"test","test"});
}

void proxyTest(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    res = restcpp::Proxy("google.com").getResponse();
}


void sessionTest(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    std::string sessionID = req.getSessionID();
    if(sessionID.empty() || !sessionManager.getSession(sessionID))
    {
        restcpp::Session* session = sessionManager.addSession(restcpp::generateUUID());

        session->setData("viewCount",1);

        std::cout << "Session EXPIRES: " << session->toCookie().getExpires() << std::endl;

        res.setBodyHTML("<html><body><h1>Session test</h1><p>Session ID: " + session->getSessionID() + "</p><p>View Count: " + std::to_string(session->getData<int>("viewCount")) + "</p><p>Expire Time: " + session->getExpiresStr() + "</p></body></html>");

        res.addCookie(session->toCookie());
    }
    else
    {
        restcpp::Session* session = sessionManager.getSession(sessionID);
        session->setData("viewCount",session->getData<int>("viewCount") + 1);
        res.setBodyHTML("<html><body><h1>Session test</h1><p>Session ID: " + session->getSessionID() + "</p><p>View Count: " + std::to_string(session->getData<int>("viewCount")) + "</p><p>Expire Time: " + session->getExpiresStr() + "</p></body></html>");
    }
}