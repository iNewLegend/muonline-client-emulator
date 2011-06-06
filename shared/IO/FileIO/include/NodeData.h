#pragma once
#include <string>
#include "assert.h"

struct NodeBuffer
{
	unsigned long size;
	char* buffer;
	NodeBuffer();
	~NodeBuffer();
	void clear();
	bool createBuffer(size_t uSize, const void* srcBuffer);
};

class CNodeData
{
public:
	CNodeData();
	~CNodeData();
public:
	const std::string& GetName() const { return m_strName; }
	void SetName(const std::string& name) { m_strName = name;}

	void Clear();

	CNodeData*	GetParent()const{return m_pParent;}
	CNodeData*	LinkEndChild(CNodeData* pNode);
	bool		RemoveChild(CNodeData* pChild);

	CNodeData* LinkPrev(CNodeData* pNode);
	CNodeData* LinkNext(CNodeData* pNode);

	CNodeData* previousSibling()const{return m_pPrev;}
	CNodeData* previousSibling(const std::string& name)const;

	CNodeData* nextSibling()const{return m_pNext;}
	CNodeData* nextSibling(const std::string& name)const;

	CNodeData* indexChild(unsigned long uIndex);
	CNodeData* indexChild(unsigned long uIndex, const std::string& name);

	CNodeData* firstChild()const{return m_pfirstChild;}
	CNodeData* firstChild(const std::string& name)const;
	CNodeData* firstChild(unsigned long uIndex)const;

	CNodeData* lastChild()const{return m_pLastChild;}
	CNodeData* lastChild(const std::string& name)const;	

	void		SetVal(size_t uSize,const void* pVal);
	void		getVal(size_t uSize, void* pVal)const;
	CNodeData*	AddNode(unsigned long uIndex);
	CNodeData*	AddNode(const std::string& name);
	CNodeData*	SetVal(const std::string& name, size_t uSize,const void* pVal);
	CNodeData*	AddNode(unsigned long uIndex, const std::string& name);
	CNodeData*	SetVal(unsigned long uIndex, const std::string& name, size_t uSize,const void* pVal);
	//
	template <class T>
	void SetVal(const T& val)
	{
		SetVal(sizeof(val), &val);
	}
	template <class T>
	CNodeData*	SetVal(const std::string& name,const T& val)
	{
		return SetVal(name, sizeof(T), &val);
	}
	template <class T>
	CNodeData*	SetVal(unsigned long uIndex, const std::string& name,const T& val)
	{
		return SetVal(uIndex, name, sizeof(T), &val);
	}
	//
	template <class T>
	void	GetVal(T& val)const
	{
		getVal(sizeof(T),&val);
	}
	template <class T>
	CNodeData*	GetVal(const std::string& name, T& val)const
	{
		CNodeData* pNode = firstChild(name);
		if (pNode)
		{
			pNode->GetVal(val);
		}
		return pNode;
	}

	template <class T>
	CNodeData*	GetVal(unsigned long uIndex, const std::string& name, T& val)const
	{
		CNodeData* pNode = firstChild(name);
		//CNodeData* pNode = indexChild(uIndex, name);
		if (pNode)
		{
			pNode->GetVal(val);
		}
		return pNode;
	}
	//
	void	SetByte(unsigned char val)		{SetVal(val);}
	void	SetInt(int val)			{SetVal(val);}
	void	SetFloat(float val)		{SetVal(val);}
	void	SetBool(bool val)		{SetVal(val);}

	void	GetByte(unsigned char& val) const		{GetVal(val);}
	void	GetInt(int& val) const			{GetVal(val);}
	void	GetUint32(unsigned long& val) const	{GetVal(val);}
	void	GetFloat(float& val) const		{GetVal(val);}
	void	GetBool(bool& val) const		{GetVal(val);}

	CNodeData*	SetByte(const std::string& name, unsigned char val)		{return SetVal(name, val);}
	CNodeData*	SetInt(const std::string& name, int val)		{return SetVal(name, val);}
	CNodeData*	SetFloat(const std::string& name, float val)	{return SetVal(name, val);}
	CNodeData*	SetBool(const std::string& name, bool val)		{return SetVal(name, val);}

	CNodeData*	GetByte(const std::string& name, unsigned char& val)const	{return GetVal(name, val);}
	CNodeData*	GetInt(const std::string& name, int& val)const		{return GetVal(name, val);}
	CNodeData*	GetUint32(const std::string& name, unsigned long& val)const{return GetVal(name, val);}
	CNodeData*	GetFloat(const std::string& name, float& val)const	{return GetVal(name, val);}
	CNodeData*	GetBool(const std::string& name, bool& val)const	{return GetVal(name, val);}

	CNodeData*	SetByte(unsigned long uIndex, const std::string& name, unsigned char val)		{return SetVal(uIndex, name, val);}
	CNodeData*	SetInt(unsigned long uIndex, const std::string& name, int val)			{return SetVal(uIndex, name, val);}
	CNodeData*	SetFloat(unsigned long uIndex, const std::string& name, float val)		{return SetVal(uIndex, name, val);}
	CNodeData*	SetBool(unsigned long uIndex, const std::string& name, bool val)		{return SetVal(uIndex, name, val);}

	CNodeData*	GetByte(unsigned long uIndex, const std::string& name, unsigned char& val)const	{return GetVal(uIndex, name, val);}
	CNodeData*	GetInt(unsigned long uIndex, const std::string& name, int& val)const		{return GetVal(uIndex, name, val);}
	CNodeData*	GetUint32(unsigned long uIndex, const std::string& name, unsigned long& val)const	{return GetVal(uIndex, name, val);}
	CNodeData*	GetFloat(unsigned long uIndex, const std::string& name, float& val)const	{return GetVal(uIndex, name, val);}
	CNodeData*	GetBool(unsigned long uIndex, const std::string& name, bool& val)const		{return GetVal(uIndex, name, val);}


	NodeBuffer&	GetNodeBuffer(){ return m_NodeBuffer; }
	const NodeBuffer&	GetNodeBuffer()const{ return m_NodeBuffer; }
	CNodeData*	GetNodeBuffer(const std::string& name, NodeBuffer& lump);
	void		GetString(std::string& val)const;
	CNodeData*	GetString(const std::string& name, std::string& val)const;

	void		SetString(const char* str)	{SetVal(strlen(str), (void*)str);}
	CNodeData*	SetString(const std::string& name, const char* str)	{return SetVal(name, strlen(str), (const void*)str);}

	// Vector
	template <class T>
	CNodeData* SetVector(const std::string& name,const T& vector)
	{
		if (vector.size()>0)
		{
			return SetVal(name, vector.size()*sizeof(T::value_type), &vector[0]);
		}
		return NULL;
	}

	template <class T>
	void getVector(T& vector) const
	{
		assert(m_NodeBuffer.size%sizeof(T::value_type)==0);
		vector.resize(m_NodeBuffer.size / sizeof(T::value_type));
		getVal(m_NodeBuffer.size,&vector[0]);
	}
	template <class T>
	const CNodeData* getVector(const std::string& name, T& vector) const
	{
		const CNodeData* pNode = firstChild(name);
		if (pNode)
		{
			pNode->getVector(vector);
		}
		return pNode;
	}
protected:
	std::string		m_strName;
	CNodeData*		m_pParent;
	CNodeData*		m_pfirstChild;
	CNodeData*		m_pLastChild;

	CNodeData*		m_pPrev;
	CNodeData*		m_pNext;

	NodeBuffer		m_NodeBuffer;
};