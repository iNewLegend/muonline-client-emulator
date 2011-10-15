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
	CSkeletonData*		getSkeletonData	(){return m_pSkeletonData;}
	virtual bool		setup			();
	void				setAnim			(int nID);
	void				setAnimByName	(const char* szAnimName);
	void				animate			(const char* szAnimName);
	void				CalcBones		(const char* szAnim, int time);
	virtual void		drawSkeleton	(CTextRender* pTextRender)const;
	std::vector<Matrix>&getBonesMatrix	(){return m_setBonesMatrix;}
protected:
	SingleAnimNode		m_AnimMgr;			// ����������
	std::vector<Matrix>	m_setBonesMatrix;	// ��������
	std::string			m_strAnimName;		// Current Name Of Animate
	int					m_nAnimTime;		// ����ʱ��֡
	CSkeletonData*		m_pSkeletonData;	// 
};