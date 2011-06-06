#pragma once
#include "Animated.h"
#include "Matrix.h"

struct iBoneInfo
{
public:
	virtual void			setName(const char* szName)=0;
	virtual void			setParent(unsigned char uParent)=0;
	virtual void			setInvLocal(const Matrix& mInvLocal)=0;
	virtual const char*		getName()const=0;
	virtual unsigned char	getParent()const=0;
	virtual const Matrix&	getInvLocal()const=0;
};

struct BoneAnim
{
	Animated<Vec3D>			trans;
	Animated<Quaternion>	rot;
	Animated<Vec3D>			scale;
	void transform(Matrix& m, unsigned int time)const
	{
		if (trans.isUsed())
		{
			Vec3D tr = trans.getValue(time);
			m *= Matrix::newTranslation(tr);
		}
		if (rot.isUsed())
		{
			Quaternion q = rot.getValue(time);
			m *= Matrix::newQuatRotate(q);
		}
		if (scale.isUsed())
		{
			Vec3D sc = scale.getValue(time);
			m *= Matrix::newScale(sc);
		}
	}
};

class iSkeletonAnim
{
public:
	virtual void			setName(const char* szName)=0;
	virtual void			setSpeed(float fSpeed)=0;
	virtual void			setTotalFrames(unsigned int uTotalFrames)=0;
	virtual const char*		getName()const=0;
	virtual float			getSpeed()const=0;
	virtual unsigned int	getTotalFrames()const=0;
public:
	std::vector<BoneAnim> setBonesAnim;
};

class iSkeletonData
{
public:
	virtual unsigned char	getBoneCount()=0;
	virtual iBoneInfo*		allotBoneInfo()=0;
	virtual iBoneInfo*		getBoneInfo(unsigned char uBoneID)=0;
	virtual int				getBoneIDByName(const char* szName)=0;
	virtual size_t			getAnimationCount()=0;
	virtual iSkeletonAnim*	allotAnimation(const std::string& strName)=0;
	virtual iSkeletonAnim*	getAnimation(const std::string& strName)=0;
	virtual iSkeletonAnim*	getAnimation(size_t index)=0;
	virtual bool			delAnimation(const std::string& strName)=0;
};