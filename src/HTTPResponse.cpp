#include "../include/HTTPResponse.h"

namespace restcpp
{
    namespace 
    {
        /**
         * @brief Generate boundary for formdata
         * 
         * @return std::string 
         */
        std::string gGenerateBoundary()
        {
            std::string res = "-----------------";
            for(int i = 0;i < 25;i++)
            {
                srand(time(nullptr));
                int r = rand() % 10;
                res += (char)(r + 48);
            }
            return res;
        }
    }

    /* Serialize cookies to string */
    const std::string HTTPResponse::serializeCookies() const
    {
        std::string res;
        for(auto& cookie : m_cookies)
        {
            res += "Set-Cookie: " + cookie.getKey() + "=" + cookie.getValue() + "; ";
            if(cookie.getPath() != "")
            {
                res += "Path=" + cookie.getPath() + "; ";
            }
            if(cookie.getDomain() != "")
            {
                res += "Domain=" + cookie.getDomain() + "; ";
            }
            if(cookie.getExpiresStr() != "")
            {
                res += "Expires=" + cookie.getExpiresStr() + "; ";
            }
            if(cookie.isSecure())
            {
                res += "Secure; ";
            }
            if(cookie.isHttpOnly())
            {
                res += "HttpOnly; ";
            }
            res += "\r\n";
        }
        return res;
    }

    /**
     * @brief Serialize response into std::string 
     * 
     * @return const std::string 
     */
    const std::string HTTPResponse::serializeResponse() const 
    {
        std::string res = "";
        try
        {
            std::string statusDesc = gGetStatusDescription(m_statusCode);
            res += std::string("HTTP/") + std::to_string(m_requestVersion.majorVersion) + std::string(".") + std::to_string(m_requestVersion.minorVersion) + " ";
            res += std::to_string(m_statusCode) + " " + statusDesc + "\r\n";
            for(auto& [key,value] : m_headers)
                res += key + ": " + value + "\r\n";
            res += serializeCookies();
            if(m_requestBody.length() > 0)
                res += "Content-Length: " + std::to_string(m_requestBody.length());
            if(m_requestBody.length() > 0 && !m_headerOnly)
                res += std::string("\r\n\r\n") +  m_requestBody;
            else if(m_headerOnly || m_requestBody.length() == 0)
                res += "\r\n";
        }
        catch (std::runtime_error ex)
        {
            std::cout << ex.what() << "\nError while serializing response";
        }
        return res;
    }

    /**
     * @brief Put formdata objects into the response
     * 
     * @param form 
     */
    void HTTPResponse::setBodyFormData(const std::vector<FormData*> form)
    {
        std::string boundary = "BOUNDARY__" + gGenerateBoundary();
        m_headers["Content-Type"] = " multipart/form-data; boundary=" + boundary;
        for(auto& data : form)
        {
            m_requestBody += "--" + boundary + "\r\n";
            m_requestBody += "Content-Disposition: form-data; ";
            m_requestBody += "name=\"" + data->getName() + "\"";
            if(data->getFileName() != "")
                m_requestBody += "name=\"" + data->getFileName() + "\"";
            if(data->getContentType() != "")
                m_requestBody += std::string("\r\n") + "Content-Type: " + data->getContentType();
            m_requestBody += "\r\n\r\n";
            if(data->isBinary())
            {
                const byte* dataHead = data->getBinaryData();
                auto length = data->getBinaryDataLength();
                for(int i = 0;i < length;i++)
                    m_requestBody += dataHead[i];
            }
            else
                m_requestBody += data->getTextData();
            
            m_requestBody += "\r\n";
        }

        m_requestBody += "--" + boundary + "--";
    }

    /**
     * @brief Set a file to body
     * 
     * @param fileName 
     */
    void HTTPResponse::setBodyFile(const std::string& fileName)
    {
        try
        {
            auto MIME = gMIMETable.at(fileName.substr(fileName.find_last_of('.') + 1));
            m_headers["Content-Type"] = " " + MIME;
            std::ifstream file(fileName, std::ios::binary);

            if(file.good())
            {
                std::stringstream stream;
                stream << file.rdbuf();
                m_requestBody = stream.str();
            }
            
            file.close();
        }
        catch (std::runtime_error ex)
        {
            std::cout << ex.what() << "\nError while set file to body\n" << fileName; 
        }
    }
}