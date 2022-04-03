#include "../include/Proxy.h"

namespace restcpp
{
    Proxy::Proxy(string address)
    {
        m_parseAddress(address);
		m_initConnection();
		m_sendRequest();
		m_getResponse();
    }

	void Proxy::m_sendRequest()
	{
        string req = "GET /" + m_addressTail + " HTTP/1.1\r\nHost: " + m_addressHead  +  "\r\nAccept: */*\r\nUser-Agent: RESTC++ Client v1.0\r\n\r\n";
        send(m_socket,req.c_str(),req.length(),0);
	}

	void Proxy::m_getResponse()
	{
		char cur;
		string res;
		while ( read(m_socket, &cur, 1) > 0 ) {
		res += cur;
		}
		std::cout << res;
		close(m_socket);
	}

	void Proxy::m_initConnection()
	{
        m_host = gethostbyname(m_addressHead.c_str());
        if(m_host == NULL)
            throw runtime_error("DNS Information could not retrieved for proxy address:" + m_addressHead);
        m_client.sin_family = AF_INET;
        m_client.sin_port = htons(m_port);
#ifdef _WIN32
        memcpy(&m_client.sin_addr.S_un,m_host->h_addr,m_host->h_length);
#else
        memcpy(&m_client.sin_addr,m_host->h_addr,m_host->h_length);
#endif
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(0x202,&wsaData);

        if((m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET)
#else
        if((m_socket = socket(AF_INET,SOCK_STREAM,0)) < 0)
#endif
        {
            std::cout << "Error while initilazing socket for proxy request\n";
        }
        #ifdef _WIN32
        if(connect(m_socket,(struct sockaddr*)&m_client, sizeof(m_client)) == SOCKET_ERROR)
#else
        if(connect(m_socket,(struct sockaddr*)&m_client, sizeof(m_client)) < 0)
#endif
        {
            close(m_socket);
            std::cout << "Error while initilazing socket for proxy request\n";
        }
	}



    void Proxy::m_parseAddress(string address)
    {
        if(address.find("://") != string::npos)
        {
			address = address.substr(address.find("://") + 3);
        }
        if(address.find("/") == string::npos)
        {
			if(address.find(":") == string::npos)
			{
					m_port = 80;
					m_addressHead = address;
					m_addressTail = "";
					return;
			}
			else
			{
					m_port = std::stoi(address.substr(address.find(":") + 1));
					m_addressHead = address.substr(0,address.find(":"));
					m_addressTail = "";
					return;
			}
        }
        m_addressHead = address.substr(0,address.find("/"));
        if(m_addressHead.find(":") == string::npos)
			m_port = 80;
        else
        {
			m_port = std::stoi(m_addressHead.substr(m_addressHead.find(":") + 1));
			m_addressHead = m_addressHead.substr(0,m_addressHead.find(":"));
        }
        m_addressTail = address.substr(address.find("/") + 1);
        return;
    }
}
