#include "../include/Server.h"

namespace restcpp
{

    /**
     * @brief Set common headers for given response
     * 
     * @param res to set headers of it
     */
    void Server::h_setMainHeaders(std::shared_ptr<HTTPResponse> res)
    {
        res->setStatus(200);
        res->addHeader("Server","RESTC++ Server v1.0");
        time_t currTime;
        time(&currTime);
        std::string timeStr = ctime(&currTime);
        std::replace(timeStr.begin(),timeStr.end(),'\n','\0');
        timeStr = timeStr.substr(0, timeStr.find('\0'));
        res->addHeader("Date",timeStr);
        res->addHeader("Connection","Keep-Alive");
        res->addHeader("Keep-Alive","timeout=5, max=100");
    }

    /**
     * @brief Set options header for request
     * 
     * @param req 
     * @param res 
     * @param router 
     */
    void Server::h_setOptions(std::shared_ptr<HTTPRequest> req, std::shared_ptr<HTTPResponse> res,Router& router)
    {
        res->setStatus(204);
        std::string allowedMethods = "HEAD";
        auto pos1 = req->getPath().find_first_of("/");
        auto pos2 = req->getPath().find_last_of("/");
        std::string URLPath = req->getPath().substr(pos1, pos2 - pos1 + 1);
        if(router.getStaticRoutes().find(URLPath) != router.getStaticRoutes().end())
        {
            res->addHeader("Allow","HEAD, GET");
            return;
        }
        for(auto& obj : router.getDefinedRoutes())
            if(obj.m_URLPath == URLPath)
                switch (obj.m_method)
                {
                    case METHOD::GET: allowedMethods += ", GET";break;
                    case METHOD::POST: allowedMethods += ", POST";break;
                    case METHOD::PUT: allowedMethods += ", PUT";break;
                    case METHOD::PATCH: allowedMethods += ", PATCH";break;
                    case METHOD::DEL: allowedMethods += ", DELETE";break;
                    default:break;
                }
        res->addHeader("Allow",allowedMethods);
    }

    /**
     * @brief Search for a static route for the path given
     * 
     * @param res 
     * @param path 
     * @param fileName 
     * @param router 
     * @return true if a static path found in router for given path
     */
    bool Server::h_searchStaticRoutes(std::shared_ptr<HTTPResponse> res, std::string path,std::string fileName,Router& router)
    {
        bool hasFoundPath = false;
        bool hasFoundFile = false;
        for(auto& [key,value] : router.getStaticRoutes())
        {
            if(path == key)
            {
                if(fileName == "")
                    fileName = "index.html";
                std::ifstream file(value + fileName);
                bool checkFile = file.is_open();
                file.close();
                if(checkFile && fileName != "")
                {
                    res->setBodyFile(value + fileName);
                    hasFoundFile = true;
                    hasFoundPath = true;
                    break;
                }
                hasFoundPath = true;
            }
        }
        if(hasFoundPath && !hasFoundFile)
        {
            res->setStatus(404);
            res->setBodyHTML("<html><pre>404 File Could Not Found</pre><br/><h6>RESTC++</h6></html>");
            res->addHeader("Connection","close");
        }
        return hasFoundPath;
    }
    

    /**
     * @brief Function to search dynamic routes
     * 
     * @param req 
     * @param res 
     * @param path 
     * @param router 
     * @return true if a dynamic route found for that path
     */
    bool Server::h_searchDefinedRoutes(const std::shared_ptr<HTTPRequest>& req, const std::shared_ptr<HTTPResponse>& res, const std::string& path,const Router& router)
    {
        for(const auto& route : router.getDefinedRoutes())
        {
            if(route.m_pathParams.size() == 0)
            {
                if(route.m_URLPath == path && route.m_method == req->getMethod())
                {
                    route.m_callBack(*(req.get()), *(res.get()));
                    return true;
                }
            }
            else if(route.m_method == req->getMethod())
            {
                
                if(path.find(route.m_pathParams[0].m_path) == std::string::npos)
                    continue;
                auto pathC = path;
                
                for(auto param : route.m_pathParams)
                {
                    auto startPos = pathC.find(param.m_path);
                    if(startPos == std::string::npos)
                        break;
                    startPos +=  param.m_path.length();
                    size_t endPos = pathC.length();
                    if(param.m_endChar > 0)
                        endPos = pathC.find_first_of(param.m_endChar, startPos);
                    auto val = pathC.substr(startPos, endPos - startPos);
                    req->m_pathParams[param.m_name] = g_decodeUri(val.c_str());
                    pathC.erase(startPos, endPos - startPos);
                    pathC.insert(startPos,"{}");
                }
                if(route.m_pathParams.size() == req->m_pathParams.size())
                {
                    route.m_callBack(*(req.get()), *(res.get()));
                    return true;
                }
                else
                    req->m_pathParams.clear();
            }
        }
        return false;
    }

    /**
     * @brief Search for both static and dynamic routes in router 
     * 
     * @param req 
     * @param res 
     * @param router 
     */
    void Server::h_processRouter(const std::shared_ptr<HTTPRequest>& req, const std::shared_ptr<HTTPResponse>& res, Router& router)
    {
        auto fullPath = req->getPath();
        auto pos1 = fullPath.find_first_of("/");
        auto pos2 = fullPath.find_last_of("/");
        std::string URLPath = fullPath.substr(pos1, pos2 - pos1 + 1);
        std::string fileName = fullPath.substr(pos2 + 1);

        if(h_searchStaticRoutes(res, URLPath, fileName,router))
            return;

        if(!h_searchDefinedRoutes(req, res, URLPath + fileName, router))
        {
            res->setStatus(404);
            res->addHeader("Connection","close");
            res->setBodyHTML("<html><pre>Invalid "+ gMethodToStr(req->getMethod())  + " request to " + URLPath + "</pre><br/><h6>RESTC++</html>");
        }

    }

    /**
     * @brief Helper function to close a socket
     * 
     * @param sock 
     */
    void Server::h_closeSocket(const SOCKET& sock)
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

    void Server::h_sendToSocket(const SOCKET& sock,const std::string& message)
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




    /**
     * @brief Init sockets for server
     * 
     */
    void Server::init()
    {
        std::cout << m_cert << " " << m_pem << std::endl;

        if(m_cert != "" && m_pem != "")
        {
            h_createSSLContext();
            if(m_ctx)
                h_configureSSLContext();
        }

#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(0x202,&wsaData);

        if((m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET)
#else
        if((m_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
#endif
        {
            std::cout << "Error while initilazing socket\n";
        }
#ifndef _WIN32
        fcntl(m_acceptSocket, F_SETFL,O_NONBLOCK);
#endif
        int op = 1;
        setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&op, sizeof(op));

        m_serverAddr.sin_family = AF_INET;
        m_serverAddr.sin_port = htons(m_port);
#ifdef _WIN32
        m_serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
        m_serverAddr.sin_addr.s_addr = INADDR_ANY;
#endif


        if(bind(m_sock, (struct sockaddr*) &m_serverAddr, sizeof(m_serverAddr)) < 0)
        {
            std::cout << "Error while binding socket\n";
        }

        if(listen(m_sock, SOMAXCONN))
        {
            std::cout << "Error while listening socket\n";
        }

    }




    /**
     * @brief Run server with a thread pool 
     * 
     */
    void Server::run()
    {
        ThreadPool tPool(THREAD_COUNT);
        while(1)
        {
#ifdef _WIN32
            if((m_acceptSocket = accept(m_sock,0,0)) == INVALID_SOCKET)
#else
            if((m_acceptSocket = accept(m_sock,0,0)) < 0)
#endif
            {
                std::cout << "Error while initilazing accept socket\n";
                continue;
            }
        

            //fOnRequest(m_acceptSocket);
            auto res = tPool.enqueue(std::bind(&Server::onRequest,this,m_acceptSocket));
        }

        h_closeSocket(m_sock);
    }

    /**
     * @brief Wrapper function to be called on every client connection
     * 
     * @param socket 
     */
    void Server::onRequest(SOCKET socket)
    {
        while(1)
        {
            SSL* ssl = nullptr;
            if(m_ctx)
            {
                ssl = SSL_new(m_ctx);
                SSL_set_fd(ssl,socket);
                if(SSL_accept(ssl) <= 0)
                {
                    std::cout << "Error while accepting SSL connection\n";
                    // log error
                    std::cout << ERR_error_string(ERR_get_error(),NULL) << std::endl;
                }
            }
            const std::string reqData = recieveNext(socket, ssl);
            if(reqData == "" || reqData == "HTTPFAIL")
            {
                h_closeSocket(socket);
                if (ssl)
                {
                    SSL_shutdown(ssl);
                    SSL_free(ssl);
                }
                return;
            }
            auto res = processRequest(reqData);
            sendResponse(res,socket,ssl);
        }
    }

    /**
     * @brief Recieve request from socket
     * 
     * @param socket to listen
     * @return const std::string raw request as std::string
     */
    const std::string Server::recieveNext(SOCKET socket, SSL*& ssl)
    {
        std::string rawData;
        char buffer[32 * 1024];
        uint8_t attempts = 0;
        try
        {
            int64_t recieveLength = 0,totalRecieved = 0; 
            int64_t recieveLengthBeforeBody = -1;
            int contentLength = 0;
            bool isChunked = false;
            while(1)
            {
                if(ssl)
                {
                    if((recieveLength = SSL_read(ssl,buffer,32 * 1024)) <= 0)
                    {
                        std::cout << "Error while receiving data from socket\n";
                        return "HTTPFAIL";
                    }
                }
                else
                {
#ifdef _WIN32
                    if((recieveLength = recv(socket,buffer,32 * 1024, 0)) == SOCKET_ERROR)
#else
                    if((recieveLength = recv(socket,buffer,32 * 1024, 0)) < 0)
#endif
                    {
                        //std::cout << "Error while receiving data from socket\n";
#ifdef _WIN32
                        std::cout << WSAGetLastError() << std::endl;
#endif
                        return "HTTPFAIL";
                    }

                }

                if(recieveLength == 0)
                    break;


                totalRecieved += recieveLength;

                rawData += std::string(buffer,recieveLength);
                // check if we have recieved all headers
                if(recieveLengthBeforeBody == -1 && rawData.find("\r\n\r\n") != std::string::npos)
                {
                    
                    recieveLengthBeforeBody = rawData.find("\r\n\r\n") + 2;
                    
                    // check if we have content length
                    if(rawData.find("Content-Length: ") != std::string::npos)
                        contentLength = std::stoi(rawData.substr(rawData.find("Content-Length: ") + 16,rawData.find("\r\n",rawData.find("Content-Length: ")) - rawData.find("Content-Length: ") - 16));
                    else if(rawData.find("content-length: ") != std::string::npos)
                        contentLength = std::stoi(rawData.substr(rawData.find("content-length: ") + 16,rawData.find("\r\n",rawData.find("content-length: ")) - rawData.find("content-length: ") - 16));
                    // check if we have chunked encoding
                    else if(rawData.find("Transfer-Encoding: chunked") != std::string::npos || rawData.find("transfer-encoding: chunked") != std::string::npos)
                        isChunked = true;
                    else
                        break;
                    
                }

                
                if(!isChunked && recieveLengthBeforeBody != -1 && totalRecieved >= recieveLengthBeforeBody + contentLength)
                    break;
                else if(isChunked && rawData.find("\r\n0\r\n\r\n") != std::string::npos)
                    break;


            }
        }
        catch (std::runtime_error ex)
        {
            std::cout << ex.what() << "\n Exception on recieve";
        }


        return rawData;
    }


    /**
     * @brief Process raw request data recieved from the socket and parse it into HTTPResponse object
     * 
     * @param rawData 
     * @return std::shared_ptr<HTTPResponse> pointer to parsed HTTPResponse object
     */
    std::shared_ptr<HTTPResponse> Server::processRequest(const std::string& rawData)
    {   

        

        auto res = std::make_shared<HTTPResponse>();
        try
        {
            h_setMainHeaders(res);
            if(rawData == "HTTPFAIL")
            {
                std::cout << "HTTPFAIL\n";
                res->setStatus(408);
                res->addHeader("Connection","close");
                return res;
            }
            auto req = std::make_shared<HTTPRequest>();
            bool isValid = req->parseRequest(rawData);
            if(!isValid)
            {
                std::cout << "Invalid request\n";
                res->setStatus(400);
                res->addHeader("Connection","close");
                res->setBodyHTML(getErrorHTML(400));
                return res;
            }
            if(req->getHeader("Host") == "")
            {
                std::cout << "Host header not found\n";
                res->setStatus(400);
                res->addHeader("Connection","close");
                res->setBodyHTML(getErrorHTML(400));
                return res;
            }
            if(req->getMethod() == METHOD::HEAD || req->getMethod() == METHOD::CONNECT)
                res->setHeaderOnly(true);
            res->setHTTPVersion(req->getHTTPVersion());
            if(req->getMethod() == METHOD::OPTIONS)
            {
                h_setOptions(req,res,m_router);
                return res;
            }
            
            h_processRouter(req,res,m_router);

            lastRequest = req;
        }
        catch (std::runtime_error ex)
        {
            std::cout << ex.what() << "\n Exception on proccess request";
        }

        return res;
    }

    /**
     * @brief Send response through the socket
     * 
     * @param response 
     * @param sock 
     */
    void Server::sendResponse(std::shared_ptr<HTTPResponse>& response,const SOCKET& sock,SSL* ssl)
    {
        try
        {
            if(ssl != nullptr)
            {
                SSL_write(ssl,response->serializeResponse().c_str(),response->serializeResponse().length());
            }
            else
            {
                h_sendToSocket(sock,response->serializeResponse());
            }
        }
        catch (std::runtime_error ex)
        {
            std::cout << ex.what() << "\n Exception on send response";
        }
    }

    void Server::h_createSSLContext()
    {
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        m_ctx = SSL_CTX_new(TLS_server_method());
        if(m_ctx == NULL)
        {
            ERR_print_errors_fp(stdout);
            abort();
        }

        std::cout << "SSL context created\n";
    }

    void Server::h_configureSSLContext()
    {
        SSL_CTX_set_ecdh_auto(m_ctx, 1);
        if(SSL_CTX_use_certificate_file(m_ctx, m_cert.c_str(), SSL_FILETYPE_PEM) <= 0)
        {
            ERR_print_errors_fp(stdout);

            abort();
        }
        if(SSL_CTX_use_PrivateKey_file(m_ctx, m_pem.c_str(), SSL_FILETYPE_PEM) <= 0 )
        {
            ERR_print_errors_fp(stdout);
            abort();
        }

        std::cout << "SSL context configured\n";
    }


}