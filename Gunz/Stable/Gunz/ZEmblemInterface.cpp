/***********************************************************************
  ZEmblemInterface.cpp
  
  ��  �� : �ϵ� ����̺꿡 ����� ���� ��Ʈ���� �޸� ĳ���� �ε��ϰ�
           ��û�� ��Ʈ�� �����͸� �Ѱ���.
  �ۼ��� : 18, Nov, 2004
  �ۼ��� : �ӵ�ȯ
************************************************************************/


#include "stdafx.h"							// Include stdafx.h
#include "ZEmblemInterface.h"				// Include ZEmblemInterface.h
#include "MResourceManager.h"				// Include MResourceManager.h
//#include "MEmblemMgr.h"					// Include MEmblemMgr.h
#include "FileInfo.h"
#include "ZGameClient.h"



/***********************************************************************
  ZEmblemInterface : public
  
  desc : ������
************************************************************************/
ZEmblemInterface::ZEmblemInterface( void)
{
}


/***********************************************************************
  ~ZEmblemInterface : public
  
  desc : �Ҹ���
************************************************************************/
ZEmblemInterface::~ZEmblemInterface( void)
{
	Destroy();
}

void ZEmblemInterface::Create(void)
{
	// "no emblem"���� �� ��Ʈ�� �����͸� �̸� ���Ѵ�.
	m_pBitmapNoEmblem = MBitmapManager::Get( "no_emblem.png");
}

void ZEmblemInterface::Destroy(void)
{
	ClearClanInfo();
}

/***********************************************************************
  AddClanInfo : public
  
  desc : Ŭ�� ������ �߰��Ѵ�. �÷��̾ �α׿� �Ҷ����� ȣ�����ָ� �ȴ�.
  arg  : nClanID = Ŭ�� ID��
  ret  : true(add) or false(do not add)
************************************************************************/
bool ZEmblemInterface::AddClanInfo( UINT nClanID)
{
	if(nClanID==0) return false;

	// ���� ���� ����Ʈ���� �ش� ���� ������ �ִ��� �˻��Ѵ�.
	EmblemInfoMapList::iterator Iterator;
	if ( FindClanInfo( nClanID, &Iterator))
	{
		// �̹� �����ϸ� �ش� ������ �ο����� �Ѹ� �ø���.
		(*Iterator).second.m_nNumOfClan++;

		return ( false);					// return : false
	}
	
	// ����Ʈ�� ������ ��带 ���� �����Ѵ�.
	ZEmblemInfoNode EmblemNode;
	EmblemNode.m_nClanID		= nClanID;
	EmblemNode.m_nNumOfClan		= 1;

	// ����Ʈ�� ��� �߰�
	m_EmblemInfoMap.insert( EmblemInfoMapList::value_type( nClanID, EmblemNode));

	// bitmap �ε�
	ReloadClanInfo(nClanID);

	// �����ǰ� ������ �ʴ� ��Ʈ���� �޸� ĳ������ �����Ѵ�(������ ���� ��).
	if ( 0)
	{
		for ( EmblemInfoMapList::iterator i = m_EmblemInfoMap.begin();  i != m_EmblemInfoMap.end();  i++)
		{
			if ( (*i).second.m_nNumOfClan <= 1)		// ���� �ں��� ����ϴ� �ο��� 1�� ���϶��...
				m_EmblemInfoMap.erase( i);			// ����Ʈ���� �����Ѵ�.
		}
	}

	return ( true);							// return : true
}

bool ZEmblemInterface::ReloadClanInfo(UINT nClanID)
{
	EmblemInfoMapList::iterator Iterator;
	if ( !FindClanInfo( nClanID, &Iterator)) return false;

	ZEmblemInfoNode &EmblemNode = Iterator->second;

	char szFilePath[256];
	if(!ZGetGameClient()->GetEmblemManager()->GetEmblemPathByCLID(nClanID,szFilePath)) 
		return false;

	SAFE_DELETE(EmblemNode.m_pBitmapEmblem);

#ifdef _PUBLISH
	MZFile::SetReadMode( MZIPREADFLAG_ZIP | MZIPREADFLAG_MRS | MZIPREADFLAG_MRS2 | MZIPREADFLAG_FILE );
#endif

	// Ŭ��ID ���� �̿��Ͽ� �ش� ��Ʈ���� �޸� ĳ���� �ε��Ѵ�.
	MBitmapR2 *pBitmap = new MBitmapR2;
	pBitmap->Create("clanEmblem",RGetDevice(),szFilePath,false);
	EmblemNode.m_pBitmapEmblem = pBitmap;

#ifdef _PUBLISH
	MZFile::SetReadMode( MZIPREADFLAG_MRS2 );
#endif

	return true;
}

/***********************************************************************
  DeleteClanInfo : public
  
  desc : �ش� ClanID�� ����ϴ� �ο� ���� 0���̸� Ŭ�� ������ �����Ѵ�.
         �÷��̾ �α׿��� �Ҷ����� ȣ�����ָ� �ȴ�.
  arg  : nClanID = Ŭ�� ID��
  ret  : true(delete) or false(do not delete)
************************************************************************/
bool ZEmblemInterface::DeleteClanInfo( UINT nClanID)
{
	if(nClanID==0) return false;

	// ����Ʈ���� �ش� Ŭ��ID�� �ִ��� �˻��Ѵ�.
	EmblemInfoMapList::iterator Iterator;
	if ( !FindClanInfo( nClanID, &Iterator))
		return ( false);					// return : false


	// �ش� Ŭ�� ������ ����ϴ� �ο����� 1�� ���ҽ�Ų��.
	(*Iterator).second.m_nNumOfClan--;

	// �ش� Ŭ�� ������ ����ϴ� �ο����� 0���̸� ����Ʈ���� �����Ѵ�.
	if ( (*Iterator).second.m_nNumOfClan == 0)
		m_EmblemInfoMap.erase( Iterator);

	return ( true);							// return : true
}


/***********************************************************************
  ClearClanInfo : public
  
  desc : Ŭ�� ������ ������ �ʱ�ȭ �Ѵ�.
  arg  : none
  ret  : true(success) or false(fail)
************************************************************************/
bool ZEmblemInterface::ClearClanInfo( void)
{
	m_EmblemInfoMap.clear();

	return ( true);							// return : true
}


/***********************************************************************
  GetClanEmblem : public
  
  desc : �ش� Ŭ���� ��Ʈ�� ���� �����͸� ��´�. ��� ���� �������Ѵ�
  arg  : nClanID = Ŭ�� ID��
  ret  : NULL(fail)  or
         Pointer of bitmap(success)
************************************************************************/
MBitmap* ZEmblemInterface::GetClanEmblem( UINT nClanID)
{
	if(nClanID==0) return NULL;

	// ����Ʈ���� �ش� Ŭ��ID�� �ִ��� �˻��Ѵ�.
	EmblemInfoMapList::iterator Iterator;
	if ( !FindClanInfo( nClanID, &Iterator))
	{
		// ����Ʈ�� ������ "no emblem" ��Ʈ�� �����͸� �����Ѵ�.
		return m_pBitmapNoEmblem;
	}

	// ����Ʈ�� �ش� Ŭ���� ������ ��Ʈ�� ���� �����͸� ���Ѵ�.
	return Iterator->second.m_pBitmapEmblem;
}


/***********************************************************************
  GetClanEmblem2 : public
  
  desc : �ش� Ŭ���� ��Ʈ�� ���� �����͸� ��´�. ��� ���� �������Ѵ�
  arg  : nClanID = Ŭ�� ID��
  ret  : NULL(fail)  or
         Pointer of bitmap(success)
************************************************************************/
MBitmap* ZEmblemInterface::GetClanEmblem2( UINT nClanID)
{
	if(nClanID==0) return NULL;

	// ����Ʈ���� �ش� Ŭ��ID�� �ִ��� �˻��Ѵ�.
	EmblemInfoMapList::iterator Iterator;
	if ( !FindClanInfo( nClanID, &Iterator))
	{
		// ����Ʈ�� ������ "no emblem" ��Ʈ�� �����͸� �����Ѵ�.
		return m_pBitmapNoEmblem;
	}

	if ( Iterator->second.m_pBitmapEmblem == NULL)
		// ���� ��Ʈ���� ������ "no emblem" ��Ʈ�� �����͸� �����Ѵ�.
		return m_pBitmapNoEmblem;

	// ����Ʈ�� �ش� Ŭ���� ������ ��Ʈ�� ���� �����͸� ���Ѵ�.
	return Iterator->second.m_pBitmapEmblem;
}


/***********************************************************************
  GetClanName : public
  
  desc : �ش� Ŭ���� �̸��� ��´�.
  arg  : nClanID = Ŭ�� ID��
  ret  : NULL(fail)  or
         String of clan name
************************************************************************/
//const char* ZEmblemInterface::GetClanName( UINT nClanID)
//{
//	// ����Ʈ���� �ش� Ŭ��ID�� �ִ��� �˻��Ѵ�.
//	EmblemInfoMapList::iterator Iterator;
//	if ( !FindClanInfo( nClanID, &Iterator))
//		return ( NULL);						// return : NULL
//
//	// ����Ʈ�� �ش� Ŭ���� ������ �̸����� ���Ѵ�.
//	char* szClanName = NULL;
//	strcpy( szClanName, (*Iterator).second.m_szClanName);
//
//	return ( szClanName);					// return : clan name
//}


/***********************************************************************
  FindClanInfo : public
  
  desc : �ش� Ŭ�� ������ �ִ��� �˻��Ѵ�. ������ ����Ʈ ��ġ�� �����Ѵ�.
  arg  : nClanID = Ŭ�� ID��
         pIterator = ���� ���� iterator ������
  ret  : true(found) or false(not found)
************************************************************************/
bool ZEmblemInterface::FindClanInfo( UINT nClanID, EmblemInfoMapList::iterator* pIterator)
{
	EmblemInfoMapList::iterator Iterator;
	Iterator = m_EmblemInfoMap.find( nClanID);

	if ( Iterator == m_EmblemInfoMap.end())
		return ( false);					// return : not found

	*pIterator = Iterator;					// Set list iterator

	return ( true);							// return : found
}
