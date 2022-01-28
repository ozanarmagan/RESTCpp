#pragma once


#include <iostream>
#include <unordered_map>

#include "Common.h"


using std::unordered_map;
using std::string;

class HTTPResponse
{
    private:
        HTTP_PROTOCOL mProtocol;
        short statusCode;
        unordered_map<string,string> mHeaders;

};