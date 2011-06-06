#pragma once
#include "NodeData.h"

class CStringNodeFile : public CNodeData
{
public:
	CStringNodeFile();
	~CStringNodeFile();
public:
	bool LoadFile(const std::string& strFilename);
	bool SaveFile(const std::string& strFilename) const;
};