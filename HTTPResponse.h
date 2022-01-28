#pragma once


#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

#define UUID_SYSTEM_GENERATOR
#include "dependency/uuid.h"
#include "Common.h"
#include "FormData.h"

using std::unordered_map;
using std::string;



class HTTPResponse
{
    public:
        HTTPResponse();
        void fSetStatus(const short& code) { mStatusCode = code; };
        void fSetProtocol(const HTTP_PROTOCOL& protocol) { mProtocol = protocol; };
        void fAddHeader(const string& key, const string& value) {  mHeaders[key] = value; };
        void fDeleteHeader(const string& key) { mHeaders.erase(key); };
        void fSetBody(const string& body) { mRequestBody = body; };
        void fSetBody(const vector<FormData*> form);
        void fSetBodyFile(const string& fileName);
        const HTTP_PROTOCOL fGetProtocol() const { return mProtocol; };
        const short fGetStatusCode() const { return mStatusCode; }; 
        const unordered_map<string,string> fGetHeaders() const { return mHeaders; };
        const string fGetBody() const { return mRequestBody; };
        const string serializeResponse() const;
    private:
        HTTP_PROTOCOL mProtocol;
        short mStatusCode;
        unordered_map<string,string> mHeaders;
        string mRequestBody;
};

const string HTTPResponse::serializeResponse() const 
{
    string res;
    for(auto& [key,value] : mHeaders)
        res += key + ": " + value + "\r\n";
    res += "Content-Length: " + std::to_string(mRequestBody.length());
    res += "\r\n";
    res += mRequestBody;

    return res;
}

void HTTPResponse::fSetBody(const vector<FormData*> form)
{
    string boundary = "BOUNDARY__" + uuids::to_string (uuids::uuid_system_generator{}());
    mHeaders["Content-Type"] = " multipart/form-data; boundary=" + boundary;
    for(auto& data : form)
    {
        mRequestBody += "--" + boundary + "\r\n";
        mRequestBody += "Content-Disposition: form-data; ";
        mRequestBody += "name=\"" + data->fGetName() + "\"";
        if(data->fGetFileName() != "")
            mRequestBody += "name=\"" + data->fGetFileName() + "\"";
        if(data->fGetContentType() != "")
            mRequestBody += string("\r\n") + "Content-Type: " + data->fGetContentType();
        mRequestBody += "\r\n\r\n";
        if(data->fIsBinary())
        {
            const byte* dataHead = data->fGetBinaryData();
            auto length = data->fGetBinaryDataLength();
            for(int i = 0;i < length;i++)
                mRequestBody += dataHead[i];
        }
        else
            mRequestBody += data->fGetTextData();
        
        mRequestBody += "\r\n";
    }

    mRequestBody += "--" + boundary + "--";
}

void HTTPResponse::fSetBodyFile(const string& fileName)
{
    auto MIME = gMIMETable[fileName.substr(fileName.find_last_of('.') + 1)];
    mHeaders["Content-Type"] = " " + MIME;

    std::ifstream file(fileName,std::ios::binary);

    if(file.good())
    {
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        file.read(mRequestBody.data(), size);
    }
    
    file.close();
}