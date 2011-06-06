#pragma once
#include "iSkeleton.h"
#include <map>

class CBoneInfo:public iBoneInfo
{
public:
	CBoneInfo():
	m_uParent(0xFF),
	m_billboard(false)
	{
		m_mInvLocal.unit();
	}
	virtual void			setName(const char* szName){m_strName = szName;}
	virtual void			setParent(unsigned char uParent){m_uParent = uParent;}
	virtual void			setInvLocal(const Matrix& mInvLocal){m_mInvLocal = mInvLocal;}
	virtual const char*		getName()const{return m_strName.c_str();}
	virtual unsigned char	getParent()const{return m_uParent;}
	virtual const Matrix&	getInvLocal()const{return m_mInvLocal;}
public:
	std::string		m_strName;
	unsigned char	m_uParent;
	Matrix			m_mInvLocal;
	Vec3D			m_pivot;
	bool			m_billboard;
};

class SkeletonAnim:public iSkeletonAnim
{
public:
	virtual void			setName(const char* szName){m_strName = szName;}
	virtual void			setSpeed(float fSpeed){m_fSpeed = fSpeed;}
	virtual void			setTotalFrames(unsigned int uTotalFrames){m_uTotalFrames = uTotalFrames;}
	virtual const char*		getName()const{return m_strName.c_str();}
	virtual float			getSpeed()const{return m_fSpeed;}
	virtual unsigned int	getTotalFrames()const{return m_uTotalFrames;}
public:
	std::string		m_strName;
	float			m_fSpeed;
	unsigned int	m_uTotalFrames;
};

class CSkeletonData:public iSkeletonData
{
public:
	virtual unsigned char	getBoneCount();
	virtual iBoneInfo*		allotBoneInfo();
	virtual iBoneInfo*		getBoneInfo(unsigned char uBoneID);
	virtual int				getBoneIDByName(const char* szName);
	virtual size_t			getAnimationCount();
	virtual iSkeletonAnim*	allotAnimation(const std::string& strName);
	virtual iSkeletonAnim*	getAnimation(const std::string& strName);
	virtual iSkeletonAnim*	getAnimation(size_t index);
	virtual bool			delAnimation(const std::string& strName);
	void					calcBonesTree(int nBoneID,std::vector<Matrix>& setBonesMatrix,std::vector<bool>& setCalc)const;
	void					CalcBonesMatrix(const std::string& strAnim, int time, std::vector<Matrix>& setBonesMatrix);	// ¼ÆËã¶¯»­Ö¡¾ØÕó
public:
	std::vector<CBoneInfo>				m_Bones;	// ¹Ç÷À
	std::map<std::string,SkeletonAnim>	m_Anims;	// ¶¯»­
};