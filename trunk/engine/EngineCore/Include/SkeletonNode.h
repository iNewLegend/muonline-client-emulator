#pragma once
#include "RenderNode.h"
#include "AnimMgr.h"

class CTextRender;
class CSkeletonData;
class CSkeletonNode:public CRenderNode
{
public:
	CSkeletonNode();
	~CSkeletonNode();
public:
	virtual int			getType			() {return NODE_SKELETON;}
	virtual void		frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void		render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	CSkeletonData*		getSkeletonData	(){return m_pSkeletonData;}
	virtual bool		setup			();
	void				setAnim			(int nID);
	void				setAnimByName	(const char* szAnimName);
	virtual void		animate			(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void		drawSkeleton	(CTextRender* pTextRender)const;
	std::vector<Matrix>&getBonesMatrix	(){return m_setBonesMatrix;}
protected:
	SingleAnimNode		m_AnimMgr;			// 动作管理器
	float				m_fAnimRate;
	std::vector<Matrix>	m_setBonesMatrix;	// 骨骼矩阵
	std::string			m_strAnimName;		// Current Name Of Animate
	int					m_nAnimTime;		// 动作时间帧
	CSkeletonData*		m_pSkeletonData;	// 
};