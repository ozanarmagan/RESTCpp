#pragma once

#ifdef _WIN32
    #include <WinSock2.h>
#else
    #include <sys/socket.h>
#endif

#include <functional>
#include <thread>
#include <ctime>
#include <fstream>
#include <string>
#include <tuple>

#include "Common.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

using std::function;
using std::unique_ptr;
using std::thread;
using std::time;
using std::ctime;
using std::ifstream;
using std::stringstream;

class HTTPServer
{
    public:
        HTTPServer(uint16_t port = 8080) : mPort(port) { init(); mBuffer = new char[8192]; };
        ~HTTPServer() { delete [] mBuffer; };
        void fRun();
        void fStop();
        void fSetLogging(bool value) { mLog = value; };
        class Router
        {
            public:
                void fAddRoute(const string& URLPath, const METHOD& method, const function<void(HTTPRequest&,HTTPResponse&)>& callBack) { mRoutes.push_back(std::make_tuple(URLPath, method, callBack)); };
                void fAddStaticRoute(string URLPath, string folderPath) { mStaticRoutes[URLPath] = folderPath; };
                friend class HTTPServer;
            private:
                unordered_map<string,string> mStaticRoutes;
                vector<std::tuple<string,METHOD,function<void(HTTPRequest&,HTTPResponse&)>>> mRoutes;
        };
        void fConnectRouter(Router* router) { mRouter = router; };
    private:
        uint64_t mSock,mAcceptSocket;
        struct sockaddr_in mServerAddr,mClientAddr;
        bool mLog; 
        uint16_t mPort;
        char* mBuffer;
        string mRawRequest;
        HTTPRequest* mCurrentRequest;
        Router* mRouter;
        vector<unique_ptr<thread>> mClientThreads;
        void init();
        void fOnRequest(uint64_t socket);
        const string fRecieveNext(uint64_t socket);
        const HTTPResponse fProcessRequest(const string& rawData);
        void fSendResponse(const HTTPResponse& response,const uint64_t socket);
};
