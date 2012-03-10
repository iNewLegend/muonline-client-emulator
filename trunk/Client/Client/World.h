//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* World.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "Scene.h"
#include "TSingleton.h"
#include "Role.h"
#include "RPGSkyTextRender.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PLAYER_VIEW_RANGE	16
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct DamageInfo 
{
	Vec3D			vPos;
	std::wstring	wcsInfo;
	CUBB			ubb;
	float			fTime;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CWorld:public CScene, public TSingleton<CWorld>
{
protected:
	// ----
	std::map<ULONG, CRole*>			m_mapRole;
	std::deque<DamageInfo>			m_dequeDamageInfo;
	// ----
	UINT							m_CurMap;
	// ----
	CNode3DTextRender				m_DamageTextRender;
public:
	CWorld();
	~CWorld();
	// ----
	CRole*			getFocusRole		();
	CRole*			getRole				(ULONG uID);
	CRole*			pickRole			(const Vec3D & vRayPos , const Vec3D & vRayDir);
	iRenderNode*	pickProps			(const Vec3D & vRayPos , const Vec3D & vRayDir);
	// ----
	bool			addRole				(CRole* pRole);
	bool			delRole				(ULONG uID);
	// ----
	void			create				(UCHAR uMapID);
	// ----
	void			addDamageInfo		(Vec3D vPos, const std::wstring& wcsInfo);
	// ----
	virtual void	updateRender		(const CFrustum& frustum);
	virtual void	frameMoveRole		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	frameMove			(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render				(int nRenderType)const;
	// ----
	void			renderDamageInfo	()const;
protected:
	// ----
	void			updateDamageInfo	(double fTime, float fElapsedTime);
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------