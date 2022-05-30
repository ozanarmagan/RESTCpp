#pragma once

#include <unordered_map>
#include <any>
#include <chrono>
#include <ctime>

#include "Common.h"
#include "Cookie.h"



namespace restcpp
{
    // HTTP Session class
    class Session
    {
        public:
            Session(const std::string& sessionID, const time_t& lifeTime = 36000, const std::string path = "/", const std::string domain = "", const bool secure = false, const bool httpOnly = true);
            const std::string& getSessionID() const { return m_sessionID; };
            const std::string& getPath() const { return m_path; };
            const std::string& getDomain() const { return m_domain; };
            const bool& getSecure() const { return m_secure; };
            const std::string getExpiresStr() const {   std::tm *ptm = std::gmtime(&m_expireTime);
                                                        char buffer[128]; 
                                                        strftime(buffer, 128, "%a, %d %b %Y %H:%M:%S", ptm); 
                                                        return std::string(buffer); };
            const bool& getHttpOnly() const { return m_httpOnly; };
            const time_t& getExpireTime() const { return m_expireTime; };
            const std::any getData(const std::string& key) const { auto val = m_data.find(key); if(val != m_data.end()) return val->second; else return std::any(); };
            template <typename T> T getData(const std::string& key) const { auto val = m_data.find(key); if(val != m_data.end()) return std::any_cast<T>(val->second); else return T(); };
            void setData(const std::string& key, const std::any& value) { m_data[key] = value; };
            const Cookie toCookie() const;
        private:
            std::string m_sessionID;
            std::string m_path;
            std::string m_domain;
            bool m_secure;
            bool m_httpOnly;
            time_t m_expireTime;
            std::unordered_map<std::string, std::any> m_data;
    };

    inline Session::Session(const std::string& sessionID, const time_t& lifeTime, const std::string path, const std::string domain, const bool secure, const bool httpOnly)
        : m_sessionID(sessionID), m_path(path), m_domain(domain), m_secure(secure), m_httpOnly(httpOnly)
    {
        m_expireTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) + lifeTime;
    }

    inline const Cookie Session::toCookie() const
    {
        return Cookie("sessionid", m_sessionID, m_path, m_domain, m_secure, m_httpOnly);
    }

    
}