#include "world.h"
#include "PlayerMe.h"
#include "Audio.h"
#include "RPGSkyUIGraph.h"
#include "RenderSystem.h"
#include "float.h"
#include "intersect.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// # (leo123) : i think here too need set patches in define
// ... : void CWorld::create(UCHAR uMapID) : must take count of World% in data and dont use this kinda of switch
// ... : Why double { in renderDamageInfo?

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CWorld::CWorld()
{
	m_CurMap = 0xFFFFFFFF;
	// ----
	// # Change the fog color.
	// ----
	Fog fog		= getFog();
	// ----
	fog.color	= 0x00000000;
	// ----
	setFog(fog);
	// ----
	//this->SetTerrain(& m_Terrain);
	// ----
	m_DamageTextRender.load("Data\\Fonts\\fzl2jw.ttf");
	m_DamageTextRender.setFontSize(18);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CWorld::~CWorld()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CRole * CWorld::getFocusRole()
{
	iRenderNode* pObj = NULL;
	// ----
	if(getFocusNodes().getChildObj().size() == 0)
	{
		// ----
		// # No focus objects.
		// ----
		pObj = NULL;
	}
	else
	{
		pObj = *getFocusNodes().getChildObj().begin();
		// ----
		if(pObj != NULL) 
		{
			if(pObj->getType() != MAP_ROLE)
			{
				// ----
				// # its not a role.
				// ----
				pObj = NULL;
			}
		}
		// ----
		// # Point of object is null.
	}
	// ----
	return (CRole*)pObj;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CRole* CWorld::getRole(ULONG uID)
{
	auto it = m_mapRole.find(uID);
	// ----
	if(it != m_mapRole.end())
	{
		return it->second;
	}
	// ----
	return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CRole* CWorld::pickRole(const Vec3D & vRayPos , const Vec3D & vRayDir)
{
	CRole * pRole	= NULL;
	// ----
	float fFocusMin = FLT_MAX;
	float fMin		= 0.00;
	float fMax		= 0.00;
	// ----
	FOR_IN(it,m_mapRole)
	{
// 		if((*it)->getType() != MAP_ROLE)
// 		{
// 			continue;
// 		}
		// ----
		if(it->second->intersect(vRayPos , vRayDir, fMin, fMax) != NULL)
		{
			if(fFocusMin > fMin)
			{
				pRole		= it->second;
				// ----
				fFocusMin	= fMax;
			}
		}
	}
	// ----
	return pRole;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CWorld::addRole(CRole * pRole)
{
	bool bReturn = false;
	// ----
	if(pRole != false)
	{
		if(m_mapRole.find(pRole->getID()) == m_mapRole.end())
		{
			m_mapRole[pRole->getID()] = pRole;
			// ----
			bReturn = true;
		}
	}
	// ----
	return bReturn;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CWorld::delRole(ULONG uID)
{
	bool bReturn = false;
	// ----
	if(CPlayerMe::getInstance().getID() != uID)
	{
		auto it = m_mapRole.find(uID);
		// ----
		if(it != m_mapRole.end())
		{
			removeRenderNode(it->second);
			// ----
			m_mapRole.erase(it);
			// ----
			bReturn = true;
		}
	}
	// ----
	return bReturn;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CWorld::create(UCHAR uMapID)
{
	if(uMapID != m_CurMap)
	{
		m_CurMap	= uMapID;
		// ----
		char szMapFilname[256];
		sprintf(szMapFilname,"Data\\World%d\\EncTerrain%d.obj", m_CurMap+1, m_CurMap+1);
		load(szMapFilname);
		setup();
		// ----
		//GetAudio().LoadMusic("Data\\Music\\main_theme.mp3");
		//GetAudio().PlayMusic(true,20);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CWorld::updateDamageInfo(double fTime, float fElapsedTime)
{
	for(int i = m_dequeDamageInfo.size() -1 ; i >= 0 ; i--)
	{
		DamageInfo& damageInfo	=  m_dequeDamageInfo[i];
		damageInfo.vPos.y		+= fElapsedTime * 2.0f;
		// ----
		if(damageInfo.vPos.y > 6.0f)
		{
			m_dequeDamageInfo.erase(m_dequeDamageInfo.begin() + i);
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CWorld::renderDamageInfo()const
{
	Node3DUIGraph::getInstance().initDrawText();
	// ----
	for (auto it=m_mapRole.begin();it!=m_mapRole.end();it++)
	{
		it->second->drawName();
	}
	// ----
	FOR_IN(it,m_dequeDamageInfo)
	{
		Pos2D pos(0,0);
		// ----
		GetRenderSystem().world2Screen(it->vPos, pos);
		// ----
		RECT rc = {pos.x - 40, pos.y - 30, pos.x + 40, pos.y}; 
		// ----
		Matrix mWorld;
		GetRenderSystem().getWorldMatrix(mWorld);
		Matrix mNewWorld = mNewWorld*Matrix::newTranslation(Vec3D(pos.x, pos.y, 0.0f));
		GetRenderSystem().setWorldMatrix(mNewWorld);
		// ----
		m_DamageTextRender.drawUBB(&it->ubb);
		// ----
		GetRenderSystem().setWorldMatrix(mWorld);
		// ----
		//RPGSkyUIGraph::getInstance().drawText(it->wcsInfo.c_str(), it->wcsInfo.length() ,rc, ALIGN_TYPE_CENTER);
		//m_DamageTextRender.drawText(it->wcsInfo.c_str(),rc, it->wcsInfo.length(), (UINT)ALIGN_TYPE_CENTER);
	}
	// ---- text demo for leo :)
 	for (size_t i = 0; i < 10; ++i)
 	{
 		//m_DamageTextRender.drawText(L"UIGraph::getInstance().getTextRende",10,10+i*20,0xFFFF3333+0x1400*i);
 	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CWorld::addDamageInfo(Vec3D vPos,const std::wstring & wcsInfo)
{
	DamageInfo damageInfo;
	damageInfo.vPos = vPos;
	damageInfo.wcsInfo = wcsInfo;
	RECT rc = {- 40, - 30, 40, 0}; 
	m_DamageTextRender.buildUBB(&damageInfo.ubb,wcsInfo.c_str(),rc,wcsInfo.length(),ALIGN_TYPE_CENTER);
	// ----
	m_dequeDamageInfo.push_back(damageInfo);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CWorld::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	// ----
	FOR_IN(it,m_RenderNodes)
	{
		if((*it)->getType())
		{
			(*it)->frameMove(Matrix::UNIT, fTime, fElapsedTime);
		}
	}
	// ----
	FOR_IN(it,m_mapRole)
	{
		it->second->frameMoveRole(Matrix::UNIT, fTime, fElapsedTime);
	}
	// ----
	// # Delete the old roles
	// ----
	for(auto it = m_mapRole.begin() ; it != m_mapRole.end() ; it++)
	{
		if((it->second->getActionState() == CRole::DEATH) ||
			((CPlayerMe::getInstance().getPos()-it->second->getPos()).length() > PLAYER_VIEW_RANGE))
		{
			CWorld::getInstance().delRole(it->second->getID());
			// ----
			break;
		}
	}
	// ----
	// # Create the light objects
	// ----
	m_setLightObj.clear();
	// ----
	FOR_IN(it,m_RenderNodes)
	{
		//if(((C3DMapObj*)(*it))->m_setParticleGroup.size() > 0)
		//{
		//	if(rand() % 2)
		//	{
		//		m_setLightObj.push_back(*it);
		//	}
		//}
	}
	// ----
	m_DamageTextRender.OnFrameMove();
	updateDamageInfo(fTime, fElapsedTime);
	// ----
	m_Messages.frameMove(fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CWorld::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	CScene::render(Matrix::UNIT, MATERIAL_NORMAL);
	// ----
	renderDamageInfo();
	//m_Messages.frameRender();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CWorld::getRenderNodes(const CFrustum& frustum, std::set<iRenderNode*>& setNode)
{
	CrossRet crossRet;
	// ----
	for(auto it = m_mapRole.begin() ; it != m_mapRole.end() ; it++)
	{
		crossRet = frustum.CheckAABBVisible(it->second->getWorldBBox());
		// ----
		if(cross_exclude != crossRet)
		{
			setNode.insert(it->second);
		}
	}
	// ----
	CScene::getRenderNodes(frustum, setNode);
	// ----
	if (CWorld::getInstance().getSceneData())
	{
		Pos2D pos;
		CPlayerMe::getInstance().GetCellPos(pos);
		unsigned char uTileID = CWorld::getInstance().getSceneData()->getCellTileID(pos.x, pos.y ,0);
		// ----
		// # hide the house wall
		if (uTileID==4)
		{
			for(auto it = setNode.begin(); it!=setNode.end();)
			{
				auto temp = it++;
				if (std::string((*temp)->getFilename()).find("HouseWall05.bmd") != std::string::npos||
					std::string((*temp)->getFilename()).find("HouseWall06.bmd") != std::string::npos)
					//if (strcmp((*it)->getFilename(),"125")==0 || strcmp((*it)->getName(),"126")==0)
				{
					m_RenderNodes.erase(temp);
				}           
			}

			FOR_IN(it,setNode)
			{

			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------