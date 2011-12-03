#ifndef MXML_H
#define MXML_H
//#pragma once

#include "winsock2.h"
#include "windows.h"
#include <comutil.h>
#include <stdio.h>
#include <string>
using namespace std;

//#define _MSXML2


#ifdef _MSXML2
	#import "msxml4.dll" named_guids no_implementation
//	using namespace MSXML2;

	typedef MSXML2::IXMLDOMDocumentPtr				MXmlDomDocPtr;
	typedef MSXML2::IXMLDOMNodePtr					MXmlDomNodePtr;
	typedef MSXML2::IXMLDOMNodeListPtr				MXmlDomNodeListPtr;
	typedef MSXML2::IXMLDOMElementPtr				MXmlDomElementPtr;
	typedef MSXML2::IXMLDOMProcessingInstructionPtr MXmlDomPIPtr;
	typedef MSXML2::IXMLDOMNamedNodeMapPtr			MXmlDomNamedNodeMapPtr;
	typedef MSXML2::IXMLDOMTextPtr					MXmlDomTextPtr;
	typedef MSXML2::IXMLDOMParseErrorPtr			MXmlDomParseErrorPtr;
#else
	#import "msxml.dll" named_guids no_implementation

	typedef MSXML::IXMLDOMDocumentPtr				MXmlDomDocPtr;
	typedef MSXML::IXMLDOMNodePtr					MXmlDomNodePtr;
	typedef MSXML::IXMLDOMNodeListPtr				MXmlDomNodeListPtr;
	typedef MSXML::IXMLDOMElementPtr				MXmlDomElementPtr;
	typedef MSXML::IXMLDOMProcessingInstructionPtr	MXmlDomPIPtr;
	typedef MSXML::IXMLDOMNamedNodeMapPtr			MXmlDomNamedNodeMapPtr;
	typedef MSXML::IXMLDOMTextPtr					MXmlDomTextPtr;
	typedef MSXML::IXMLDOMParseErrorPtr				MXmlDomParseErrorPtr;

//	using namespace MSXML;
#endif

class MXmlDocument;

/// IXMLDOMNode ���� Ŭ����
class MXmlNode
{
private:

protected:
	MXmlDomNodePtr		m_pDomNode;			///< IXMLDOMNode ����Ʈ ������
public:
	/// Default constructor.
	MXmlNode() { m_pDomNode = NULL; }
	MXmlNode(MXmlDomNodePtr a_pDomNode) { m_pDomNode = a_pDomNode; }
	/// Default destructor.
	virtual ~MXmlNode() { m_pDomNode = NULL; }

	/// IXMLDOMNode�������̽��� ��ȯ
	MXmlDomNodePtr	GetXmlDomNodePtr() { return m_pDomNode; }
	/// IXMLDOMNode�� ����
	void			SetXmlDomNodePtr(MXmlDomNodePtr pNode) { m_pDomNode = pNode; }

	/// IXMLDOMNode�� NULL���� ����
	bool IsEmpty() { if (m_pDomNode == NULL) return true; else return false; }
	/// Node�� �̸��� ��ȯ.
	/// @param sOutStr			[out] ��ȯ��
	void GetNodeName(char* sOutStr);
	/// Node�� Text�� ��ȯ.
	/// @param sOutStr			[out] ��ȯ��
	void GetText(char* sOutStr, int nMaxCharNum = -1);
	/// Node�� Text�� ����.
	/// @param sOutStr			[in] ������ Text
	void SetText(const char* sText);
	
	/// Child Node�� ������ ��ȯ.
	int	GetChildNodeCount();
	/// Node�� Ÿ���� ��ȯ.
	DOMNodeType GetNodeType();
	/// Child Node�� �ֳ�?
	bool HasChildNodes();

	void NextSibling();
	void PreviousSibling();

	bool AppendChild(MXmlNode node);

	/// �ش��̸��� ���� Child Node�� ã�´�.
	/// ������ NULL�� ����
	/// @param sNodeName		[in] ã�� Node�� �̸�
	bool FindChildNode(const char* sNodeName, MXmlNode* pOutNode);

	/// �θ� Node�� ��ȯ. ������ NULL�� ��ȯ�Ѵ�.
	MXmlNode GetParent() { if (m_pDomNode) return MXmlNode(m_pDomNode->parentNode); else return MXmlNode(); }
	/// �ε����� Child Node�� ��ȯ
	/// @param iIndex			[in] �ε���
	MXmlNode GetChildNode(int iIndex);

	/// ���Ͻ��� �̿��Ͽ� �ش� ��带 ã�´�. ���� ���� ã�� ù��常 ��ȯ
	/// @param sQueryStr		[in] ���Ͻ�
	MXmlNode SelectSingleNode(TCHAR* sQueryStr);
	/// ���ϽĿ� �´� �������� ��带 ã�´�.
	/// @param sQueryStr		[in] ���Ͻ�
	MXmlDomNodeListPtr	SelectNodes(TCHAR* sQueryStr);

	MXmlNode& operator= (MXmlNode aNode) { m_pDomNode = aNode.GetXmlDomNodePtr(); return *this; }
};

/// IXMLDOMElement ���� Ŭ����
class MXmlElement: public MXmlNode
{
private:

protected:

public:
	/// Default constructor.
	MXmlElement() { }
	MXmlElement(MXmlDomElementPtr a_pDomElement)	{ m_pDomNode = a_pDomElement; }
	MXmlElement(MXmlDomNodePtr a_pDomNode)			{ m_pDomNode = a_pDomNode; }
	MXmlElement(MXmlNode aNode)						{ m_pDomNode = aNode.GetXmlDomNodePtr(); }
	/// Default destructor.
	virtual ~MXmlElement() { }
	/// �±� �̸��� ��ȯ�Ѵ�.
	/// @param sOutStr			[out] �±� �̸�
	void GetTagName(char* sOutStr) { MXmlNode::GetNodeName(sOutStr); }
	
	/// �ش��±׷� �ѷ����� Contents�� ��ȯ
	/// @param sOutStr			[out] ��ȯ��
	void GetContents(char* sOutStr) { MXmlNode::GetText(sOutStr); }
	void GetContents(int* ipOutValue);
	void GetContents(bool* bpOutValue);
	void GetContents(float* fpOutValue);
	void GetContents(string* pstrValue);

	/// Contents�� ����
	void SetContents(const char* sStr) { MXmlNode::SetText(sStr); }
	void SetContents(int iValue);
	void SetContents(bool bValue);
	void SetContents(float fValue);

	bool GetChildContents(char* sOutStr, const char* sChildTagName, int nMaxCharNum = -1);
	bool GetChildContents(int* iOutValue, const char* sChildTagName);
	bool GetChildContents(float* fOutValue, const char* sChildTagName);
	bool GetChildContents(bool* bOutValue, const char* sChildTagName);

	/// �Ӽ����� ��ȯ - �����˻��̶� �ð��� ���� �ɸ���.
	/// @param sOutText			[out] ��ȯ�� �Ӽ���
	/// @param sAttrName		[in] �Ӽ� �̸�
	bool GetAttribute(char* sOutText, const char* sAttrName, char* sDefaultText = "");
	bool GetAttribute(int* ipOutValue, const char* sAttrName, int nDefaultValue = 0);
	bool GetAttribute(bool* bOutValue, const char* sAttrName, bool bDefaultValue = false);
	bool GetAttribute(float* fpOutValue, const char* sAttrName, float fDefaultValue = 0.0f);
	bool GetAttribute(string* pstrOutValue, const char* sAttrName, char* sDefaultValue = "");
	/// �Ӽ��� �߰��Ѵ�.
	/// @param sAttrName		[in] �Ӽ� �̸�
	/// @param sAttrText		[in] �Ӽ���
	bool AddAttribute(const char* sAttrName, const char* sAttrText);
	bool AddAttribute(const char* sAttrName, int iAttrValue);
	bool AddAttribute(const char* sAttrName, bool bAttrValue);
	/// �Ӽ��� �缳��. �ش�Ӽ��� �������� ������ �߰��Ѵ�.
	/// @param sAttrName		[in] �Ӽ� �̸�
	/// @param sAttrText		[in] �Ӽ���
	bool SetAttribute(const char* sAttrName, char* sAttrText);
	bool SetAttribute(const char* sAttrName, int iAttrValue);
	/// �Ӽ��� �����.
	bool RemoveAttribute(const char* sAttrName);

	int GetAttributeCount();
	/// �Ӽ��� ��ȯ - ���� GetAttribute�Լ����� ������.
	void GetAttribute(int index, char* szoutAttrName, char* szoutAttrValue);

	/// �ڽ� Element�� �߰��Ѵ�.
	/// @param sTagName			[in] �±� �̸�
	/// @param sTagText			[in] �±� Contents
	bool AppendChild(const char* sTagName, const char* sTagText = NULL);
	bool AppendChild(MXmlElement aChildElement);

	/// �ڽ� Element�� �߰��Ѵ�.
	/// @param sTagName			[in] �±� �̸�
	MXmlElement	CreateChildElement(const char* sTagName);

	/// �ؽ�Ʈ�� �߰��Ѵ�.
	bool AppendText(const char* sText);

	MXmlElement& operator= (MXmlElement aElement) { m_pDomNode = aElement.GetXmlDomNodePtr(); return *this; }
	MXmlElement& operator= (MXmlNode aNode) { m_pDomNode = aNode.GetXmlDomNodePtr(); return *this; }
};

/// XML Document Ŭ����.
class MXmlDocument
{
private:
	bool							m_bInitialized;		///< �ʱ�ȭ�Ǿ����� ����
	MXmlDomDocPtr*					m_ppDom;			///< IXMLDOMDocument
protected:

public:
	/// Default constructor.
	MXmlDocument();
	/// Default destructor.
	virtual ~MXmlDocument();

	/// �ʱ�ȭ. �� �Լ��� �� Ŭ������ ����ϱ� ���� �� ����Ǿ�� �Ѵ�
	bool				Create(void);
	/// ������.
	bool				Destroy(void);

	/// XML ������ �д´�.
	bool				LoadFromFile(const char* m_sFileName);
	/// XML �޸𸮹��۷κ��� �д´�.
	bool				LoadFromMemory(char* szBuffer, LANGID lanid = LANG_KOREAN);

	/// XML ���Ϸ� ����.
	bool				SaveToFile(const char* m_sFileName);

	/// processing instruction node�� ����.
	bool				CreateProcessingInstruction( const char* szHeader = "version=\"1.0\"");
	/// Node�� ����. ���� Child Node�� ������ �Բ� �����Ѵ�.
	bool				Delete(MXmlNode* pNode);

	MXmlElement			CreateElement(const char* sName);

	bool				AppendChild(MXmlNode node);

	/// XML DOM tree �ֻ��� Node�� ��ȯ.
	MXmlDomDocPtr		GetDocument()	{ return (*m_ppDom); }
	/// �ֻ��� Element�� ��ȯ.
	MXmlElement			GetDocumentElement()	{ return MXmlElement((*m_ppDom)->documentElement); }

	/// �±� �̸����� Element�� ã�´�. ���� ���� ã�� ù��常 ��ȯ.
	MXmlNode			FindElement(TCHAR* sTagName);

	bool				IsInitialized() { return m_bInitialized; }
};

// Utils
#define _BSTRToAscii(s) (const char*)(_bstr_t)(s)
BSTR _AsciiToBSTR(const char* ascii);

#endif