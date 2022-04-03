#include "../include/HTTPRequest.h"

namespace restcpp
{
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

        string decodeUri(const char* in) 
        {
            string res;
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
            char c, v1, v2;
            if(in != NULL) {
                while((c=*in++) != '\0') {
                    if(c == '%') {
                        if((v1=tbl[(unsigned char)*in++])<0 || 
                        (v2=tbl[(unsigned char)*in++])<0) {
                            return res;
                        }
                        c = (v1<<4)|v2;
                    }
                    res += c;
                }
            }
            return res;
        }
    }


    HTTPRequest::HTTPRequest(const string& data)
    {
        try
        {
            int cursor = 0,cursorPrev = 0;

            m_time = std::time(nullptr);

            vector<string> vec = splitByChar(data,'\r');

            cursor = vec[0].find_first_of(' ');

            auto methodStr = vec[0].substr(0, cursor);
            cursorPrev = cursor + 1;
            if(methodStr == "GET")
                m_method = METHOD::GET;
            else if(methodStr == "POST")
                m_method = METHOD::POST;
            else if(methodStr == "PUT")
                m_method = METHOD::PUT;
            else if(methodStr == "PATCH")
                m_method = METHOD::PATCH;
            else if(methodStr == "DELETE")
                m_method = METHOD::DEL;
            else if(methodStr == "HEAD")
                m_method = METHOD::HEAD;
            else if(methodStr == "OPTIONS")
                m_method = METHOD::OPTIONS;

            cursor = data.find_first_of(" ",cursorPrev);
            m_path = data.substr(cursorPrev,cursor - cursorPrev);

            if(m_path.find('?') != string::npos)
            {
                string queryStr = m_path.substr(m_path.find("?") + 1);
                m_path = m_path.substr(0,m_path.find("?"));
                
                queryStr = decodeUri(queryStr.c_str());
                vector<string> queries = splitByChar(queryStr,'&');

                for(auto& query : queries)
                {
                    auto pos = query.find("=");
                    auto key = query.substr(0,pos);
                    auto val = query.substr(pos + 1, query.length() - pos - 1);
                    m_queries[key] = val;
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
                m_requestVersion = {majorV, minorV};
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
                    m_headers[key] = val;
                }
            }

            if(m_headers["Content-Length"] != "")
            {
                if(m_headers["Content-Type"].find("multipart/form-data") != string::npos)
                {
                    string contentType = m_headers["Content-Type"];
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
                            m_formData.push_back(FormData(name, fileName, data, contentType));
                        else
                        {
                            std::shared_ptr<byte> byteArray = std::make_shared<byte>(data.length()); 
                            memcpy(byteArray.get(), data.c_str(), sizeof(byte) * data.length());
                            m_formData.push_back(FormData(name, fileName, byteArray, contentType));
                        }

                        
                    }
                }

                else if(m_headers["Content-Type"].find("application/x-www-form-urlencoded") != string::npos)
                {
                    string body = data.substr(data.find("\r\n\r\n") + 4);
                    body = decodeUri(body.c_str());
                    vector<string> objects = splitByChar(body,'&');
                    for(auto& object : objects)
                    {
                        auto pos = object.find("=");
                        auto key = object.substr(0,pos);
                        auto val = object.substr(pos + 1, object.length() - pos - 1);
                        m_formData.push_back(FormData(key, "", val));
                    }
                }

                else
                {
                    string body = data.substr(data.find("\r\n\r\n") + 4);
                    auto m_rawBodyData =  std::make_unique<byte>(body.length());
                    memcpy(m_rawBodyData.get(), body.c_str(), sizeof(byte) * body.length());
                }
            }

            


        }
        catch (runtime_error ex)
        {
            std::cout << ex.what() << "\nException while parsing request";
        }
    }
}