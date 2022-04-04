#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/ip.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <cerrno>
    #include <poll.h>
#endif

#include "Common.h"
#include "HTTPResponse.h"
#include "HTTPRequest.h"

namespace restcpp
{
    using std::string;

    class Proxy
    {
        public:
            Proxy(string address);
            const HTTPResponse& getResponse() const { return m_response; };

        private:
            char* m_buffer;
            HTTPResponse m_response;
            struct sockaddr_in m_client;
            struct hostent* m_host;
            SOCKET m_socket;
            void m_initConnection();
            void m_sendRequest();
            void m_getResponse();
            void m_parseAddress(string address);
            string m_addressHead,m_addressTail;
            short m_port;
    };
}