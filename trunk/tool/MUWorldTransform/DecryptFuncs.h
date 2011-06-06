#pragma once
#include <string>

void fileOffset(const std::string& strSrcFilename, const std::string& strDestFilename,int offset);
void decryptMuBuffer(unsigned char* buffer, size_t size);
void decryptBuffEffectFile(const std::string& strSrcFilename, const std::string& strDestFilename);
void decryptMuFile(const std::string& strSrcFilename, const std::string& strDestFilename);
void decryptMuBufferXOR3(unsigned char* buffer, size_t size);
void decryptMuFileXOR3(const std::string& strSrcFilename, const std::string& strDestFilename);
void decryptMuATTFile(const std::string& strSrcFilename, const std::string& strDestFilename);
bool isEncBmd(const std::string& strSrcFilename);