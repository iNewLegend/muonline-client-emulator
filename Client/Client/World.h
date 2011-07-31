//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* World.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "Scene.h"
#include "TSingleton.h"
#include "Role.h"
#include "RPGSkyTextRender.h"
#include "Messages.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PLAYER_VIEW_RANGE	16
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct DamageInfo 
{
	Vec3D			vPos;
	std::wstring	wcsInfo;
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
	CRPGSkyTextRender				m_DamageTextRender;	
public:
	CMessages						m_Messages;
	// ----
	CWorld();
	~CWorld();
	// ----
	CRole*			getFocusRole		();
	CRole*			getRole				(ULONG uID);
	CRole*			pickRole			(const Vec3D & vRayPos , const Vec3D & vRayDir);
	// ----
	bool			addRole				(CRole* pRole);
	bool			delRole				(ULONG uID);
	// ----
	void			create				(UCHAR uMapID);
	// ----
	void			renderDamageInfo	(double fTime, float fElapsedTime);
	void			addDamageInfo		(Vec3D vPos, const std::wstring& wcsInfo);
	// ----
	virtual void	frameMove			(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render				(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	virtual void	getRenderNodes		(const CFrustum& frustum, std::set<iRenderNode*>& setNode);
	// ----
protected:
	void			loadMap				(const char* szFilename, ...);
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------