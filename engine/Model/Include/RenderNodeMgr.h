#pragma once
#include "DataPlugsMgr.h"
#include "TSingleton.h"
#include "SkeletonNode.h"
#include "Particle.h"
#include "SkinModel.h"
#include "SkeletonData.h"
#include "LodMesh.h"

class CRenderNodeMgr:public iRenderNodeMgr, public TSingleton<CRenderNodeMgr>
{
public:
	CRenderNodeMgr();
	virtual ~CRenderNodeMgr(){};
	virtual iRenderNode*	loadRenderNode(const char* szFilename);
	virtual iRenderNode*	createRenderNode(const char* szClassName);
	virtual iSkeletonData*	getSkeletonData(const char* szName);
	virtual ParticleData*	getParticleData(const char* szName);
	virtual iLodMesh*		getLodMesh(const char* szName);
	virtual CMaterial*		getMaterial(const char* szName);
	virtual iSkeletonData*	createSkeletonData(const char* szName);
	virtual ParticleData*	createParticleData(const char* szName);
	virtual iLodMesh*		createLodMesh(const char* szName);
	virtual CMaterial*		createMaterial(const char* szName);
	CDataPlugsMgr&			getDataPlugsMgr(){return m_DataPlugsMgr;}
private:
	CDataPlugsMgr	m_DataPlugsMgr;
	std::map<std::string, CSkeletonData>		m_mapSkeletonData;
	std::map<std::string, ParticleData>			m_mapParticleData;
	std::map<std::string, CLodMesh>				m_mapLodMesh;
};
