#pragma once

#include <iostream>
#include <memory>


#include "Common.h"

namespace restcpp
{
    using std::string;
    /**
     * @brief Class to store form data as seperate objects and add to requests or responses.Stored data can be both text or binary
     * 
     */
    class FormData
    {
        public:
            FormData(string name, string fileName, string textData, string contentType = "") : m_fileName(fileName),m_name(name),m_textData(textData),m_contentType(contentType) { m_isBinary = false; };
            FormData(string name, string fileName, std::shared_ptr<byte> binaryData, string contentType = "") : m_fileName(fileName),m_name(name),m_binaryData(binaryData),m_contentType(contentType) { m_isBinary = true; };
            const string getFileName() const { return m_fileName; };
            const string getName() const { return m_name; };
            const string getTextData() const { return m_textData; };
            const string getContentType() const { return m_contentType; };
            const byte* getBinaryData() const { return m_binaryData.get(); };
            const size_t getBinaryDataLength() const { return m_binaryDataLength; };
            const bool isBinary() const { return m_isBinary; };
            string serialize(bool isMultiPart = true);
        private:
            string m_fileName;
            string m_name;
            string m_textData;
            string m_contentType;
            bool m_isBinary;
            std::shared_ptr<byte> m_binaryData;
            size_t m_binaryDataLength;
    };
}