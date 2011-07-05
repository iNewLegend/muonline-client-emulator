#pragma once
#include "InterfaceDataPlugsBase.h"
#include "iSkeleton.h"
#include "iMesh.h"
#include "iParticle.h"
#include "Material.h"

class iRenderNode
{
public:
	enum{NODE_BASE,NODE_PARTICLE,NODE_SKELETON,NODE_MODEL,NODE_SKINE};
	virtual	int				getType				() = 0;
	virtual void			frameMove	(const Matrix& mWorld, double fTime, float fElapsedTime)=0;
	virtual void			render		(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const=0;
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

	virtual bool			loadRenderNode(const char* szFilename, iRenderNode* pRenderNode)=0;
	virtual iRenderNode*	createRenderNode(const char* szClassName)=0;
	virtual void*			createRenderData(const char* szClassName)=0;
	virtual void*			getRenderData(const char* szClassName, const char* szName)=0;
	virtual void*			createRenderData(const char* szClassName, const char* szName)=0;
};

//////////////////////////////////////////////////////////////////////////
class CModelPlugBase:public CDataPlugBase
{
public:
	CModelPlugBase(){};
	virtual ~CModelPlugBase(){};
	virtual bool importData(iRenderNodeMgr* pRenderNodeMgr, iRenderNode* pRenderNode, const char* szFilename)=0;
};