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

void proxyTest(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    res = restcpp::Proxy("eksisozluk.com").getResponse();
}