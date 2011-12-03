#ifndef _ZCHARACTERITEM_H
#define _ZCHARACTERITEM_H

#include "MMatchItem.h"
#include "ZItem.h"
#include "ZFile.h"
#include <list>
#include <algorithm>
using namespace std;


/// ĳ���Ͱ� ����ϰ� �ִ� �����۵�
class ZCharacterItem
{
private:
protected:
	ZItem					m_Items[MMCIP_END];
	MMatchCharItemParts		m_nSelectedWeapon;		// ������ Item�� ���� m_Items�� �ش��ϴ� �ε���
	bool Confirm(MMatchCharItemParts parts, MMatchItemDesc* pDesc);
	bool IsWeaponItem(MMatchCharItemParts parts);
public:
	ZCharacterItem();
	virtual ~ZCharacterItem();
	void SelectWeapon(MMatchCharItemParts parts);
	bool EquipItem(MMatchCharItemParts parts, int nItemDescID, int nItemCount = 1);

	bool Reload();

	ZItem* GetItem(MMatchCharItemParts parts)
	{
		if ((parts < MMCIP_HEAD) || (parts >= MMCIP_END))
		{
			_ASSERT(0);
			return NULL;
		}
		return &m_Items[(int)parts]; 
	}
	ZItem* GetSelectedWeapon(); 
	MMatchCharItemParts GetSelectedWeaponParts() { return (MMatchCharItemParts)m_nSelectedWeapon; }

	MMatchCharItemParts GetSelectedWeaponType() {
		return m_nSelectedWeapon;
	}

	bool Save(ZFile *file);
	bool Load(ZFile *file, int nReplayVersion);

	void ShiftFugitiveValues();
	
	/*
	void SetWarppingItemDesc(DWORD tick)
	{ 
		for(int i = 0; i < (int)MMCIP_END; ++i)
		{
			MMatchItemDesc* pDesc = ((MMatchItem)m_Items[i]).GetDesc();
			if(pDesc)
				pDesc->m_pMItemName->SetWarpingAdd(tick);
		}
	}
	*/
};

#endif