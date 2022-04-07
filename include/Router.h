#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <regex>

#include "Common.h"




namespace restcpp
{

    class Server;
    class HTTPRequest;
    class HTTPResponse;

    /**
     * @brief Router class to store both static and dynamic routes for urls
     * 
     */
    class Router
    {
        public:
            struct pathParam
            {
                std::string m_path;
                std::string m_name;
                char m_endChar = 0;
            };
            /**
             * @brief Route struct to hold routes, callBack function will be called when a route with given URL Path and method found.
             * Also stores path parameters to use path parameters as variables
             * 
             */
            struct route
            {
                route(const std::string& URLPath, const METHOD& method, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack)
                {
                    m_URLPath = URLPath;
                    m_method = method;
                    m_callBack = callBack;
                    parsePathParams(URLPath,m_pathParams);
                }
                std::string m_URLPath;
                METHOD m_method;
                std::function<void(const HTTPRequest&,HTTPResponse&)> m_callBack; 
                std::vector<pathParam> m_pathParams;
            };

            void addRoute(const std::string& URLPath, const METHOD& method, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack) { m_routes.push_back(route(URLPath,method,callBack)); };
            void addStaticRoute(std::string URLPath, std::string folderPath) { m_staticRoutes[URLPath] = folderPath; };
            const std::unordered_map<std::string, std::string>& getStaticRoutes() const { return m_staticRoutes; };
            const std::vector<route>& getDefinedRoutes() const { return m_routes; }; 
            friend class Server;
            friend class HTTPRequest;
        private:
            static inline void parsePathParams(std::string path, std::vector<pathParam>& params,size_t lastPos = 0)     
            {
                if(!std::regex_search(path,std::regex("\\{([^}]+)\\}")))
                    return;
                auto pos = path.find_first_of('{',lastPos);
                auto endPos = path.find_first_of('}',pos);
                if(endPos < pos)
                    return;
                auto name = path.substr(pos + 1,endPos - pos - 1);
                auto URLPath = path.substr(0, pos);
                if(path.length() <= endPos + 1)
                    params.push_back({URLPath, name, 0});
                else
                    params.push_back({URLPath, name, path[endPos + 1]});
                path.erase(pos + 1,endPos - pos - 1);
                parsePathParams(path, params,endPos - name.length());
            };
            std::unordered_map<std::string, std::string> m_staticRoutes;
            std::vector<route> m_routes;
    };

}

