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
    res = restcpp::Proxy("www.w3.org").getResponse();
}


void sessionTest(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    std::string sessionID = req.getSessionID();
    if(sessionID.empty() || !sessionManager.getSession(sessionID))
    {
        restcpp::Session* session = sessionManager.addSession(restcpp::generateUUID());

        session->setData("viewCount",1);

        session->setSecure(false);

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


// Test queries
void testQuery(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    auto q = req.getQueriesAll();
    std::stringstream ss;
    ss << "<html><body><h1>Query test</h1>";
    for(auto& qq: q)
    {
        ss << "<p>" << qq.first << ": " << qq.second << "</p>";
    }
    ss << "</body></html>";
    res.setBodyHTML(ss.str());
}


void testFileUpload(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    std::stringstream ss;
    ss << "<html><body><h1>File upload test</h1>";
    std::cout << "File upload test" << std::endl;
    for(auto& formData : req.getFormData())
    {
        if(formData.isBinary())
        {
            ss << "<p>File: " << formData.getFileName() << "</p>";
            ss << "<p>Size: " << formData.getBinaryDataLength() << "</p>";
            ss << "<p>Content type: " << formData.getContentType() << "</p>";
            std::ofstream file("./root/" + formData.getFileName(), std::ios::binary);
            file.write((char*)formData.getBinaryData(), formData.getBinaryDataLength());
            file.close();
        }
        else
        {
            ss << "<p>Text: " << formData.getTextData() << "</p>";
        }
    }
    ss << "</body></html>";
    res.setBodyHTML(ss.str());
}


