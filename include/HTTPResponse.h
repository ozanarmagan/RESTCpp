#pragma once


#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>
#include <set>

#include "Common.h"
#include "FormData.h"
#include "Cookie.h"

namespace restcpp
{

    /**
     * @brief Class to create,store and utilitize HTTP Responses
     * 
     */
    class HTTPResponse
    {
        public:
            HTTPResponse() {   };
            void setStatus(const short& code) { m_statusCode = code; };
            void setHTTPVersion(const HTTP_VERSION& version) { m_requestVersion = version; };
            void addHeader(const std::string& key, const std::string& value) {  m_headers[key] = value; };
            void addCookie(const Cookie& cookie) { m_cookies.insert(cookie); };
            void deleteHeader(const std::string& key) { m_headers.erase(key); };
            void setBodyText(const std::string& body) { m_headers["Content-Type"] = "text/plain"; m_requestBody = body; };
            void setBodyHTML(const std::string& serializedHTML) { m_headers["Content-Type"] = "text/html"; m_requestBody = serializedHTML; };
            void setBodyFormData(const std::vector<FormData*> form);
            void setBodyFile(const std::string& fileName);
            void setBodyJSON(const std::string& serializedJSON) { m_headers["Content-Type"] = "application/json"; m_requestBody = serializedJSON; };
            void setHeaderOnly(bool val) { m_headerOnly = val; };
            void setContentType(const std::string& contentType) { m_headers["Content-Type"] = contentType; };
            void deleteCookie(const std::string& name) { m_cookies.erase(name); };
            const HTTP_VERSION getHTTPVersion() const { return m_requestVersion; };
            const short getStatusCode() const { return m_statusCode; }; 
            const std::unordered_map<std::string,std::string> getHeaders() const { return m_headers; };
            const std::string getBody() const { return m_requestBody; };
            const bool isHeaderOnly() const { return m_headerOnly; };
            const std::string serializeResponse() const;
        private:
            bool m_headerOnly = false;
            HTTP_VERSION m_requestVersion = {1, 1};
            short m_statusCode;
            std::unordered_map<std::string,std::string> m_headers;
            std::string m_requestBody;
            std::set<Cookie> m_cookies;
            const std::string serializeCookies() const;
    };




}