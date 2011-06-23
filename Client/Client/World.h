//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* World.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "Scene.h"
#include "TSingleton.h"
#include "Role.h"
#include "RPGSkyTextRender.h"
#include "Messages.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define CLASS_TEMP_SIZE		200
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
	CTerrain						m_Terrain;
	// ----
	std::map<ULONG, CRole*>			m_mapRole;
	std::deque<DamageInfo>			m_dequeDamageInfo;
	// ----
	CDataPlugsMgr					m_DataPlugsMgr;
	// ----
	char							m_szTemp[CLASS_TEMP_SIZE];
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
	virtual void	OnFrameMove			(double fTime, float fElapsedTime);
	virtual void	OnFrameRender		(double fTime, float fElapsedTime);
	virtual void	GetRenderObject		(const CFrustum & frustum, DEQUE_MAPOBJ & ObjectList);
	// ----
	CTerrain&		getTerrain			(){ return m_Terrain; }
protected:
	void			loadMap				(const char* szFilename, ...);
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------