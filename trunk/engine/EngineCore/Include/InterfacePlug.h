#pragma once
#include "iSkeleton.h"
#include "iMesh.h"
#include "iParticle.h"
#include "Material.h"

class iRenderNode
{
public:
	enum{NODE_BASE,NODE_PARTICLE,NODE_SKELETON,NODE_MESH,NODE_SKINE};
	virtual	int				getType				() = 0;
	virtual void			frameMove			(const Matrix& mWorld, double fTime, float fElapsedTime)=0;
	virtual void			render				(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const=0;
	virtual void			setPos				(const Vec3D& vPos)=0;
	virtual void			setRotate			(const Vec3D& vRotate)=0;
	virtual void			setScale			(const Vec3D& vScale)=0;
	virtual void			setName				(const char* szName)=0;
	virtual void			setFilename			(const char* szName)=0;
	virtual void			setLocalBBox		(const BBox& LocalBBox)=0;
	virtual void			setWorldBBox		(const BBox& WorldBBox)=0;
	virtual void			setWorldMatrix		(const Matrix& mWorldMatrix)=0;
	virtual void			setParent			(iRenderNode* pParent)=0;
	virtual void			setData				(void* pData)=0;
	virtual const Vec3D&	getPos				()const=0;
	virtual const Vec3D&	getRotate			()const=0;
	virtual const Vec3D&	getScale			()const=0;
	virtual const BBox&		getLocalBBox		()const=0;
	virtual const BBox&		getWorldBBox		()const=0;
	virtual const Matrix&	getWorldMatrix		()const=0;
	virtual iRenderNode*	getParent			()=0;
	virtual void*			getData				()=0;
	virtual const char*		getName				()const=0;
	virtual const char*		getFilename			()const=0;
	virtual iRenderNode*	getChild			(const char* szName)=0;
	virtual void			addChild			(iRenderNode* pChild)=0;
	virtual bool			load				(const char* szFilename)=0;
	virtual void			updateWorldBBox		()=0;
	virtual void			updateWorldMatrix	()=0;
};

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

class CDataPlugBase
{
public:
	CDataPlugBase(){};
	virtual ~CDataPlugBase(){};
	virtual const char*		getTitle() = 0;
	virtual const char*		getFormat() = 0;
	virtual void			release() = 0;
	virtual void			setRenderNodeMgr(iRenderNodeMgr* pRenderNodeMgr){m_pRenderNodeMgr = pRenderNodeMgr;}
	virtual bool			importData(iRenderNode* pRenderNode, const char* szFilename)=0;
#ifdef _DEBUG
	virtual bool			isDebug(){return true;}
#else
	virtual bool			isDebug(){return false;}
#endif
protected:
	iRenderNodeMgr* m_pRenderNodeMgr;
};