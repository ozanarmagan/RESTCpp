#pragma once


#include <ctime>

#include "Common.h"

namespace restcpp
{
    class Cookie
    {
        public:
            Cookie() {};
            Cookie(const std::string& key, const std::string& value, const std::string& path, const std::string& domain, const std::time_t& expires, const bool& secure, const bool& httpOnly)
            {
                m_key = key;
                m_value = value;
                m_path = path;
                m_domain = domain;
                m_expires = expires;
                m_secure = secure;
                m_httpOnly = httpOnly;
            };
            Cookie(const std::string& key, const std::string& value, const std::string& path, const std::string& domain, const std::time_t& expires, const bool& secure)
            {
                m_key = key;
                m_value = value;
                m_path = path;
                m_domain = domain;
                m_expires = expires;
                m_secure = secure;
            };
            Cookie(const std::string& key, const std::string& value, const std::string& path, const std::string& domain, const std::time_t& expires)
            {
                m_key = key;
                m_value = value;
                m_path = path;
                m_domain = domain;
                m_expires = expires;
            };
            Cookie(const std::string& key, const std::string& value, const std::string& path, const std::string& domain)
            {
                m_key = key;
                m_value = value;
                m_path = path;
                m_domain = domain;
            };
            Cookie(const std::string& key, const std::string& value, const std::string& path)
            {
                m_key = key;
                m_value = value;
                m_path = path;
            };
            Cookie(const std::string& key, const std::string& value)
            {
                m_key = key;
                m_value = value;
            };
            Cookie(const std::string& key)
            {
                m_key = key;
            };
            const std::string getKey() const { return m_key; };
            const std::string getValue() const { return m_value; };
            const std::string getPath() const { return m_path; };
            const std::string getDomain() const { return m_domain; };
            const std::string getExpiresStr() const {   if(m_expires) {
                                                                        std::tm *ptm = std::gmtime(&m_expires);
                                                                        char buffer[128]; 
                                                                        strftime(buffer, 128, "%a, %d %b %Y %H:%M:%S", ptm); 
                                                                        return std::string(buffer);
                                                                    } 
                                                                    else
                                                                        return ""; };
            const std::time_t getExpires() const { return m_expires; };
            const bool isSecure() const { return m_secure; };
            const bool isHttpOnly() const { return m_httpOnly; };
            void setKey(const std::string& key) { m_key = key; };
            void setValue(const std::string& value) { m_value = value; };
            void setPath(const std::string& path) { m_path = path; };
            void setDomain(const std::string& domain) { m_domain = domain; };
            void setExpires(const std::time_t& expires) { m_expires = expires; };
            void setSecure(const bool& secure) { m_secure = secure; };
            void setHttpOnly(const bool& httpOnly) { m_httpOnly = httpOnly; };
            friend bool operator<(const Cookie& lhs, const Cookie& rhs)
            {
                return lhs.m_key < rhs.m_key;
            }
        private:
            std::string m_key;
            std::string m_value;
            std::string m_path;
            std::string m_domain;
            std::time_t m_expires;
            bool m_secure = false;
            bool m_httpOnly = false;
    };
}