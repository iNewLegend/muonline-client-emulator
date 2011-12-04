#include "MapSound.h"
#include "Audio.h"
//#include "SceneMgr.h"
//#include "Hero.h"

bool CMapSound::s_bStroke = false;

CMapSound::CMapSound()
{
	m_nType = MAP_SOUND;
	m_pSound = NULL;
	m_dwInterval = 0;
	m_bActive = false;
}
//-------------------------------------------------------------
CMapSound::~CMapSound()
{
	m_bFocus = false;
//	if(m_pSound)
//		DXClose2DSound(m_pSound);
}
//-------------------------------------------------------------

void CMapSound::render()const
{
#ifdef _ARHUN_DEBUG
	if(!s_bStroke)
		return;
	static size_t s_uTexSund = 0;
	if(!s_uTexSund)
	{
		s_uTexSund = CRenderSystem::getSingleton().GetTextureMgr().RegisterTexture("sound.tga");
	}
	Pos2D posScreen;
	GetSceneMgr().World2Screen( m_vPos, posScreen );
	GetGraphics().DrawTex(posScreen.x,posScreen.y,s_uTexSund,0xFFFFFFFF);
#endif
}
//-------------------------------------------------------------
//-------------------------------------------------------------
bool CMapSound::IsFocus()
{
	return m_bFocus;
}

//-------------------------------------------------------------
// void CMapSound::setPos(const Vec3D& vPos)
// {
// //	if(m_pSound)
// //		DXSet2DSoundPos(m_pSound, vPos.x, vPos.z);
// }

bool CMapSound::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir,Vec3D& vCoord)
{
	return false;
}

bool CMapSound::Create(const Vec3D& vPos, int nRange, int nVolume, char* pszFile, unsigned long dwInterval)
{
	if(!pszFile)
		return false;

	this->setPos( vPos );
	m_nRange = nRange;
	m_nVolume = nVolume;
	m_strFilename = pszFile;
	m_dwInterval = dwInterval; 
//	GetAudio().playSound(m_szFile);
//	m_pSound = (CSound*)GetAudio().Play3DSound(m_szFile, 0, 0,
//											m_nRange, -1, m_nVolume);
	return true;
}
//-------------------------------------------------------------
CMapSound* CMapSound::CreateNew(const Vec3D& vPos, int nRange, int nVolume, char* pszFile, unsigned long dwInterval)
{
	CMapSound* pSound = new CMapSound;
	// assert(pSound);
	if(!pSound->Create( vPos, nRange, nVolume, pszFile, dwInterval ))
	{
		delete pSound;
		return NULL;
	}
	return pSound;
}
//-------------------------------------------------------------
void CMapSound::Play()
{
//	if(m_pSound && Check2DSound(m_pSound))
//		DXClose2DSound(m_pSound);
	if(m_dwInterval == 0)
		//m_pSound = (CSound*)
		GetAudio().Play3DSound(m_strFilename.c_str(), m_vPos.x, m_vPos.z,
											m_nRange, m_nVolume);
	else
		//m_pSound = (CSound*)
		GetAudio().Play3DSound(m_strFilename.c_str(), m_vPos.x, m_vPos.z,
											m_nRange, m_nVolume);
}
//-------------------------------------------------------------
