#include "HTTPRequest.h"

namespace {
    vector<string> splitByChar(string to_split, char delimiter) {
        vector<string> tokens;
        string token;

        for (const auto& c: to_split) {
            if (c != delimiter)
            token += c;
            else {
            if (token.length()) tokens.push_back(token);
            token.clear();
            }
        }

        if (token.length()) tokens.push_back(token);
        return tokens;
    }

    vector<string> splitByStr(string str,string delimeter)
    {
        std::vector<std::string> splittedStrings = {};
        size_t pos = 0;

        while ((pos = str.find(delimeter)) != std::string::npos)
        {
            std::string token = str.substr(0, pos);
            if (token.length() > 0)
                splittedStrings.push_back(token);
            str.erase(0, pos + delimeter.length());
        }

        if (str.length() > 0)
            splittedStrings.push_back(str);
        return splittedStrings;
    }

    int decodeUri(char* out, const char* in) 
    {
        static const char tbl[256] = {
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            0, 1, 2, 3, 4, 5, 6, 7,  8, 9,-1,-1,-1,-1,-1,-1,
            -1,10,11,12,13,14,15,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,10,11,12,13,14,15,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1
        };
        char c, v1, v2, *beg=out;
        if(in != NULL) {
            while((c=*in++) != '\0') {
                if(c == '%') {
                    if((v1=tbl[(unsigned char)*in++])<0 || 
                    (v2=tbl[(unsigned char)*in++])<0) {
                        *beg = '\0';
                        return -1;
                    }
                    c = (v1<<4)|v2;
                }
                *out++ = c;
            }
        }
        *out = '\0';
        return 0;
    }
}


HTTPRequest::HTTPRequest(const string& data)
{

    int cursor = 0,cursorPrev = 0;

    mTime = std::time(nullptr);

    vector<string> vec = splitByChar(data,'\r');

    cursor = vec[0].find_first_of(' ');

    auto methodStr = vec[0].substr(0, cursor);
    cursorPrev = cursor + 1;
    if(methodStr == "GET")
        mMethod = METHOD::GET;
    else if(methodStr == "POST")
        mMethod = METHOD::POST;
    else if(methodStr == "PUT")
        mMethod = METHOD::PUT;
    else if(methodStr == "PATCH")
        mMethod = METHOD::PATCH;
    else if(methodStr == "DELETE")
        mMethod = METHOD::DEL;

    cursor = data.find_first_of(" ",cursorPrev);
    mPath = data.substr(cursorPrev,cursor - cursorPrev);

    if(mPath.find('?') != string::npos)
    {
        string queryStr = mPath.substr(mPath.find("?") + 1);
        mPath = mPath.substr(0,mPath.find("?"));
        char *buffer = new char[queryStr.length()];
        decodeUri(buffer,queryStr.c_str());
        queryStr = string(buffer);
        delete [] buffer;
        vector<string> queries = splitByChar(queryStr,'&');

        for(auto& query : queries)
        {
            auto pos = query.find("=");
            auto key = query.substr(0,pos);
            auto val = query.substr(pos + 1, query.length() - pos - 1);
            mQueries[key] = val;
        }
    }

    cursorPrev = cursor + 1;
    auto protocolStr = vec[0].substr(cursorPrev,vec[0].length() - cursorPrev);   
    if(protocolStr.find("HTTP/") != string::npos)
    {
        auto pos = protocolStr.find("HTTP/") + 5;
        short majorV = protocolStr.substr(pos, protocolStr.find(".",pos) - pos)[0] - 48;
        auto pos2 = protocolStr.find(".",pos);
        short minorV = protocolStr.substr(pos2 + 1)[0] - 48;
        mRequestVersion = {majorV, minorV};
    }


    int i = 1;
    for(;i < vec.size();i++)
    {
        if(vec[i] == "\n")
            break;
        auto pos = vec[i].find(":");
        if(pos != string::npos)
        {
            auto key = vec[i].substr(1,pos - 1);
            auto val = vec[i].substr(pos + 1, vec[i].length() - pos - 1);
            mHeaders[key] = val;
        }
    }

    if(mHeaders["Content-Length"] != "")
    {
        if(mHeaders["Content-Type"].find("multipart/form-data") != string::npos)
        {
            string contentType = mHeaders["Content-Type"];
            string boundary = contentType.substr(contentType.find("boundary=") + 9);
            string body = data.substr(data.find("--" + boundary));
            vector<string> form = splitByStr(body,"--" + boundary);
            form.pop_back();


            for(auto& obj : form)
            {
                string fileName = "";
                string name = "";
                string data = "";
                string contentType = "";
                bool isBinary = false;
                if(obj.find("Content-Type") != string::npos)
                {
                    auto pos = obj.find("Content-Type:") + 14;
                    contentType = obj.substr(pos,obj.find("\r\n",pos) - pos);
                    isBinary = contentType.find("text/") == string::npos;
                }

                if(obj.find("filename=") != string::npos)
                {
                    auto pos = obj.find("filename=") + 10;
                    fileName = obj.substr(pos,obj.find("\"",pos) - pos);
                }

                auto pos = obj.find("name=") + 6;
                name = obj.substr(pos,obj.find("\"",pos) - pos);

                data = obj.substr(obj.find("\r\n\r\n") + 4);

                if(!isBinary)
                    mFormData.push_back(new FormData(name, fileName, data, contentType));
                else
                {
                    byte* byteArray = new byte[data.length()];
                    memcpy(byteArray, data.c_str(), sizeof(byte) * data.length());
                    mFormData.push_back(new FormData(name, fileName, byteArray, contentType));
                }

                
            }
        }

        else if(mHeaders["Content-Type"].find("application/x-www-form-urlencoded") != string::npos)
        {
            string body = data.substr(data.find("\r\n\r\n") + 4);
            char *buffer = new char[body.length()];
            decodeUri(buffer,body.c_str());
            body = string(buffer);
            delete [] buffer;
            vector<string> objects = splitByChar(body,'&');
            for(auto& object : objects)
            {
                auto pos = object.find("=");
                auto key = object.substr(0,pos);
                auto val = object.substr(pos + 1, object.length() - pos - 1);
                mFormData.push_back(new FormData(key, "", val));
            }
        }

        else
        {
            string body = data.substr(data.find("\r\n\r\n") + 4);
            mRawBodyData = new byte[body.length()];
            memcpy(mRawBodyData, body.c_str(), sizeof(byte) * body.length());
        }
    }

}