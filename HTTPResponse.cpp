#include "HTTPResponse.h"

const string HTTPResponse::fSerializeResponse() const 
{
    string res = "";
    string statusDesc = gGetStatusDescription(mStatusCode);
    res += string("HTTP/") + std::to_string(mRequestVersion.majorVersion) + string(".") + std::to_string(mRequestVersion.minorVersion) + " ";
    res += to_string(mStatusCode) + " " + statusDesc + "\r\n";
    for(auto& [key,value] : mHeaders)
        res += key + ": " + value + "\r\n";
    if(mRequestBody.length() > 0)
        res += "Content-Length: " + std::to_string(mRequestBody.length());
    if(mRequestBody.length() > 0 && !mHeaderOnly)
    res += string("\r\n\r\n") +  mRequestBody;
    return res;
}

void HTTPResponse::fSetBody(const vector<FormData*> form)
{
    string boundary = "BOUNDARY__" + gGenerateBoundary();
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
    auto MIME = gMIMETable.at(fileName.substr(fileName.find_last_of('.') + 1));
    mHeaders["Content-Type"] = " " + MIME;
    std::ifstream file(fileName);

    if(file.good())
    {
        stringstream stream;
        stream << file.rdbuf();
        mRequestBody = stream.str();
    }
    
    file.close();
}