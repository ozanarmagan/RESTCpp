#pragma once

#include "Common.h"
#include "Session.h"


namespace restcpp
{
    // Session Manager class
    class SessionManager
    {
        public:
            SessionManager();
            ~SessionManager() { for(auto& s : m_sessions) delete s.second; };
            Session* getSession(const std::string& sessionID);
            Session* addSession(const std::string& sessionID, const time_t& lifeTime, const std::string path = "/", const std::string domain = "", const bool secure = false, const bool httpOnly = true);
            Session* addSession(const Session& session);
            void removeSession(const std::string& sessionID);
            void removeExpiredSessions();
            void clear();
        private:
            std::unordered_map<std::string, Session*> m_sessions;
    };

    inline SessionManager::SessionManager()
    {
    }

    inline Session* SessionManager::getSession(const std::string& sessionID)
    {
        removeExpiredSessions();
        return m_sessions[sessionID];
    }

    inline Session* SessionManager::addSession(const std::string& sessionID, const time_t& lifeTime, const std::string path, const std::string domain, const bool secure, const bool httpOnly)
    {
        m_sessions[sessionID] = new Session(sessionID, lifeTime, path, domain, secure, httpOnly);

        return m_sessions[sessionID];
    }

    inline Session* SessionManager::addSession(const Session& session)
    {
        m_sessions[session.getSessionID()] = new Session(session);

        return m_sessions[session.getSessionID()];
    }


    inline void SessionManager::removeSession(const std::string& sessionID)
    {
        m_sessions.erase(sessionID);
    }

    inline void SessionManager::removeExpiredSessions()
    {
        time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        for (auto it = m_sessions.begin(); it != m_sessions.end();)
        {
            if (it->second->getExpireTime() < currentTime)
            {
                it = m_sessions.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    inline void SessionManager::clear()
    {
        m_sessions.clear();
    }

}