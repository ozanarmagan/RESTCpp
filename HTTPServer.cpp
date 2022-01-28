#include "HTTPServer.h"


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

    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_port = htons(mPort);
#ifdef _WIN32
    mServerAddr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    mServerAddr.sin_addr.S_addr = INADDR_ANY;
#endif

    if(bind(mSock, (struct sockaddr*) &mServerAddr, sizeof(mServerAddr)) < 0)
    {
        std::cout << "Error while binding socket\n";
    }

    if(listen(mSock, 5))
    {
        std::cout << "Error while listening socket\n";
    }

}


void HTTPServer::fRecieveNext()
{
    int64_t recieveLength = 0,totalRecieved = 0; 
    int64_t recieveLengthBeforeBody = -1;
    int contentLength = 0;
    mRawRequest.clear();
    while(1)
    {
       

        memset(mBuffer,'\0',8192);


        if((recieveLength = recv(mAcceptSocket,mBuffer,8192, 0)) == SOCKET_ERROR)
        {
            std::cout << "Error while receiving data from socket\n";
            std::cout << WSAGetLastError();
        }


        totalRecieved += recieveLength; 
        mRawRequest += string(mBuffer,recieveLength);

        if(totalRecieved > 32)
        {
            string temp = string(mBuffer);
            if(temp.find("Content-Length") == string::npos && contentLength == 0)
                break;
            else
            {
                if(contentLength == 0)
                {
                    auto pos = temp.find("Content-Length:");
                    auto lengthStr = temp.substr(pos + 16, temp.find_first_of("\r",pos) - pos - 16);
                    contentLength = std::stoi(lengthStr);
                }
                if(temp.find("\r\n\r\n") != string::npos && recieveLengthBeforeBody == -1)
                {
                    recieveLengthBeforeBody = temp.find("\r\n\r\n") + 4;
                }

                if(totalRecieved >= contentLength + recieveLengthBeforeBody)
                    break;
            }
        }

    }


}


void HTTPServer::fRun()
{
    while(1)
    {
        int size = sizeof(mClientAddr);
#ifdef _WIN32
        if((mAcceptSocket = accept(mSock,(struct sockaddr*) &mClientAddr,&size)) == INVALID_SOCKET)
#else
        if((mAcceptSocket = accept(mSock,(struct sockaddr*) &mClientAddr,&size))) < 0)
#endif
    {
        std::cout << "Error while initilazing accept socket\n";
    }

        
        fRecieveNext();
        fProcessRequest();
        fSendResponse();

        closesocket(mAcceptSocket);
    }
}


void HTTPServer::fProcessRequest()
{   
    mCurrentRequest = new HTTPRequest(mRawRequest);
}

void HTTPServer::fSendResponse()
{   }

