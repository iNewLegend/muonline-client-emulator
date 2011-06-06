#include <windows.h>
#include "NetStringPacker.h"

CNetStringPacker::CNetStringPacker()
{
	memset(m_pBuf, 0L, MAX_STRING_PACKER_BUFFER);
}

CNetStringPacker::~CNetStringPacker()
{
}

bool CNetStringPacker::addRef()
{
	if (GetCount()>=255)
	{
		return false;
	}
	*((UCHAR*)m_pBuf)=GetCount()+1;
	return true;
}

bool CNetStringPacker::AddString(const std::string& strString)
{
	unsigned long uLen	=strString.length();
	if(uLen > 255)
		return false;

	unsigned long uCurSize	=this->GetSize();
	if(MAX_STRING_PACKER_BUFFER < uLen+uCurSize)
		return false;

	if (addRef()==false)
	{
		return false;
	}
	UCHAR* pBuf	=(UCHAR*)m_pBuf;
	pBuf	+=uCurSize;
	*pBuf	=uLen;
	if (0!=uLen)
	{
		pBuf++;
		memcpy(pBuf, strString.c_str(), uLen);
	}
	return true;
}

bool CNetStringPacker::getString(size_t index, std::string& strString)const
{
	strString.clear();

	size_t uCount =this->GetCount();
	if(index >= uCount)
		return false;

	const char*	pBuf	=(const char*)m_pBuf;
	pBuf++;

	for(size_t i=0; i<uCount; ++i)
	{
		UCHAR ucStrSize	=*pBuf;	// get size...
		pBuf++;
		if(256 <= (unsigned long)ucStrSize)
		{
			return false;
		}
		if(index == i)	// got it...
		{
			strString.append(pBuf,ucStrSize);
			return true;
		}
		// next string
		pBuf +=ucStrSize;
	}
	return false;
}

bool CNetStringPacker::GetString(unsigned long uIndex, char* pszStrBuf, unsigned long uBufSize)
{
	memset(pszStrBuf, 0L, uBufSize);
	UCHAR*	pBuf	=(UCHAR*)m_pBuf;
	pBuf++;
	unsigned long uCount	=this->GetCount();
	if(uIndex >= uCount)
		return false;

	for(unsigned long i=0; i<uCount; i++)
	{
		UCHAR ucStrSize	=*pBuf;	// get size...
		if(uBufSize <= (unsigned long)ucStrSize)
		{
			return false;
		}
		if(uIndex == i)	// got it...
		{
			memcpy(pszStrBuf, pBuf+1, (int)ucStrSize);
			return true;
		}
		// next string
		pBuf	+=(1+ucStrSize);
	}
	return false;
}

size_t CNetStringPacker::GetCount()const
{
	UCHAR*	pBuf	=(UCHAR*)m_pBuf;
	return *pBuf;
}

size_t CNetStringPacker::GetSize()
{
	UCHAR*	pBuf	=(UCHAR*)m_pBuf;
	pBuf++;

	unsigned long uObjSize	=1;
	unsigned long uCount	=this->GetCount();
	for(unsigned long i=0; i<uCount; ++i)
	{
		UCHAR ucStrSize	=*pBuf;
		pBuf	+=(1+ucStrSize);

		uObjSize	+=(1+ucStrSize);
	}
	return uObjSize;
}

size_t CNetStringPacker::GetFreeBufferSize()
{
	return sizeof(*this)-GetSize();
}