#ifndef ZEFFECTBILLBOARD_H
#define ZEFFECTBILLBOARD_H

#include "ZEffectManager.h"
#include <d3dx9math.h>
#include <d3d9.h>
#include "RBaseTexture.h"

struct CUSTOMVERTEX{
	FLOAT	x, y, z;
	DWORD	color;
	FLOAT	tu, tv;
};

class ZEffectBillboardSource {
protected:
	RBaseTexture*	m_pTex;
//	LPDIRECT3DVERTEXBUFFER9	m_pVB;
//	CUSTOMVERTEX	m_pVertices[4];
public:
	ZEffectBillboardSource(const char* szTextureFileName);
	virtual ~ZEffectBillboardSource(void);

	bool Draw(rvector &Pos, rvector &Dir, rvector &Up, rvector &Scale, float fOpacity);
};

class ZEffectBillboard : public ZEffect{
protected:
	ZEffectBillboardSource*	m_pEffectBillboardSource;
public:
	rvector			m_Pos;
	rvector			m_Normal;
	rvector			m_Up;
	rvector			m_Scale;
	float			m_fOpacity;


public:
	ZEffectBillboard(ZEffectBillboardSource* pEffectBillboardSource);
	virtual ~ZEffectBillboard(void);

	void SetSource(ZEffectBillboardSource*	pSource) {
		m_pEffectBillboardSource = pSource;
	}

	virtual bool Draw(unsigned long int nTime);

	virtual rvector GetSortPos() {
		return m_Pos;
	}

};

// �ؽ��� �ڵ��� �����ִ� �ܼ��� Drawer
class ZEffectBillboardDrawer{
protected:
//	LPDIRECT3DVERTEXBUFFER9	m_pVB;
//	CUSTOMVERTEX	m_pVertices[4];
	bool			m_bCreate;
public:
	ZEffectBillboardDrawer(void);
	virtual ~ZEffectBillboardDrawer(void);

	void Create(void);
	bool IsCreated(void){ return m_bCreate; }

	bool Draw(LPDIRECT3DTEXTURE9 pEffectBillboardTexture, rvector &Pos, rvector &Dir, rvector &Up, rvector &Scale, float fOpacity);
};

// �ؽ��� �ڵ�(LPDIRECT3DTEXTURE9)�� �̿��ϴ� Billboard
class ZEffectBillboard2 : public ZEffect{
protected:
	LPDIRECT3DTEXTURE9	m_pEffectBillboardTexture;
	static ZEffectBillboardDrawer	m_EffectBillboardDrawer;	// �׸��� ���� ��ü�̹Ƿ� �ϳ��� �����ǰ�, ���μ��� ������ �Ҹ�ȴ�.
public:
	rvector			m_Pos;
	rvector			m_Normal;
	rvector			m_Up;
	rvector			m_Scale;
	float			m_fOpacity;
public:
	ZEffectBillboard2(LPDIRECT3DTEXTURE9 pEffectBillboardTexture);
	virtual ~ZEffectBillboard2(void);

	virtual bool Draw(unsigned long int nTime);

	virtual rvector GetSortPos() {
		return m_Pos;
	}

};

#endif