#pragma once
#include "MapObj.h"

class CSound;
class CMapSound : public CMapObj
{
public:
	CMapSound();
	virtual ~CMapSound();
private:
	bool	m_bFocus;
	CSound* m_pSound;
	// info
	int		m_nRange;
	int		m_nVolume;
	std::string	m_strFilename;
	unsigned long	m_dwInterval;
	bool	m_bWait;
	unsigned long	m_dwWaitPoint;
	bool	m_bActive;
public:
	static bool s_bStroke;
public:
	virtual void render()const;
	virtual int  GetObjType(){return MAP_SOUND;}
	virtual bool IsFocus();
	virtual void setPos(const Vec3D& vPos);

	virtual bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir,Vec3D& vCoord);
	void   Play();

	bool   Create(const Vec3D& vPos, int nRange, int nVolume, char* pszFile, unsigned long dwInterval);
	static CMapSound* CreateNew(const Vec3D& vPos, int nRange, int nVolume, char* pszFile, unsigned long dwInterval);
};