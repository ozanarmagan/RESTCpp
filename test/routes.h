#pragma once

#include<iostream>

#include "../include/Server.h"
#include "../include/Proxy.h"


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

void react(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    std::ifstream file("./root/index.html");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    res.setBodyHTML(content);
}