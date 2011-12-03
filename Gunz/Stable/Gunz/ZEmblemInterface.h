#ifndef _ZEMBLEMINTERFACE_H
#define _ZEMBLEMINTERFACE_H

/***********************************************************************
  ZEmblemInterface.h
  
  ��  �� : �ϵ� ����̺꿡 ����� ���� ��Ʈ���� �޸� ĳ���� �ε��ϰ�
           ��û�� ��Ʈ�� �����͸� �Ѱ���.
  �ۼ��� : 18, Nov, 2004
  �ۼ��� : �ӵ�ȯ
************************************************************************/


//#pragma once
#include <string>							// Include string
#include <map>								// Include map
#include "MMatchGlobal.h"					// Include MMatchGlobal.h
#include "MBitmap.h"						// Include MBitmap.h
using namespace std;



// Class : ZEmblemMemNode
// ���� ���� �� Ŭ�� ������ map list �������� �����ϱ� ���� ���
struct ZEmblemInfoNode
{
	UINT		m_nClanID;								// Clan ID
	UINT		m_nNumOfClan;							// Number of people in clan
	MBitmap*	m_pBitmapEmblem;						// Clan bitmap emblem

	// Initialize
	ZEmblemInfoNode() {									// Constructor
		m_nClanID			= 0;
		m_nNumOfClan		= 0;
		m_pBitmapEmblem		= NULL;
	}

	virtual ~ZEmblemInfoNode() {
		SAFE_DELETE(m_pBitmapEmblem);
	}
};

typedef  map<int,ZEmblemInfoNode>  EmblemInfoMapList;


// Class : ZEmblemInterface
// ���� ������ ���� �� �ε��ϱ� ���� Ŭ����
class ZEmblemInterface
{
public:		// public variables


public:		// public functions
	ZEmblemInterface( void);									// Constructor
	~ZEmblemInterface( void);									// Destructor

	void Create(void);
	void Destroy(void);

	bool AddClanInfo( UINT nClanID);							// Add clan infomation
	bool DeleteClanInfo( UINT nClanID);							// Delete clan infomation
	bool ClearClanInfo( void);									// Clear clan infomation
	bool ReloadClanInfo( UINT nClanID);							// ������ bitmap�� �ε��Ѵ�

	bool ZEmblemInterface::FindClanInfo( UINT nClanID, EmblemInfoMapList::iterator* pIterator);		// Find clan infomation

	MBitmap* GetClanEmblem( UINT nClanID);						// Get pointer of bitmap emblem
	MBitmap* GetClanEmblem2( UINT nClanID);						// Get pointer of bitmap emblem

protected:	// protected varialbes
	EmblemInfoMapList	m_EmblemInfoMap;						// Map list of emblem infomation
	MBitmap*			m_pBitmapNoEmblem;						// Bitmap pointer of "no emblem"

protected:	// protected functions
};


#endif