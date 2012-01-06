#pragma once
#include "SkeletonNode.h"

class C3DMapEffect 	:	public CSkeletonNode 
{
public:
	static bool s_bStroke;
protected:
	bool			m_bDelSelf;
	bool			m_bDie;
	int				m_nDelay;
	bool			m_bSave;
	char			m_szIndex[64];
	float			m_fHorizontal;
	float			m_fVertical;
	bool			m_bOver;
	bool			m_bSimpleObj;
	////C3DSimpleObj	m_objSimple;
	unsigned long			m_dwFrameIndex;

public:
	C3DMapEffect();
	virtual ~C3DMapEffect();

public:
	static  C3DMapEffect* CreateNew(const Vec3D& vWorldPos, char* pszIndex, bool bDelSelf = true, bool bSave = false);
	bool Create(const Vec3D& vWorldPos, char* pszIndex, bool bDelSelf = true, bool bSave = false);

public:
	void SetRotate(float fHorizontal, float fVertical);
	virtual void Show(void* pInfo);
	//virtual int getObjType(){return MAP_3DEFFECT;}
	virtual void Die(){m_bDie = true;}
public:
	bool	NeedSave(){return m_bSave;}
	void	LoadDataObj(FILE* fp);
	void	LoadTextObj(FILE* fp);
	void	SetDir(int nDir);
public:
	bool	IsOver(){return m_bOver;}
};