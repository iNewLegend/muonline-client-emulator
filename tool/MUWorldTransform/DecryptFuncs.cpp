#include "DecryptFuncs.h"
#include "FileSystem.h"

void fileOffset(const std::string& strSrcFilename, const std::string& strDestFilename,int offset)
{
	FILE* fp = fopen(strSrcFilename.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		if (offset<0)
		{
			size_t addSize=abs(offset);
			fseek(fp, 0, SEEK_SET);
			size+=addSize;
			char* buffer= new char[size];
			fread(buffer,addSize,1,fp);
			fseek(fp, 0, SEEK_SET);
			fread(&buffer[addSize],size-addSize,1,fp);
			{

				FILE* fpw = fopen(strDestFilename.c_str(), "wb");
				if (fpw)
				{
					fwrite(buffer,size,1,fpw);
					fclose(fpw);
				}
			}
			delete buffer;
		}
		else if (size>offset)
		{
			fseek(fp, offset, SEEK_SET);
			size-=offset;
			char* buffer= new char[size];
			fread(buffer,size,1,fp);
			{

				FILE* fpw = fopen(strDestFilename.c_str(), "wb");
				if (fpw)
				{
					fwrite(buffer,size,1,fpw);
					fclose(fpw);
				}
			}
			delete buffer;
		}
		fclose(fp);
	}
}

void decryptMuBuffer(unsigned char* buffer, size_t size)
{
	const unsigned char xorKeys[] = {
		0xd1, 0x73, 0x52, 0xf6,
		0xd2, 0x9a, 0xcb, 0x27,
		0x3e, 0xaf, 0x59, 0x31,
		0x37, 0xb3, 0xe7, 0xa2
	};
	char key = 0x5E;
	for (size_t i=0; i<size; ++i)
	{
		char encode = *buffer;
		*buffer ^= xorKeys[i%16];
		*buffer -= key;
		key = encode+0x3D;
		buffer++;
	}
}

void decryptBuffEffectFile(const std::string& strSrcFilename, const std::string& strDestFilename)
{
	FILE* fp = fopen(strSrcFilename.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		unsigned char* buffer = new unsigned char[size];
		fread(buffer,size,1,fp);
		fclose(fp);
		fp = NULL;
		decryptMuBuffer(buffer+4,size-4);
		fp = fopen(strDestFilename.c_str(), "wb");
		if (fp)
		{
			fwrite(buffer,size,1,fp);
			fclose(fp);
		}
		delete buffer;
	}
}

void decryptMuFile(const std::string& strSrcFilename, const std::string& strDestFilename)
{
	FILE* fp = fopen(strSrcFilename.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		unsigned char* buffer = new unsigned char[size];
		fread(buffer,size,1,fp);
		fclose(fp);
		fp = NULL;
		decryptMuBuffer(buffer,size);
		fp = fopen(strDestFilename.c_str(), "wb");
		if (fp)
		{
			fwrite(buffer,size,1,fp);
			fclose(fp);
		}
		delete buffer;
	}
}

void decryptMuBufferXOR3(unsigned char* buffer, size_t size)
{
	const unsigned char xorKeys[] = {0xFC, 0xCF, 0xAB};
	for (size_t i=0; i<size; ++i)
	{
		*buffer ^= xorKeys[i%3];
		buffer++;
	}
}
void decryptMuFileXOR3(const std::string& strSrcFilename, const std::string& strDestFilename)
{
	FILE* fp = fopen(strSrcFilename.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		unsigned char* buffer = new unsigned char[size];
		fread(buffer,size,1,fp);
		fclose(fp);
		fp = NULL;
		decryptMuBufferXOR3(buffer,size);
		fp = fopen(strDestFilename.c_str(), "wb");
		if (fp)
		{
			fwrite(buffer,size,1,fp);
			fclose(fp);
		}
		delete buffer;
	}
}

void decryptMuATTFile(const std::string& strSrcFilename, const std::string& strDestFilename)
{
	FILE* fp = fopen(strSrcFilename.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		unsigned char* buffer = new unsigned char[size];
		fread(buffer,size,1,fp);
		fclose(fp);
		fp = NULL;
		decryptMuBuffer(buffer,size);
		decryptMuBufferXOR3(buffer,size);
		fp = fopen(strDestFilename.c_str(), "wb");
		if (fp)
		{
			fwrite(buffer,size,1,fp);
			fclose(fp);
		}
		delete buffer;
	}
}

bool isEncBmd(const std::string& strSrcFilename)
{
	//FILE* fp = fopen(strSrcFilename.c_str(), "rb");
	//if (fp)
	//{
	//}
	return false;
}
