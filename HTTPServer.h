#pragma once

#ifdef _WIN32
    #include <WinSock2.h>
#else
    #include <sys/socket.h>
#endif

#include <functional>

#include "Common.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

using std::function;

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
            private:
                vector<std::tuple<string,METHOD,function<void(HTTPRequest&,HTTPResponse&)>>> routes;
        };
    private:
        uint64_t mSock,mAcceptSocket;
        struct sockaddr_in mServerAddr,mClientAddr;
        bool mLog;
        unordered_map<string,string> staticRoutes; 
        uint16_t mPort;
        char* mBuffer;
        string mRawRequest;
        HTTPRequest* mCurrentRequest;
        void init();
        void fRecieveNext();
        void fProcessRequest();
        void fSendResponse();
};
