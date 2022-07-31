#pragma once

#ifdef _WIN32
    #include <WinSock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/ip.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <cerrno>
    #include <poll.h>
#endif

#include <functional>
#include <thread>
#include <ctime>
#include <fstream>
#include <string>
#include <tuple>
#include <cstring>
#include <filesystem>

#include "Common.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "ThreadPool.h"
#include "Router.h"
#include "Proxy.h"

namespace restcpp
{

    /**
     * @brief Main class for server 
     * 
     */
    class Server
    {
        public:
            Server(const uint16_t& port = 8080) : m_port(port),m_router() { init(); };
            Server(const std::string& certFilePath, const std::string& pemFilePath, const uint16_t& port = 8080){   };
            void addRoute(const std::string& URLPath, const METHOD& method, const std::function<void(const HTTPRequest&,HTTPResponse&)>& callBack) { m_router.m_routes.push_back(Router::route(URLPath, method, callBack)); };
            void addStaticRoute(std::string URLPath, std::string folderPath) {
                 m_router.m_staticRoutes[URLPath] = folderPath;
                 for(const auto& p : std::filesystem::directory_iterator(folderPath))
                    if(p.is_directory())
                        addStaticRoute(URLPath + p.path().filename().string() + "/", p.path().string() + "/");
             };
            void run();
            void stop();
            void setLogging(bool value) { m_log = value; };
        private:
            SOCKET m_sock,m_acceptSocket;
            sockaddr_in m_serverAddr;
            bool m_log; 
            bool m_https;
            uint16_t m_port;
            Router m_router;
            void init();
            void onRequest(SOCKET socket);
            const std::string recieveNext(SOCKET socket);
            std::shared_ptr<HTTPResponse> processRequest(const std::string& rawData);
            std::shared_ptr<HTTPRequest> lastRequest;
            std::string certFile,pemFile;
            void sendResponse(std::shared_ptr<HTTPResponse>& response,const SOCKET& sock);
            static void h_setMainHeaders(std::shared_ptr<HTTPResponse> res);
            static void h_setOptions(std::shared_ptr<HTTPRequest> req, std::shared_ptr<HTTPResponse> res,Router& router);
            static bool h_searchStaticRoutes(std::shared_ptr<HTTPResponse> res, std::string path,std::string fileName,Router& router);
            static bool h_searchDefinedRoutes(const std::shared_ptr<HTTPRequest>& req, const std::shared_ptr<HTTPResponse>& res, const std::string& path,const Router& router);
            static void h_processRouter(const std::shared_ptr<HTTPRequest>& req, const std::shared_ptr<HTTPResponse>& res, Router& router);
            static void h_closeSocket(const SOCKET& sock);
            static void h_sendToSocket(const SOCKET& sock,const std::string& message);

    };

}