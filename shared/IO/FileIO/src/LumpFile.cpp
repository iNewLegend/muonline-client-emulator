#include "LumpFile.h"
#include "IORead.h"

const char NAME_END = ':';
const char NODE_END = ';';

CLumpFile::CLumpFile()
{
}

CLumpFile::~CLumpFile()
{
}

void ParseLumpFile(CNodeData& nodeData, unsigned long& p, char* buffer)
{
	// Name
	std::string strName;
	for (int i = 0; i < 33; i++)
	{
		if (buffer[p] == NAME_END)
		{
			p++;
			break;
		}
		strName += buffer[p];
		p++;
		assert(i<32);
	}
	nodeData.SetName(strName);
	// Buffer
	unsigned long uBufferSize;
	memcpy(&uBufferSize, &buffer[p], sizeof(unsigned long));
	p += sizeof(unsigned long);
	NodeBuffer&	nodeBuffer = nodeData.GetNodeBuffer();
	nodeBuffer.createBuffer(uBufferSize,&buffer[p]);
	p += uBufferSize;
	// Child
	while (buffer[p] != NODE_END)
	{
		CNodeData* pNode = new CNodeData();
		nodeData.LinkEndChild(pNode);
		ParseLumpFile(*pNode, p, buffer);
	}
	p++;
	return;
}

void WriteLumpFile(const CNodeData& nodeData, FILE* cfile)
{
	// Name
	const std::string& strName = nodeData.GetName();
	fwrite(strName.c_str(), strName.length(), 1, cfile);
	fwrite(&NAME_END, 1, 1, cfile);
	// Buffer
	const NodeBuffer&	nodeBuffer = nodeData.GetNodeBuffer();
	fwrite(&nodeBuffer.size, sizeof(unsigned long), 1, cfile);
	if (nodeBuffer.size>0)
	{
		fwrite(nodeBuffer.buffer, nodeBuffer.size, 1, cfile);
	}
	// Child
	for (CNodeData* pNode = nodeData.firstChild(); pNode; pNode = pNode->nextSibling())
	{
		WriteLumpFile(*pNode,cfile);
	}
	fwrite(&NODE_END, 1, 1, cfile);
}

bool CLumpFile::LoadFile(const std::string& strFilename)
{
	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	if (pRead)
	{
		Clear();
		if (pRead->GetSize()>0)
		{
			char* pBuffer = new char [pRead->GetSize()];
			if (pRead->Read(pBuffer, pRead->GetSize())==1)
			{
				IOReadBase::autoClose(pRead);
				unsigned long p = 0; 
				ParseLumpFile(*this, p, pBuffer);
				return true;
			}
			delete[] pBuffer;
		}
		IOReadBase::autoClose(pRead);
	}
	return false;
}

bool CLumpFile::SaveFile(const std::string& strFilename)const
{
	FILE* fp = fopen(strFilename.c_str(), "wb");
	if (fp)
	{
		WriteLumpFile(*this,fp);
		fclose(fp);
		return true;
	}
	return false;
}