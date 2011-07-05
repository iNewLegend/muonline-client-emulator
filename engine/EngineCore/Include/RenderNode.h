#pragma once
#include "Material.h"
#include "Matrix.h"
#include "frustum.h"
#include <string>
#include <list>
#include "common.h"
#include "InterfaceModel.h"

class CRenderNode;
typedef std::list<CRenderNode*>		LIST_RENDER_NODE;
//----

class CRenderNode:public iRenderNode
{
public:
public:
	CRenderNode();
	~CRenderNode();
public:
	//----
	virtual GSET_CONST_VAR	(Vec3D&,		m_v,Pos);
	virtual GSET_CONST_VAR	(Vec3D&,		m_v,Rotate);
	virtual GSET_CONST_VAR	(Vec3D&,		m_v,Scale);
	GSET_CONST_VAR			(BBox&,			m_,LocalBBox);
	GSET_CONST_VAR			(BBox&,			m_,WorldBBox);
	GSET_CONST_VAR			(Matrix&,		m_m,WorldMatrix);
	//----
	void					updateWorldBBox		();
	void					updateWorldMatrix	();
public:
	GSET_VAR				(CRenderNode*,	m_p,Parent);
	//----
	GET_VARIABLE			(LIST_RENDER_NODE&,ChildObj,m_mapChildObj);
	CONST_GET_VARIABLE		(LIST_RENDER_NODE&,ChildObj,m_mapChildObj);
	GSET_CONST_VAR			(BBox&,m_,BBox);
	//----
	GSET_STRING	(Name);
	GSET_STRING	(BindingBoneName);
	//----
	virtual	int				getType				() = 0;
	virtual void			frameMove			(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void			render				(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	iRenderNode*			getChild			(const char* szName);
	const iRenderNode*		getChild			(const char* szName)const;
	virtual void			addChild			(iRenderNode* pChild);
	virtual bool			removeChild			(iRenderNode* pChild);
	virtual bool			delChild			(iRenderNode* pChild);
	virtual bool			contain				(const CRenderNode* pChild)const;
	virtual void			clearChildren		();
	virtual void			removeChildren		();
	virtual bool			intersectSelf		(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)const{return false;}
	virtual CRenderNode*	intersect			(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax);
	virtual bool			init				(void* pData){return true;}
	//----
	void					setChildBindingBone	(const char* szName, const char* szBoneName);
protected:
	CRenderNode*			m_pParent;
	LIST_RENDER_NODE		m_mapChildObj;
	//----
	std::string				m_strName;
	std::string				m_strBindingBoneName;
	int						m_nBindingBoneID;
	BBox					m_BBox;
protected:
	Vec3D					m_vPos;
	Vec3D					m_vRotate;
	Vec3D					m_vScale;
	BBox					m_LocalBBox;
	BBox					m_WorldBBox;
	Matrix					m_mWorldMatrix;
};
