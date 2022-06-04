#pragma once
#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <cstring>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include "Common.h"
#include "FormData.h"
#include "Router.h"
#include "Session.h"

namespace restcpp
{



    /**
     * @brief Class to create,store and parse HTTP Requests
     * 
     */
    class HTTPRequest
    {
        public:
            HTTPRequest() {     }
            HTTPRequest(const std::string& data);
            const METHOD getMethod() const { return m_method; }
            const HTTP_VERSION getHTTPVersion() const { return m_requestVersion; }
            const time_t& getRequestTime() const { return m_time; }
            const std::string getUserAgent() const { if(m_headers.count("User-Agent")) return m_headers.at("User-Agent"); else return ""; }
            const std::string getHostName()  const { if(m_headers.count("Host")) return m_headers.at("Host"); else return ""; }
            const std::string getPath() const { return m_path; }
            const std::unordered_map<std::string,std::string>& getHeadersAll() const { return m_headers; }
            const std::unordered_map<std::string,std::string>& getQueriesAll() const { return m_queries; }
            const std::unordered_map<std::string,std::string>& getParamsAll()  const { return m_pathParams; }
            const std::unordered_map<std::string,std::string>& getCookiesAll() const { return m_cookies; }
            const std::string getHeader(const std::string& header) const { if(m_headers.count(header)) return m_headers.at(header); else return ""; }
            const std::string getQuery(const std::string& query) const { if(m_queries.count(query)) return m_queries.at(query);  else return "";}
            const std::string getParam(const std::string& param) const { if(m_pathParams.count(param)) return m_pathParams.at(param); else return ""; }
            const std::vector<FormData> getFormData() const { return m_formData; }
            const std::string getCookie (const std::string& cookie) const { if(m_cookies.count(cookie)) return m_cookies.at(cookie); else return ""; }
            const std::string& getSessionID() const { return m_sessionID; }
            void setMethod(const METHOD& method) { m_method = method; } 
            void setHTTPVersion(const HTTP_VERSION& version) { m_requestVersion = version; } 
            void setRequestTime(const time_t& time) { m_time = time; } 
            void setUserAgent(const std::string& agent) { m_headers["User-Agent"] = agent; } 
            void setHost(const std::string& host) { m_headers["Host"] = host; } 
            void setPath(const std::string& path) { m_path = path; }
            void setHeader(const std::string& key, const std::string& value) { m_headers[key] = value; }
            void setQuery(const std::string& key, const std::string& value) {  m_queries[key] = value; }
            void setParam(const std::string& key, const std::string& value) {  m_pathParams[key] = value; }
            void setFormData(FormData form) { m_formData.push_back(form); }
            bool parseRequest(const std::string& data);
        private:
            friend class Server;
            METHOD m_method;
            HTTP_VERSION m_requestVersion;
            time_t m_time;
            std::string m_path;
            size_t m_contentLength;
            std::unordered_map<std::string,std::string> m_queries;
            std::vector<FormData> m_formData;
            std::unordered_map<std::string,std::string> m_headers;
            std::unordered_map<std::string,std::string> m_pathParams;
            std::unordered_map<std::string,std::string> m_cookies;
            byte* m_rawBodyData;
            std::string m_sessionID;
    };
}