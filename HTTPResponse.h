#pragma once


#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>

#include "Common.h"
#include "FormData.h"

using std::unordered_map;
using std::string;
using std::vector;
using std::to_string;
using std::stringstream;


namespace {
    string gGenerateBoundary()
    {
        string res = "-----------------";
        for(int i = 0;i < 25;i++)
        {
            srand(time(nullptr));
            int r = rand() % 10;
            res += (char)(r + 48);
        }
        return res;
    }
}

class HTTPResponse
{
    public:
        HTTPResponse() {   };
        void fSetStatus(const short& code) { mStatusCode = code; };
        void fSetHTTPVersion(const HTTP_VERSION& version) { mRequestVersion = version; };
        void fAddHeader(const string& key, const string& value) {  mHeaders[key] = value; };
        void fDeleteHeader(const string& key) { mHeaders.erase(key); };
        void fSetBody(const string& body) { mRequestBody = body; };
        void fSetBody(const vector<FormData*> form);
        void fSetBodyFile(const string& fileName);
        const HTTP_VERSION fGetHTTPVersion() const { return mRequestVersion; };
        const short fGetStatusCode() const { return mStatusCode; }; 
        const unordered_map<string,string> fGetHeaders() const { return mHeaders; };
        const string fGetBody() const { return mRequestBody; };
        const string fSerializeResponse() const;
    private:
        HTTP_VERSION mRequestVersion = {1, 1};
        short mStatusCode;
        unordered_map<string,string> mHeaders;
        string mRequestBody;
};



