#pragma once

#include <iostream>
#include <memory>


#include "Common.h"

namespace restcpp
{
    /**
     * @brief Class to store form data as seperate objects and add to requests or responses.Stored data can be both text or binary
     * 
     */
    class FormData
    {
        public:
            FormData(std::string name, std::string fileName, std::string textData, std::string contentType = "") : m_fileName(fileName),m_name(name),m_textData(textData),m_contentType(contentType) { m_isBinary = false; };
            FormData(std::string name, std::string fileName, byte* binaryData,size_t binaryDataLength, std::string contentType = "") : m_fileName(fileName),m_name(name),m_binaryData(binaryData),m_contentType(contentType),m_binaryDataLength(binaryDataLength) { m_isBinary = true; };
            //~FormData() {delete [] m_binaryData;};
            const std::string getFileName() const { return m_fileName; };
            const std::string getName() const { return m_name; };
            const std::string getTextData() const { return m_textData; };
            const std::string getContentType() const { return m_contentType; };
            const byte* getBinaryData() const { return m_binaryData; };
            const size_t getBinaryDataLength() const { return m_binaryDataLength; };
            const bool isBinary() const { return m_isBinary; };
            std::string serialize(bool isMultiPart = true);
        private:
            std::string m_fileName;
            std::string m_name;
            std::string m_textData;
            std::string m_contentType;
            bool m_isBinary;
            byte* m_binaryData;
            size_t m_binaryDataLength;
    };
}