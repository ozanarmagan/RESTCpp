#pragma once

#include <iostream>
#include "Common.h"

using std::string;

class FormData
{
    public:
        FormData(string name, string fileName, string textData, string contentType = "") : mFileName(fileName),mName(name),mTextData(textData),mContentType(contentType) { mIsBinary = false; };
        FormData(string name, string fileName, byte* binaryData, string contentType = "") : mFileName(fileName),mName(name),mBinaryData(binaryData),mContentType(contentType) { mIsBinary = true; };
        ~FormData() { delete [] mBinaryData; };
        const string fGetFileName() const { return mFileName; };
        const string fGetName() const { return mName; };
        const string fGetTextData() const { return mTextData; };
        const byte* fGetBinaryData() const { return mBinaryData; };
        const size_t fGetBinaryDataLength() const { return mBinaryDataLength; };
        const bool fIsBinary() const { return mIsBinary; };
        string serialize(bool isMultiPart = true);
    private:
        string mFileName;
        string mName;
        string mTextData;
        string mContentType;
        bool mIsBinary;
        byte* mBinaryData;
        size_t mBinaryDataLength;
};