#include "SkeletonData.h"
#include "RenderSystem.h"

void CSkeletonData::calcBonesTree(int nBoneID,std::vector<Matrix>& setBonesMatrix,std::vector<bool>& setCalc)const
{
	if (setCalc[nBoneID])
	{
		return;
	}
	setCalc[nBoneID] = true;

	// 找到父类的转换矩阵
	int nParent = m_Bones[nBoneID].m_uParent;
	if (nParent!=255)
	{
		calcBonesTree(nParent,setBonesMatrix,setCalc);
		setBonesMatrix[nBoneID] = setBonesMatrix[nParent] * setBonesMatrix[nBoneID];
	}
}

void CSkeletonData::CalcBonesMatrix(const std::string& strAnim, int time, std::vector<Matrix>& setBonesMatrix)
{
	// 默认的骨骼动画运算
	auto it = m_Anims.find(strAnim);
	if (it==m_Anims.end())
	{
		//return;
		for (size_t i=0;i<m_Bones.size();++i)
		{
			setBonesMatrix[i].unit();
		}
	}
	else
	{
		const std::vector<BoneAnim>& bonesAnim = it->second.setBonesAnim;
		for (size_t i=0;i<m_Bones.size();++i)
		{
			//bool tr = m_pBoneAnim->rot.isUsed() || m_pBoneAnim->scale.isUsed() || m_pBoneAnim->trans.isUsed() || m_pBoneAnim->billboard;
			//if (tr)
			//m_mat.translation(boneInfo.pivot);
			setBonesMatrix[i].unit();
			bonesAnim[i].transform(setBonesMatrix[i],time);
		}
	}

	std::vector<bool> setCalc(m_Bones.size(),false);// 重置所有骨骼为'false',说明骨骼的动画还没计算过！
	
	for (size_t i=0;i<m_Bones.size();++i)
	{
		calcBonesTree(i,setBonesMatrix,setCalc);
	}
	for (size_t i=0;i<m_Bones.size();++i)
	{
		if (m_Bones[i].m_billboard)
		{
			Matrix mtrans;
			GetRenderSystem().getViewMatrix(mtrans);
			mtrans.transpose();
			mtrans.Invert();
			Vec3D camera = mtrans * Vec3D(0.0f,0.0f,0.0f);
			Vec3D look = (camera - m_Bones[i].m_pivot).normalize();

			Vec3D up = ((mtrans * Vec3D(0.0f,1.0f,0.0f)) - camera).normalize();
			// these should be normalized by default but fp inaccuracy kicks in when looking down :(
			Vec3D right = (up % look).normalize();
			up = (look % right).normalize();

			// calculate a billboard matrix
			Matrix mbb=Matrix::UNIT;
			mbb.m[3][2] = 0.0f;
			mbb.m[0][1] = up.x;
			mbb.m[1][1] = up.y;
			mbb.m[2][1] = up.z;
			mbb.m[3][1] = 0.0f;
			mbb.m[3][0] = 0.0f;
			setBonesMatrix[i] *= mbb;
		}

		//m_mat*=Matrix::newTranslation(m_Bones[i].pivot*-1.0f);
		setBonesMatrix[i]*=m_Bones[i].m_mInvLocal;
		//m_mRot = Matrix::newQuatRotate(q);

		/*setBonesMatrix[i].m_mRot=setBonesMatrix[i];
		setBonesMatrix[i].m_mRot._14=0;
		setBonesMatrix[i].m_mRot._24=0;
		setBonesMatrix[i].m_mRot._34=0;*/
	}
}
unsigned char CSkeletonData::getBoneCount()
{
	return m_Bones.size();
}

iBoneInfo* CSkeletonData::allotBoneInfo()
{
	size_t size = m_Bones.size();
	m_Bones.resize(size+1);
	return (iBoneInfo*)&m_Bones[size];
}

iBoneInfo* CSkeletonData::getBoneInfo(unsigned char uBoneID)
{
	if(uBoneID>=m_Bones.size())
	{
		return NULL;
	}
	return (iBoneInfo*)&m_Bones[uBoneID];
}

int CSkeletonData::getBoneIDByName(const char* szName)
{
	size_t uBoneCount = m_Bones.size();
	// ----
	for(size_t i = 0 ; i < uBoneCount; i++)
	{
		if(strcmp(m_Bones[i].m_strName.c_str(),szName)==0)
		{
			return i;
		}
	}
	// ----
	return -1;
}

size_t CSkeletonData::getAnimationCount()
{
	return m_Anims.size();
}

iSkeletonAnim* CSkeletonData::allotAnimation(const std::string& strName)
{
	return (iSkeletonAnim*)&m_Anims[strName];
}

iSkeletonAnim* CSkeletonData::getAnimation(const std::string& strName)
{
	auto it = m_Anims.find(strName);
	if (it!=m_Anims.end())
	{
		return (iSkeletonAnim*)&it->second;
	}
	return NULL;
}

iSkeletonAnim* CSkeletonData::getAnimation(size_t index)
{
	if (m_Anims.size()<=index)
	{
		return NULL;
	}
	auto it = m_Anims.begin();
	advance(it,index);
	return (iSkeletonAnim*)&it->second;
}

bool CSkeletonData::delAnimation(const std::string& strName)
{
	auto it=m_Anims.find(strName);
	if(it!=m_Anims.end())
	{
		m_Anims.erase(it);
		return true;
	}
	return false;
}