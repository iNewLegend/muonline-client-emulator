#pragma once
#include "NodeData.h"

class CLumpFile : public CNodeData
{
public:
	CLumpFile();
	~CLumpFile();
public:
	bool LoadFile(const std::string& strFilename);
	bool SaveFile(const std::string& strFilename) const;
};