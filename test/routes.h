#pragma once

#include<iostream>

#include "../include/Server.h"
#include "dbConnection.h"


bool isNumber(const std::string& str)
{
    for(auto& c: str)
    {
        if(!isdigit(c) && c != '.')
            return false;
    }

    return true;
}

void getProducts(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    mongocxx::collection col = db["products"];
    bsoncxx::builder::stream::document doc = document{};
    auto queries = req.getQueriesAll();

    for(auto& [key,val] : queries)
    {
        if(isNumber(val))
            doc << key << open_document << "$gte" << std::strtod(val.c_str(),NULL) << close_document; 
        else
            doc << key << val;
        
    }

    mongocxx::cursor cursor = col.find(doc.view());
    auto array_builder = bsoncxx::builder::basic::array{};
    for(auto doc_ : cursor )
            array_builder.append(doc_);

    auto doc_ =  array_builder.extract();
    res.setBodyJSON(bsoncxx::to_json(doc_.view()));
}

void testParams(const restcpp::HTTPRequest& req, restcpp::HTTPResponse& res)
{
    res.setBodyText("Greetings, Mr./Mrs. " + req.getParam("name") +  " " + req.getParam("surname"));
}
