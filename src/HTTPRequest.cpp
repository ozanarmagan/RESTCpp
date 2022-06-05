#include "../include/HTTPRequest.h"

namespace restcpp
{
    namespace {
        /**
         * @brief Helper function to split a std::string by a delimeter char
         * 
         * @param to_split 
         * @param delimiter 
         * @return std::vector<std::string> 
         */
        std::vector<std::string> splitByChar(std::string to_split, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;

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

        /**
         * @brief Helper function to split a std::string by a delimeter std::string
         * 
         * @param str 
         * @param delimeter 
         * @return std::vector<std::string> 
         */
        std::vector<std::string> splitByStr(std::string str,std::string delimeter)
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

        /**
         * @brief Helper function to decode given url
         * 
         * @param in 
         * @return std::string 
         */
        std::string decodeUri(const char* in) 
        {
            std::string res;
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

    bool HTTPRequest::parseRequest(const std::string& data)
    {
        try
        {
            int cursor = 0,cursorPrev = 0;

            m_time = std::time(nullptr);

            std::vector<std::string> vec = splitByChar(data,'\r');

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
            else if(methodStr == "TRACE")
                m_method = METHOD::TRACE;
            else if(methodStr == "CONNECT")
                m_method = METHOD::CONNECT;
            else
                return false;
            

            cursor = data.find_first_of(" ",cursorPrev);
            m_path = data.substr(cursorPrev,cursor - cursorPrev);

            if(m_path.find('?') != std::string::npos)
            {
                std::string queryStr = m_path.substr(m_path.find("?") + 1);
                m_path = m_path.substr(0,m_path.find("?"));
                
                queryStr = decodeUri(queryStr.c_str());
                std::vector<std::string> queries = splitByChar(queryStr,'&');

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
            if(protocolStr.find("HTTP/") != std::string::npos)
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
                if(pos != std::string::npos)
                {
                    auto key = vec[i].substr(1,pos - 1);
                    if(key == "Cookie")
                    {
                        auto val = vec[i].substr(pos + 1, vec[i].length() - pos - 1);
                        val.erase(val.begin(), std::find_if(val.begin(), val.end(), std::bind1st(std::not_equal_to<char>(), ' ')));
                        std::vector<std::string> cookies = splitByChar(val,';');
                        for(auto& cookie : cookies)
                        {
                            auto pos2 = cookie.find("=");
                            if(pos2 != std::string::npos)
                            {
                                auto key = cookie.substr(0,pos2);
                                auto val = cookie.substr(pos2 + 1, cookie.length() - pos2 - 1);
                                if(key == "sessionid")
                                    m_sessionID = val;
                                else
                                    m_cookies[key] = val;
                            }
                        }
                    }
                    else
                    {
                        auto val = vec[i].substr(pos + 1, vec[i].length() - pos - 1);
                        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                        m_headers[key] = val;
                    }
                }
            }

            if(m_headers["content-length"] != "")
                for(const char& c : m_headers["content-length"])
                    if(c < '0' || c > '9')
                        return false;

            if(m_headers["content-length"] != "")
            {
                if(m_headers["Content-Type"].find("multipart/form-data") != std::string::npos)
                {
                    std::string body = data.substr(data.find("\r\n\r\n") + 4);
                    if(m_headers["transfer-encoding"] == "chunked")
                    {
                        std::string ans;
                        std::vector<std::string> chunks = splitByStr(body,"\r\n");
                        bool isEven = false;
                        for(auto& chunk : chunks)
                        {
                            if(isEven)
                                ans += chunk;
                            isEven = !isEven;
                        }

                        body = ans;
                    }
                    std::string contentType = m_headers["content-type"];
                    std::string boundary = contentType.substr(contentType.find("boundary=") + 9);
                    body = body.substr(body.find("--" + boundary));
                    std::vector<std::string> form = splitByStr(body,"--" + boundary);
                    form.pop_back();


                    for(auto& obj : form)
                    {
                        std::string fileName = "";
                        std::string name = "";
                        std::string data = "";
                        std::string contentType = "";
                        bool isBinary = false;
                        if(obj.find("Content-Type") != std::string::npos)
                        {
                            auto pos = obj.find("Content-Type:") + 14;
                            contentType = obj.substr(pos,obj.find("\r\n",pos) - pos);
                            isBinary = (contentType.find("text/") == std::string::npos);
                        }

                        if(obj.find("filename=") != std::string::npos)
                        {
                            auto pos = obj.find("filename=") + 10;
                            fileName = obj.substr(pos,obj.find("\"",pos) - pos);
                        }

                        auto pos = obj.find("name=") + 6;
                        name = obj.substr(pos,obj.find("\"",pos) - pos);

                        auto formDataVal = obj.substr(obj.find("\r\n\r\n") + 4);
                        if(!isBinary)
                            m_formData.push_back(FormData(name, fileName, formDataVal, contentType));
                        else
                        {
                            byte* byteArray = new byte[formDataVal.length()];; 
                            memcpy(byteArray, formDataVal.c_str(), sizeof(byte) * formDataVal.length());
                            m_formData.push_back(FormData(name, fileName, byteArray, formDataVal.length(), contentType));
                        }
                    }
                }

                else if(m_headers["content-type"].find("application/x-www-form-urlencoded") != std::string::npos)
                {
                    std::string body = data.substr(data.find("\r\n\r\n") + 4);
                    if(m_headers["transfer-encoding"] == "chunked")
                    {
                        std::string ans;
                        std::vector<std::string> chunks = splitByStr(body,"\r\n");
                        bool isEven = false;
                        for(auto& chunk : chunks)
                        {
                            if(isEven)
                                ans += chunk;
                            isEven = !isEven;
                        }

                        body = ans;
                    }
                    body = decodeUri(body.c_str());
                    std::vector<std::string> objects = splitByChar(body,'&');
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
                    if(m_headers["transfer-encoding"] != "chunked")
                    {
                        std::string body = data.substr(data.find("\r\n\r\n") + 4);
                        memccpy(m_rawBodyData, body.c_str(), 0, body.length());
                    }
                    else
                    {
                        std::string body = data.substr(data.find("\r\n\r\n") + 4);
                        std::vector<std::string> chunks = splitByStr(body,"\r\n");
                        std::string ans;
                        bool isEven = false;
                        size_t size = 0;
                        for(auto& chunk : chunks)
                        {
                            if(isEven)
                            {
                                ans += chunk;
                            }

                            isEven = !isEven;
                        }

                        m_rawBodyData = new byte[ans.length()];
                        memcpy(m_rawBodyData, ans.c_str(), sizeof(byte) * ans.length());
                    }
                }
            }


            return true;
        }
        catch (std::runtime_error ex)
        {
            std::cout << ex.what() << "\nException while parsing request";
            return false;
        }

        return true;
    }




    /**
     * @brief Construct a new HTTPRequest::HTTPRequest object
     * 
     * @param data raw data to parse and create HTTPRequest object
     */
    HTTPRequest::HTTPRequest(const std::string& data)
    {
        parseRequest(data);
    }


}