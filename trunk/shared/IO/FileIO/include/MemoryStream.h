#pragma once
#include "assert.h"

class CMemoryStream
{
	unsigned char* m_p;
	unsigned char* m_buffer;
	unsigned long m_uSize;
public:
	CMemoryStream()
	{
		m_p=NULL;
		m_buffer=NULL;
		m_uSize=0;
	}
	~CMemoryStream()
	{
		clear();
	}
	unsigned char* getPt()
	{
		return m_p;
	}
	size_t getCursorPos()
	{
		return m_p-m_buffer;
	}
	void setCursorPos(size_t pos)
	{
		m_p=m_buffer+pos;
	}
	//
	unsigned char* getBuffer()
	{
		return m_buffer;
	}
	void resize(unsigned long uSize)
	{
		clear();
		m_buffer = new unsigned char[uSize];
		m_p=m_buffer;
		m_uSize=uSize;
	}
	void clear()
	{
		if (m_buffer)
		{
			delete[] m_buffer;
		}
		m_p=NULL;
		m_uSize=0;
	}
	//
	bool read(unsigned char* pValue, unsigned long uSize)
	{
		if (m_p+uSize>m_buffer+m_uSize)
		{
			assert(false);
			return false;
		}
		memcpy(pValue,m_p,uSize);
		m_p+=uSize;
	}
	//
	template <class T>
	bool read(T& val)
	{
		return read((unsigned char*)&val,sizeof(T));
	}
	//
	template <class T>
	bool readVector(T& val, unsigned long uSize)
	{
		val.resize(uSize);
		return read((unsigned char*)&val[0],val.size()*sizeof(val[0]));
	}
	//
	bool write(unsigned char* pValue, unsigned long uSize)
	{
		if (m_p+uSize>m_buffer+m_uSize)
		{
			assert(false);
			return false;
		}
		memcpy(m_p,pValue,uSize);
		m_p+=uSize;
	}
	//
	template <class T>
	bool write(T& val)
	{
		return write((unsigned char*)&val,sizeof(T));
	}
	//
	template <class T>
	bool writeVector(T& val)
	{
		return write((unsigned char*)&val[0],val.size()*sizeof(val[0]));
	}
};