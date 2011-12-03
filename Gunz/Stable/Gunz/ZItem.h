#ifndef _ZITEM_H
#define _ZITEM_H

#include "MMatchItem.h"


// �̰� ���߿� MBaseItem�� �θ�� �ϵ��� �ٲ��� �Ѵ�.
class ZItem : public MMatchItem
{
private:
protected:
	// weapon���� ����ϴ� ������
	MProtectValue<int>	m_nBulletSpare;			// ������ ź�˼�(���� ������ ź���� ����)
	MProtectValue<int>	m_nBulletCurrMagazine;	// ���� źâ�� ����ִ� ź�˼�
public:
	ZItem();
	virtual ~ZItem();

	// weapon������ ���̴� �Լ�
	void InitBullet(int nBulletSpare, int nBulletCurrMagazine);
	bool Shot();
	bool Reload();
	bool isReloadable();

	int GetBulletSpare()				{ return m_nBulletSpare.Ref(); }
	void SetBulletSpare(int nBullet) { m_nBulletSpare.Set_CheckCrc(nBullet); }
	int GetBulletCurrMagazine()	{ return m_nBulletCurrMagazine.Ref(); }
	void SetBulletCurrMagazine(int nBulletPerMagazine)	{ m_nBulletCurrMagazine.Set_CheckCrc(nBulletPerMagazine); }

	static float GetPiercingRatio(MMatchWeaponType wtype,RMeshPartsType partstype);
	float GetKnockbackForce();

	void ShiftFugitiveValues();
};


#endif