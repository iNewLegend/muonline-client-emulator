#ifndef _MLANGUAGECONF_H
#define _MLANGUAGECONF_H

//#pragma once

#include <map>
#include <string>

using std::map;
using std::string;
using std::pair;

typedef map< string, string >	MsgGroupMap;
typedef MsgGroupMap::iterator	MsgGroupIter;
typedef pair< string, string >	MsgGroupPair;

const char CSC_ERR_MESSAGE_TYPE[]		= "cserror";
const char CSC_ERR_MESSAGE_FILE_NAME[] = "cserror.xml";

class MLanguageConf;
class MMessenger;
class MZFileSystem;

/*
 * ��� Ÿ���� string�̳� int������ ������ �ִ� Ŭ����.
 */
class MLanguageCode
{
public :
	enum LANGUAGE_CODE
	{
		KOR = 1,
		ENG,
		JPN,
		INTERNATIONAL,
	};

	MLanguageCode();
	~MLanguageCode();

	string	ConvCodeToStrLangType( const int nLangCode );
	int		ConvStrLangTypeToCode( const char* pszLangType );

	inline int GetLangCode() { return m_nLangCode; }

	inline void SetLangCode( const int nLangCode )		
	{ 
		m_nLangCode = nLangCode; 
		m_isCreated = true;
	}

	inline void SetLangCode( const char* pszLangType )	
	{ 
		m_nLangCode = ConvStrLangTypeToCode( pszLangType ); 
		m_isCreated = true;
	}

	inline bool IsCreated() { return m_isCreated; }

private :
	int		m_nLangCode;
	bool	m_isCreated;
};

/*
 * MLanguageConfŬ������ ���� ������ ������ ����� �޽��� ������ ������ ������.
 * MMessengerManager�� �����Ҷ� �⺻ �����ͷ� ����.
 */
class MMessageNameGroup
{
public :
	friend MLanguageConf;
	
	MMessageNameGroup();
	virtual ~MMessageNameGroup();

	string GetErrMessageDir();

	inline MsgGroupMap& GetGroup() { return m_mMsgGroup; }

public :
	virtual void Release() { m_mMsgGroup.clear(); }

protected :
	MsgGroupMap m_mMsgGroup;
};

/*
 * ��� Ÿ�Կ����� ȯ�� ����( ��� Ÿ��, ��� ���ϵ��� �ִ� ���� �� ).
 */
class MLanguageConf
{
public :
	MLanguageConf();
	virtual ~MLanguageConf();

	inline const string& GetRootDir() const		{ return m_strRootDir; }
	inline const string& GetLangtype() const	{ return m_strLangType; }
	inline const string& GetLangDir() const		{ return m_strLangDir; }

	bool Init( MZFileSystem* pfs, MMessageNameGroup* pMsgNameGroup, const char* pszRootDir = "" );

	inline bool IsHaveLangType() { return m_LangCode.IsCreated(); }

	inline MMessageNameGroup* GetMessageNameGroup() { return m_pMsgNameGroup; }

	inline int		GetCurLangCode() { return m_LangCode.GetLangCode(); }
	inline string	GetCurLangType() { return m_strLangType; }

	// ���� �������̽�.
	bool CheckFile( MZFileSystem* pfs, MMessageNameGroup* pMsgNameGroup );

	// ��� Ÿ�� ����.
	bool SetLangTypeByStr( const char* pszLangType );

	virtual void Release();

private :
	// �� ��ӹ��� Ŭ������ �´� ������ ������ ����θ� ����� ���ؼ� ������ �ؾ��ϴ� �Լ�.
	virtual void MakeCompleteDir() = 0;	
	// �߰����� �޽��� ������ ��� ��Ű���� �� �Լ��� ������ �Ͽ� ��Ͻ�Ű�� ��.
	virtual bool CheckSubTaskFile( MZFileSystem* pfs, MMessageNameGroup* pMsgNameGroup ) = 0;

protected :
	string			m_strRootDir;
	string			m_strLangType;
	string			m_strLangDir;
	MLanguageCode	m_LangCode;

	MMessageNameGroup* m_pMsgNameGroup;
};

/*
 * �޽����� ������. 
 * MMessageNameGroup�� MLanguageConf�� �����͸� �������� ����� �޽����� ������.
 */
class MMessengerManager
{
public :
	MMessengerManager();
	virtual ~MMessengerManager();

	bool Init( MMessageNameGroup* pMMsgNameGroup, MZFileSystem* pfs );

	inline MMessenger* GetErrMessage() { return m_pErrMessenger; }

	void Release();

private :
	// ���� �������̽�.
	bool Allocate( const string& strKey, const string& strFile, MZFileSystem* pfs );

private :
	virtual bool LoadFile( MMessageNameGroup* pMMsgNameGroup, MZFileSystem* pfs );

	// ��ӵ� Ŭ�������� �߰��� ��ϵ� �޽����� ���( �Ҵ� )�Ϸ��� �� �Լ��� ������ �ؾ���.
	virtual bool AllocateSubTask( const string& strKey, const string& strFile, MZFileSystem* pfs ) = 0;
	// ��ӵ� Ŭ�������� ��Ͻ�Ų ��ü�� �����ϱ� ���ؼ��� �� �Լ��� ������ �ؾ���.
	virtual void ReleaseSubTask() = 0;

protected :
	// ������ Ŭ���̾�Ʈ�� �������� ���Ǵ� �κ��� ���⿡ �����.

	MMessenger* m_pErrMessenger;
};

#endif