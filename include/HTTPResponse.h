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

namespace restcpp
{
    using std::unordered_map;
    using std::string;
    using std::vector;
    using std::to_string;
    using std::stringstream;

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
            void addHeader(const string& key, const string& value) {  m_headers[key] = value; };
            void deleteHeader(const string& key) { m_headers.erase(key); };
            void setBodyText(const string& body) { m_headers["Content-Type"] = "text/plain"; m_requestBody = body; };
            void setBodyHTML(const string& serializedHTML) { m_headers["Content-Type"] = "text/html"; m_requestBody = serializedHTML; };
            void setBodyFormData(const vector<FormData*> form);
            void setBodyFile(const string& fileName);
            void setBodyJSON(const string& serializedJSON) { m_headers["Content-Type"] = "application/json"; m_requestBody = serializedJSON; };
            void setHeaderOnly(bool val) { m_headerOnly = val; };
            void setContentType(const string& contentType) { m_headers["Content-Type"] = contentType; };
            const HTTP_VERSION getHTTPVersion() const { return m_requestVersion; };
            const short getStatusCode() const { return m_statusCode; }; 
            const unordered_map<string,string> getHeaders() const { return m_headers; };
            const string getBody() const { return m_requestBody; };
            const bool isHeaderOnly() const { return m_headerOnly; };
            const string serializeResponse() const;
        private:
            bool m_headerOnly = false;
            HTTP_VERSION m_requestVersion = {1, 1};
            short m_statusCode;
            unordered_map<string,string> m_headers;
            string m_requestBody;
    };




}