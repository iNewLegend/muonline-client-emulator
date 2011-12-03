#ifndef ZCHARACTERVIEW_H
#define ZCHARACTERVIEW_H

#include "ZPrerequisites.h"
#include "ZMeshView.h"
#include "MUID.h"
#include "MMatchItem.h"

#include "RCharCloth.h"

class ZItemSlot{
public:
	unsigned long int	m_nItemID;
	MRECT				m_Rect;
};

struct ZCharacterViewInfo
{
	MUID		UID;
	MMatchSex	nSex;
	int			nHair;
	int			nFace;
	int			nLevel;

	//jintriple3 �� �ٲٱ� �� ������..�޸� ���Ͻ÷�..
	MProtectValue<int>* m_pMnTeam;
//	MMatchTeam	nTeam;
	MMatchObjectStageState		nStageState;
//	bool		bReady;
	bool		bMaster;
	bool		bFireWall;
	bool		bNAT;
};

class ZCharacterView : public ZMeshView{
public:
	bool				m_bDrawInfo;
	ZCharacterViewInfo	m_Info;
	ZItemSlot			m_ItemSlots[MMCIP_END];
	ZMeshView*			m_pItemMeshView[MMCIP_END];
protected:
	bool				m_bVisibleEquipment;
	MMatchCharItemParts	m_nVisualWeaponParts;	// ���� �ִ� ������ �����ִ� ���� ����

	bool				m_bAutoRotate;
	DWORD				m_dwTime;				// ȸ�� �ð�
	
protected:
	void		 RepositionItemSlots(void);

	virtual void OnSize(int w, int h);
	virtual void OnDraw(MDrawContext* pDC);
	virtual bool IsDropable(MWidget* pSender);
	virtual bool OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);
public:
	ZCharacterView(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~ZCharacterView(void);

	void SetDrawInfo(bool bVal)	{ m_bDrawInfo = bVal; }
	bool GetDrawInfo()			{ return m_bDrawInfo; }

	void InitCharParts(ZCharacterView* pCharView, MMatchCharItemParts nVisualWeaponParts = MMCIP_PRIMARY);
	void InitCharParts(MMatchSex nSex, unsigned int nHair, unsigned int nFace, 
				       const unsigned long int* nEquipItemIDs, MMatchCharItemParts nVisualWeaponParts = MMCIP_PRIMARY);
	void SetParts(MMatchCharItemParts nParts, unsigned int nItemID);
	void ChangeVisualWeaponParts(MMatchCharItemParts nVisualWeaponParts);

	void SetVisibleEquipment(bool bVisible)		{ m_bVisibleEquipment = bVisible; }
	void SetSelectMyCharacter();

	void EnableAutoRotate( bool bAutoRotate)	{ m_bAutoRotate = bAutoRotate; }
	bool IsAutoRotate()							{ return m_bAutoRotate; }

	virtual void OnInvalidate();
	virtual void OnRestore();

public:
	#define MINT_CHARACTERVIEW	"CharacterView"
	virtual const char* GetClassName(void){ return MINT_CHARACTERVIEW; }
	
	void SetCharacter( MUID uid );
	const MUID& GetCharacter() { return m_Info.UID; }
};

unsigned long int GetVisualWeaponID(unsigned long int nMeleeItemID, unsigned long int nPrimaryItemID,
									unsigned long int nSecondaryItemID, unsigned long int nCustom1ItemID,
									unsigned long int nCustom2ItemID);



#endif