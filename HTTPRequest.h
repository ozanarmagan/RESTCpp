#pragma once
#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <string>
#include <unordered_map>
#include "Common.h"
#include "FormData.h"

using std::string;
using std::vector;
using std::unordered_map;
using std::replace;

class HTTPRequest
{
    public:
        HTTPRequest(const string& data);
        ~HTTPRequest() { for( auto d : mFormData) { delete d; } delete mRawBodyData; };
        const METHOD fGetMethod() const { return mMethod; };
        const HTTP_PROTOCOL fGetProtocol() const { return mProtocol; };
        const time_t fGetRequestTime() const { return mTime; };
        const string fGetUserAgent() const { return mHeaders.at("User-Agent"); };
        const string fGetHostName() const { return mHeaders.at("Host"); };
        const unordered_map<string,string> fGetHeaders() const { return mHeaders; };
        const unordered_map<string,string> fGetQueries() const { return mQueries; };
    private:
        METHOD mMethod;
        HTTP_PROTOCOL mProtocol;
        time_t mTime;
        string mPath;
        size_t contentLength;
        unordered_map<string,string> mQueries;
        vector<FormData*> mFormData;
        unordered_map<string,string> mHeaders;
        byte* mRawBodyData;
};