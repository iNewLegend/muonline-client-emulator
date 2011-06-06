#include "StringNodeFile.h"
#include "IORead.h"
#include "Windows.h"

const char NAME_END = ':';
const char NODE_END = ';';

CStringNodeFile::CStringNodeFile()
{
}

CStringNodeFile::~CStringNodeFile()
{
}

std::string getString(unsigned long& p, char* buffer, char end)
{
	std::string str;
	for (int i = 0; i < 256; i++)
	{
		if (buffer[p] == end)
		{
			p+=2;
			break;
		}
		str += buffer[p];
		p+=2;
		assert(i<255);
	}
	return str;
}

void ParseStringNodeFile(CNodeData& nodeData, unsigned long& p, char* buffer)
{
	if (buffer[p]=='[')
	{
		p+=2;
		std::string strName = getString(p, buffer, ']');
		nodeData.SetName(strName);
		p+=4; // \n

		while (buffer[p+2] != '/')
		{
			CNodeData* pNode = new CNodeData();
			nodeData.LinkEndChild(pNode);
			ParseStringNodeFile(*pNode, p, buffer);
		}
		p+=4;
		getString(p, buffer, ']');
		p+=4; // \n
	}
	else if (buffer[p]=='<')
	{
		p+=2;
		std::string strType = getString(p, buffer, '>');
		// Name
		std::string strName = getString(p, buffer, ':');
		nodeData.SetName(strName);
		// Val
		std::string strVal = getString(p, buffer, 13);
		p+=2;
		// Buffer
		NodeBuffer&	nodeBuffer = nodeData.GetNodeBuffer();
		if ("STRING"==strType)
		{
			nodeBuffer.createBuffer(strVal.length(),strVal.c_str());
		}
		else if ("INTEGER"==strType)
		{
			int nVal=atoi(strVal.c_str());
			nodeBuffer.createBuffer(sizeof(nVal),&nVal);
		}
		else if ("UNSIGNED INT"==strType)
		{
			unsigned int uVal=atoi(strVal.c_str());
			nodeBuffer.createBuffer(sizeof(uVal),&uVal);
		}
		else if ("INTEGER64"==strType)
		{
			__int64 nVal=_atoi64(strVal.c_str());
			nodeBuffer.createBuffer(sizeof(nVal),&nVal);
		}
		else if ("FLOAT"==strType)
		{
			float fVal=(float)atof(strVal.c_str());
			nodeBuffer.createBuffer(sizeof(fVal),&fVal);
		}
		else if ("BOOL"==strType)
		{
			bool bVal=("True"==strVal);
			nodeBuffer.createBuffer(sizeof(bVal),&bVal);
		}
		else
		{
			MessageBoxA(NULL,strType.c_str(),"ParseStringNodeFile() Can't find this type.",0);
		}
	}
}

void WriteStringNodeFile(const CNodeData& nodeData, FILE* cfile)
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
		WriteStringNodeFile(*pNode,cfile);
	}
	fwrite(&NODE_END, 1, 1, cfile);
}

bool CStringNodeFile::LoadFile(const std::string& strFilename)
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
				unsigned long p = 2; 
				ParseStringNodeFile(*this, p, pBuffer);
				return true;
			}
			delete[] pBuffer;
		}
		IOReadBase::autoClose(pRead);
	}
	return false;
}

bool CStringNodeFile::SaveFile(const std::string& strFilename)const
{
	FILE* fp = fopen(strFilename.c_str(), "wb");
	if (fp)
	{
		WriteStringNodeFile(*this,fp);
		fclose(fp);
		return true;
	}
	return false;
}