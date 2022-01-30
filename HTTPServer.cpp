#include "HTTPServer.h"

namespace restcpp
{
    namespace
    {   
        void hSetMainHeaders(std::shared_ptr<HTTPResponse> res)
        {
            res->fSetStatus(200);
            res->fAddHeader("Server","RESTC++");
            time_t currTime;
            time(&currTime);
            string timeStr = ctime(&currTime);
            std::replace(timeStr.begin(),timeStr.end(),'\n','\0');
            timeStr = timeStr.substr(0, timeStr.find('\0'));
            res->fAddHeader("Date",timeStr);
            res->fAddHeader("Connection","Close");
        }

        void hSetOptions(std::shared_ptr<HTTPRequest> req, std::shared_ptr<HTTPResponse> res,HTTPServer::Router* router)
        {
            res->fSetStatus(204);
            if(router == nullptr)
            {
                res->fAddHeader("Allow","HEAD");
                return;
            }
            string allowedMethods = "HEAD";
            auto pos1 = req->fGetPath().find_first_of("/");
            auto pos2 = req->fGetPath().find_last_of("/");
            string URLPath = req->fGetPath().substr(pos1, pos2 - pos1 + 1);
            if(router->fGetStaticRoutes().find(URLPath) != router->fGetStaticRoutes().end())
            {
                res->fAddHeader("Allow","HEAD, GET");
                return;
            }
            for(auto& obj : router->fGetDefinedRoutes())
                if(std::get<0>(obj) == URLPath)
                    switch (std::get<1>(obj))
                    {
                        case METHOD::GET: allowedMethods += ", GET";break;
                        case METHOD::POST: allowedMethods += ", POST";break;
                        case METHOD::PUT: allowedMethods += ", PUT";break;
                        case METHOD::PATCH: allowedMethods += ", PATCH";break;
                        case METHOD::DEL: allowedMethods += ", DELETE";break;
                        default:break;
                    }
            res->fAddHeader("Allow",allowedMethods);
        }

        bool hSearchStaticRoutes(std::shared_ptr<HTTPResponse> res, string path,string fileName,HTTPServer::Router* router)
        {
            bool hasFoundPath = false;
            bool hasFoundFile = false;
            for(auto& [key,value] : router->fGetStaticRoutes())
            {
                if(path == key)
                {
                    ifstream file(value + fileName);
                    bool checkFile = file.is_open();
                    file.close();
                    if(checkFile && fileName != "")
                    {
                        res->fSetBodyFile(value + fileName);
                        hasFoundFile = true;
                        hasFoundPath = true;
                        break;
                    }
                    hasFoundPath = true;
                }
            }
            if(hasFoundPath && !hasFoundFile)
            {
                res->fSetStatus(404);
                res->fSetBodyHTML("<html><pre>404 File Could Not Found</pre><br/><h6>RESTC++</html>");
            }
            return hasFoundPath;
        }

        bool hSearchDefinedRoutes(const std::shared_ptr<HTTPRequest>& req, const std::shared_ptr<HTTPResponse>& res, const string& path,const HTTPServer::Router* router)
        {
            for(const auto& route : router->fGetDefinedRoutes())
            {
                if(std::get<0>(route) == path && std::get<1>(route) == req->fGetMethod())
                {
                    std::get<2>(route)(*(req.get()), *(res.get()));
                    return true;
                }
            }
            return false;
        }

        void hProcessRouter(const std::shared_ptr<HTTPRequest>& req, const std::shared_ptr<HTTPResponse>& res, HTTPServer::Router* router)
        {
            auto fullPath = req->fGetPath();
            auto pos1 = fullPath.find_first_of("/");
            auto pos2 = fullPath.find_last_of("/");
            string URLPath = fullPath.substr(pos1, pos2 - pos1 + 1);
            string fileName = fullPath.substr(pos2 + 1);

            if(hSearchStaticRoutes(res, URLPath, fileName,router))
                return;

            if(!hSearchDefinedRoutes(req, res, URLPath + fileName, router))
            {
                res->fSetStatus(404);
                res->fSetBodyHTML("<html><pre>Invalid "+ gMethodToStr(req->fGetMethod())  + " request to " + URLPath + "</pre><br/><h6>RESTC++</html>");
            }
        }

        void hCloseSocket(const uint64_t& sock)
        {
    #ifdef _WIN32
            if(shutdown(sock, SD_BOTH) == SOCKET_ERROR)
            {
                std::cout << "Error while closing socket" << std::endl;
                std::cout << WSAGetLastError() << std::endl;
            };
            if(closesocket(sock) == SOCKET_ERROR)
            {
                std::cout << "Error while closing socket" << std::endl;
                std::cout << WSAGetLastError() << std::endl;
            };
    #else
            if(shutdown(sock, SHUT_RDWR) < 0)
            {
                std::cout << "Error while closing socket" << std::endl;
            };
            if(close(sock) < 0)
            {
                std::cout << "Error while closing socket" << std::endl;
            };
    #endif
        }

        void hSendToSocket(const uint64_t& sock,const string& message)
        {
            size_t sent = 0,totalSent = 0;
            const char* buffer = message.c_str();

            while(totalSent < message.length())
            {
                if((sent = send(sock, buffer + totalSent, message.length() - totalSent,0)) < 0)
                {
                    std::cout << "Error while sending response" << std::endl;
                }

                totalSent += sent;
            }
        }
    }




    void HTTPServer::init()
    {

    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(0x202,&wsaData);

        if((mSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET)
    #else
        if((mSock = socket(AF_INET,SOCK_STREAM,0)) < 0)
    #endif
        {
            std::cout << "Error while initilazing socket\n";
        }

    #ifdef _WIN32
        unsigned long o = 1;
        ioctlsocket(mSock, FIONBIO, &o);
    #else
        fcntl(mAcceptSocket, F_SETFL,O_NONBLOCK);
    #endif
        int op = 1;
        setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (char*)&op, sizeof(op));

        mServerAddr.sin_family = AF_INET;
        mServerAddr.sin_port = htons(mPort);
    #ifdef _WIN32
        mServerAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    #else
        mServerAddr.sin_addr.s_addr = INADDR_ANY;
    #endif


        if(bind(mSock, (struct sockaddr*) &mServerAddr, sizeof(mServerAddr)) < 0)
        {
            std::cout << "Error while binding socket\n";
        }

        if(listen(mSock, SOMAXCONN))
        {
            std::cout << "Error while listening socket\n";
        }

    }


    const string HTTPServer::fRecieveNext(uint64_t socket)
    {
        string rawData;
        char buffer[8192];
        uint8_t attempts = 0;
        try
        {
            int64_t recieveLength = 0,totalRecieved = 0; 
            int64_t recieveLengthBeforeBody = -1;
            int contentLength = 0;
            while(1)
            {
    #ifdef _WIN32
                if((recieveLength = recv(socket,buffer,8192, 0)) == SOCKET_ERROR)
    #else
                if((recieveLength = recv(socket,buffer,8192, 0)) < 0)
    #endif
                {
                    //std::cout << "Error while receiving data from socket\n";
        #ifdef _WIN32
                    //std::cout << WSAGetLastError() << std::endl;
        #endif
                    return "HTTPFAIL";
                }



                if(recieveLength == 0)
                    break;
                
                totalRecieved += recieveLength; 
                rawData += string(buffer,recieveLength);

                if(totalRecieved > 32)
                {
                    if(rawData.find("Content-Length") == string::npos && contentLength == 0)
                        break;
                    else
                    {
                        if(contentLength == 0)
                        {
                            auto pos = rawData.find("Content-Length:");
                            auto lengthStr = rawData.substr(pos + 16, rawData.find_first_of("\r",pos) - pos - 16);
                            contentLength = std::stoi(lengthStr);
                        }
                        if(rawData.find("\r\n\r\n") != string::npos && recieveLengthBeforeBody == -1)
                        {
                            recieveLengthBeforeBody = rawData.find("\r\n\r\n") + 4;
                        }

                        if(totalRecieved >= contentLength + recieveLengthBeforeBody)
                            break;
                    }
                }
            }
        }
        catch (runtime_error ex)
        {
            std::cout << ex.what() << "\n Exception on recieve";
        }
        
        return rawData;
    }


    void HTTPServer::fRun()
    {
        ThreadPool tPool(8);
        while(1)
        {
            socklen_t size = sizeof(mClientAddr);
    #ifdef _WIN32
            if((mAcceptSocket = accept(mSock,0,0)) == INVALID_SOCKET)
    #else
            if((mAcceptSocket = accept(mSock,0,0)) < 0)
    #endif
            {
                //std::cout << "Error while initilazing accept socket\n";
                continue;
            }
        
    #ifdef _WIN32
            int timeout = 2000;
            setsockopt(mAcceptSocket, SOL_SOCKET, SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
    #else
            struct timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            setsockopt(mAcceptSocket, SOL_SOCKET, SO_RCVTIMEO,&timeout,sizeof(timeout));
    #endif

            //fOnRequest(mAcceptSocket);
            auto res = tPool.fEnqueue(std::bind(&HTTPServer::fOnRequest,this,mAcceptSocket));
        }

        hCloseSocket(mSock);
    }


    void HTTPServer::fOnRequest(uint64_t socket)
    {
        const string reqData = fRecieveNext(mAcceptSocket);
        if(reqData == "")
            return;
        auto res = fProcessRequest(reqData);
        fSendResponse(res,socket);
    }

    std::shared_ptr<HTTPResponse> HTTPServer::fProcessRequest(const string& rawData)
    {   

        auto res = std::make_shared<HTTPResponse>();
        try
        {
            hSetMainHeaders(res);
            if(rawData == "HTTPFAIL")
            {
                res->fSetStatus(408);
                return res;
            }
            auto req = std::make_shared<HTTPRequest>(rawData);
            if(req->fGetMethod() == METHOD::HEAD)
                res->fSetHeaderOnly(true);
            res->fSetHTTPVersion(req->fGetHTTPVersion());
            if(req->fGetMethod() == METHOD::OPTIONS)
            {
                hSetOptions(req,res,mRouter);
                return res;
            }
            
            if(mRouter != nullptr)
                hProcessRouter(req,res,mRouter);
        }
        catch (runtime_error ex)
        {
            std::cout << ex.what() << "\n Exception on proccess request";
        }
        return res;
    }


    void HTTPServer::fSendResponse(std::shared_ptr<HTTPResponse>& response,const uint64_t& sock)
    {
        try
        {
            hSendToSocket(sock,response->fSerializeResponse());

            hCloseSocket(sock);
        }
        catch (runtime_error ex)
        {
            std::cout << ex.what() << "\n Exception on send response";
        }
    }


}