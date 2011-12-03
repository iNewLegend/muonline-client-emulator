#ifndef _ZWORLD_H
#define _ZWORLD_H

#include "ZWater.h"
#include "ZMapDesc.h"
#include "ZClothEmblem.h"

// ������ ���带 �̷�� �ִ� ���� ����� �ֵ��� �սô�. (Ư�� �׸��� �� ���õ� �͵�)
// ���� ���� ������ ZGame���� �ϰ�, ���带 �̷�� �ִ� �͵��̳�, �׸��� �� ���� ���� ���⼭ ������ ���ڽ��ϴ�.
// ���� ��ǥ�� ZWorld�� ���� �� �� �־ ��𼭵��� �� �� �ֵ��� �ϴ°�. (ĳ���� ���� ȭ��, ���� ���)

// ZGame���� �ϳ����ϳ��� �ð��������� ����� �Űܳ����� �ϰڽ��ϴ�. - bird

class ZWorldManager;
class ZMapDesc;
class ZSkyBox;
class ZLoadingProgress;

class ZWorld
{
	friend ZWorldManager;

private:
	// ZGame���� ����� �̻�� ������ ��
	// 	RParticles			*m_pParticles;
	//	ZMapDesc			m_MapDesc;
	//	RBspObject			m_bsp;

	bool			m_bWaterMap;
	float			m_fWaterHeight;
	ZWaterList		m_waters;
	ZEmblemList		m_flags;
	ZSkyBox*		m_pSkyBox;

	bool	m_bFog; // ����
	DWORD	m_dwFogColor;
	float	m_fFogNear;
	float	m_fFogFar;

	char		m_szName[64];
	char		m_szBspName[_MAX_PATH];
	RBspObject	*m_pBsp;
	ZMapDesc	*m_pMapDesc;

	int		m_nRefCount;	// ���۷��� ī��Ʈ
	bool	m_bCreated;

	ZWorld();		// Ŭ������ ���� �������� ����. ZWorldManager�� ���ؼ� �����Ѵ�.
public:
	~ZWorld();

	void Update(float fDelta);
	void Draw();
	bool Create(ZLoadingProgress *pLoading );
	void Destroy();

	void OnInvalidate();
	void OnRestore();

	RBspObject	*GetBsp() { return m_pBsp; }
	ZMapDesc	*GetDesc() { return m_pMapDesc; }
	ZEmblemList	*GetFlags() { return &m_flags; }
	ZWaterList	*GetWaters() { return &m_waters; }

	void SetFog(bool bFog);
	bool IsWaterMap() { return m_bWaterMap; }
	float GetWaterHeight() { return m_fWaterHeight; }
	bool IsFogVisible()		{ return m_bFog; }
};

#endif