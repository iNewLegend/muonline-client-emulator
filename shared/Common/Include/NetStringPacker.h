#pragma once
#include "Common.h"

const int MAX_STRING_PACKER_BUFFER = 1024;
class CNetStringPacker  
{
public:
	CNetStringPacker();
	~CNetStringPacker();
	bool	AddString(const std::string& strString);
	bool	getString(size_t index, std::string& strString)const;
	bool	GetString(unsigned long uIndex, char* pszStrBuf, unsigned long uBufSize);
	size_t	GetCount()const;
	size_t	GetSize();
	size_t	GetFreeBufferSize();
private:
	bool	addRef();
	char	m_pBuf[MAX_STRING_PACKER_BUFFER];
};

const int MAX_DYNAMIC_PACKER_SIZE = 256;
template<class T>
class CNetDynamicPacker  
{
public:
	//CNetDynamicPacker():m_uSize(0)
	//{
	//	memset(m_Data, 0L, MAX_DYNAMIC_PACKER_SIZE*sizeof(T));
	//}
	virtual ~CNetDynamicPacker(){}
	const T& operator[](int nIndex) const { return m_Data[nIndex]; }
	T& operator[](int nIndex) { return m_Data[nIndex]; }
	bool	Append(const T& _Val)
	{
		if(MAX_DYNAMIC_PACKER_SIZE <= m_uSize)
			return false;
		m_Data[m_uSize]=_Val;
		m_uSize++;
		return true;
	}
	size_t	GetCount(){return m_uSize;}
	size_t	GetSize(){return GetCount()*sizeof(T)+sizeof(m_uSize);}
	size_t	GetFreeBufferSize(){return sizeof(*this)-GetSize();}
private:
	unsigned long	m_uSize;
	T		m_Data[MAX_DYNAMIC_PACKER_SIZE];
};