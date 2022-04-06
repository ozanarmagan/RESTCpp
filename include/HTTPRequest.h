#pragma once
#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <cstring>
#include <string>
#include <unordered_map>
#include <memory>
#include "Common.h"
#include "FormData.h"
#include "Router.h"

namespace restcpp
{
    using std::string;
    using std::vector;
    using std::unordered_map;
    using std::replace;
    using std::memcpy;


    /**
     * @brief Class to create,store and parse HTTP Requests
     * 
     */
    class HTTPRequest
    {
        public:
            HTTPRequest() {     }
            HTTPRequest(const string& data);
            const METHOD getMethod() const { return m_method; }
            const HTTP_VERSION getHTTPVersion() const { return m_requestVersion; }
            const time_t& getRequestTime() const { return m_time; }
            const string getUserAgent() const { if(m_headers.count("User-Agent")) return m_headers.at("User-Agent"); else return ""; }
            const string getHostName()  const { if(m_headers.count("Host")) return m_headers.at("Host"); else return ""; }
            const string& getPath() const { return m_path; }
            const unordered_map<string,string> getHeadersAll() const { return m_headers; }
            const unordered_map<string,string> getQueriesAll() const { return m_queries; }
            const unordered_map<string,string> getParamsAll()  const { return m_pathParams; }
            const string getHeader(const string& header) const { if(m_headers.count(header)) return m_headers.at(header); else return ""; }
            const string getQuery(const string& query) const { if(m_queries.count(query)) return m_queries.at(query);  else return "";}
            const string getParam(const string& param) const { if(m_pathParams.count(param)) return m_pathParams.at(param); else return ""; }
            const vector<FormData> getFormData() const { return m_formData; }
            void setMethod(const METHOD& method) { m_method = method; } 
            void setHTTPVersion(const HTTP_VERSION& version) { m_requestVersion = version; } 
            void setRequestTime(const time_t& time) { m_time = time; } 
            void setUserAgent(const string& agent) { m_headers["User-Agent"] = agent; } 
            void setHost(const string& host) { m_headers["Host"] = host; } 
            void setPath(const string& path) { m_path = path; }
            void setHeader(const string& key, const string& value) { m_headers[key] = value; }
            void setQuery(const string& key, const string& value) {  m_queries[key] = value; }
            void setParam(const string& key, const string& value) {  m_pathParams[key] = value; }
            void setFormData(FormData form) { m_formData.push_back(form); }
        private:
            friend class Server;
            METHOD m_method;
            HTTP_VERSION m_requestVersion;
            time_t m_time;
            string m_path;
            size_t m_contentLength;
            unordered_map<string,string> m_queries;
            vector<FormData> m_formData;
            unordered_map<string,string> m_headers;
            unordered_map<string,string> m_pathParams;
            byte* m_rawBodyData;
    };
}