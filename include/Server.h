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

#include "Common.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "ThreadPool.h"
#include "Router.h"

namespace restcpp
{
    using std::function;
    using std::unique_ptr;
    using std::thread;
    using std::time;
    using std::ctime;
    using std::ifstream;
    using std::stringstream;
    using std::memcpy;
    using std::memset;
    /**
     * @brief Main class for server 
     * 
     */
    class Server
    {
        public:
            Server(uint16_t port = 8080) : m_port(port),m_router() { init(); HTTPRequest::m_router = &m_router; };
            void addRoute(const string& URLPath, const METHOD& method, const function<void(const HTTPRequest&,HTTPResponse&)>& callBack) { m_router.m_routes.push_back(Router::route(URLPath, method, callBack)); };
            void addStaticRoute(string URLPath, string folderPath) { m_router.m_staticRoutes[URLPath] = folderPath; };
            void run();
            void stop();
            void setLogging(bool value) { m_log = value; };
        private:
            SOCKET m_sock,m_acceptSocket;
            struct sockaddr_in m_serverAddr;
            bool m_log; 
            uint16_t m_port;
            Router m_router;
            void init();
            void onRequest(SOCKET socket);
            const string recieveNext(SOCKET socket);
            std::shared_ptr<HTTPResponse> processRequest(const string& rawData);
            static void sendResponse(std::shared_ptr<HTTPResponse>& response,const SOCKET& sock);
            static void h_setMainHeaders(std::shared_ptr<HTTPResponse> res);
            static void h_setOptions(std::shared_ptr<HTTPRequest> req, std::shared_ptr<HTTPResponse> res,Router& router);
            static bool h_searchStaticRoutes(std::shared_ptr<HTTPResponse> res, string path,string fileName,Router& router);
            static bool h_searchDefinedRoutes(const std::shared_ptr<HTTPRequest>& req, const std::shared_ptr<HTTPResponse>& res, const string& path,const Router& router);
            static void h_processRouter(const std::shared_ptr<HTTPRequest>& req, const std::shared_ptr<HTTPResponse>& res, Router& router);
            static void h_closeSocket(const SOCKET& sock);
            static void h_sendToSocket(const SOCKET& sock,const string& message);
    };

}