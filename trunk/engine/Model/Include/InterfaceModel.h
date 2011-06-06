#pragma once
#include "InterfaceDataPlugsBase.h"
#include "iSkeleton.h"
#include "iMesh.h"
#include "iParticle.h"

class iRenderNode
{
public:
	virtual void			setPos		(const Vec3D& vPos)=0;
	virtual void			setRotate	(const Vec3D& vRotate)=0;
	virtual void			setScale	(const Vec3D& vScale)=0;
	virtual const Vec3D&	getPos		()const=0;
	virtual const Vec3D&	getRotate	()const=0;
	virtual const Vec3D&	getScale	()const=0;
	virtual iRenderNode*	getChild	(const char* szName)=0;
	virtual void			addChild	(iRenderNode* pChild)=0;
	virtual bool			init		(void* pData)=0;
};

class CMaterial;
class iRenderNodeMgr
{
public:
	iRenderNodeMgr(){};
	virtual ~iRenderNodeMgr(){};

	virtual iRenderNode*	loadRenderNode(const char* szFilename)=0;
	virtual iRenderNode*	createRenderNode(const char* szClassName)=0;
	virtual iSkeletonData*	getSkeletonData(const char* szName)=0;
	virtual ParticleData*	getParticleData(const char* szName)=0;
	virtual iLodMesh*		getLodMesh(const char* szName)=0;
	virtual CMaterial*		getMaterial(const char* szName)=0;
	virtual iSkeletonData*	createSkeletonData(const char* szName)=0;
	virtual ParticleData*	createParticleData(const char* szName)=0;
	virtual iLodMesh*		createLodMesh(const char* szName)=0;
	virtual CMaterial*		createMaterial(const char* szName)=0;
};

//////////////////////////////////////////////////////////////////////////
class CModelPlugBase:public CDataPlugBase
{
public:
	CModelPlugBase(){};
	virtual ~CModelPlugBase(){};
	virtual iRenderNode* importData(iRenderNodeMgr* pRenderNodeMgr, const char* szFilename)=0;
};