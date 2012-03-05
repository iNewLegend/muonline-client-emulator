#include "RenderNode.h"
#include <algorithm>
#include "Intersect.h"
#include "RenderNodeMgr.h"

CRenderNode::CRenderNode()
:m_pParent(NULL)
,m_nBindingBoneID(-1)
,m_vPos(0.0f,0.0f,0.0f)
,m_vRotate(0.0f,0.0f,0.0f)
,m_vScale(1.0f,1.0f,1.0f)
,m_pData(NULL)
,m_nID(0)
,m_nFocus(0)
{
	m_mWorldMatrix.unit();
}


CRenderNode::~CRenderNode()
{
	clearChildren();
}

void CRenderNode::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	updateWorldMatrix();
	m_mRealMatrix = mWorld;
	if (m_pParent&&m_pParent->getType()==NODE_SKELETON)
	{
		CSkeletonNode* pModel = (CSkeletonNode*)m_pParent;
		if (pModel->getSkeletonData())
		{
			// ----
			if (m_nBindingBoneID==-1)
			{
				m_nBindingBoneID = pModel->getSkeletonData()->getBoneIDByName(m_strBindingBoneName.c_str());
			}
			// ----
			if (m_nBindingBoneID>=0&&m_nBindingBoneID<pModel->getBonesMatrix().size())
			{
				Matrix mBoneLocal = pModel->getSkeletonData()->m_Bones[m_nBindingBoneID].m_mInvLocal;
				mBoneLocal.Invert();
				Matrix mBone = pModel->getBonesMatrix()[m_nBindingBoneID]*mBoneLocal;
				m_mRealMatrix *= mBone;
			}
		}
	}
	m_mRealMatrix *= m_mWorldMatrix;
	BBox bbox;
	FOR_IN(it,m_mapChildNode)
	{
		(*it)->frameMove(m_mRealMatrix, fTime, fElapsedTime);
		// ----
		// # Update BBox
		// ----
		bbox += (*it)->getWorldBBox();
	}
	m_LocalBBox = bbox;
	updateWorldBBox();
}

void CRenderNode::render(int nRenderType)const
{
	FOR_IN(it,m_mapChildNode)
	{
		(*it)->render(nRenderType);
	}
}

void CRenderNode::addChild(iRenderNode* pChild)
{
	((CRenderNode*)pChild)->setParent(this);
	m_mapChildNode.push_back((CRenderNode*)pChild);
}

iRenderNode* CRenderNode::getChild(const char* szName)
{
	FOR_IN(it,m_mapChildNode)
	{
		if (strcmp( (*it)->getName(), szName ) == 0 )
		{
			return (*it);
		}
	}
	return NULL;
}

const iRenderNode* CRenderNode::getChild(const char* szName)const
{
	FOR_IN(it,m_mapChildNode)
	{
		if (strcmp( (*it)->getName(), szName ) == 0 )
		{
			return (*it);
		}
	}
	return NULL;
}

bool CRenderNode::removeChild(iRenderNode* pChild)
{
	auto it = std::find(m_mapChildNode.begin(),m_mapChildNode.end(),pChild);
	// ----
	if (it != m_mapChildNode.end())
	{
		m_mapChildNode.erase(it);
		return true;
	}
	return false;
}

bool CRenderNode::delChild(iRenderNode* pChild)
{
	if (removeChild(pChild))
	{
		delete pChild;
		return true;
	}
	return false;
}

bool CRenderNode::contain(const CRenderNode* pChild)const
{
	auto it = std::find(m_mapChildNode.begin(),m_mapChildNode.end(),pChild);
	if (it != m_mapChildNode.end())
	{
		return true;
	}
	return false;
}

void CRenderNode::clearChildren()
{
	FOR_IN(it,m_mapChildNode)
		delete (*it);
	// ----
	removeChildren();
}

void CRenderNode::removeChildren()
{
	m_mapChildNode.clear();
}

iRenderNode* CRenderNode::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax)
{
	Vec3D vNewRayPos = vRayPos;
	Vec3D vNewRayDir = vRayDir;
	transformRay(vNewRayPos,vNewRayDir,m_mWorldMatrix);
	if (!getLocalBBox().intersect(vNewRayPos , vNewRayDir, tmin, tmax))
	{
		return NULL;
	}
	FOR_IN(it,m_mapChildNode)
 	{
		iRenderNode* pRenderNode = (*it)->intersect(vNewRayPos,vNewRayDir,tmin,tmax);
		if(pRenderNode)
		{
			return pRenderNode;
		}
	}
	if(!intersectSelf(vNewRayPos,vNewRayDir,tmin,tmax))
	{
		return NULL;
	}
	return this;
}

void CRenderNode::setChildBindingBone(const char* szName, const char* szBoneName)
{
	CRenderNode* pRenderNodel = (CRenderNode*)getChild(szName);
	// ----
	if (pRenderNodel)
	{
		pRenderNodel->setBindingBoneName(szBoneName);
	}
}

bool CRenderNode::load(const char* szFilename)
{
	char szTemp[256];
	if (m_pData==(void*)-1)
	{
		sprintf(szTemp,"Error: This Node is s broken! Trying to load file \"%s\".\n", szFilename);
		OutputDebugString(szTemp);
		return false;
	}
	if (m_pData)
	{
		return false;
	}
	// ----
	m_strFilename = szFilename;
	// ----
	if (!CRenderNodeMgr::getInstance().loadRenderNode(szFilename,this))
	{
		sprintf(szTemp,"Error: Loading file \"%s\" failed.\n", szFilename);
		OutputDebugString(szTemp);
		m_pData=(void*)-1;
		return false;
	}
	return true;
}

void CRenderNode::updateWorldMatrix()
{
	Matrix mTrans;
	Matrix mRotate;
	Matrix mScale;
	mTrans.translation(getPos());
	mRotate.rotate(getRotate());
	mScale.scale(getScale());
	m_mWorldMatrix = mTrans*mRotate*mScale;
}

void CRenderNode::updateWorldBBox()
{
	m_WorldBBox = m_LocalBBox;
	Matrix mRotate;
	mRotate.rotate(getRotate());

	Vec3D vHalfExtents	= (m_WorldBBox.vMax-m_WorldBBox.vMin)*0.5f*getScale();
	Vec3D vCenter		= (m_WorldBBox.vMax+m_WorldBBox.vMin)*0.5f*getScale();
	vCenter				= mRotate*vCenter+getPos();

	Vec3D vExtent;
	vExtent.x			= Vec3D(abs(mRotate._11),abs(mRotate._12),abs(mRotate._13)).dot(vHalfExtents);
	vExtent.y			= Vec3D(abs(mRotate._21),abs(mRotate._22),abs(mRotate._23)).dot(vHalfExtents);
	vExtent.z			= Vec3D(abs(mRotate._31),abs(mRotate._32),abs(mRotate._33)).dot(vHalfExtents);

	m_WorldBBox.vMin	= vCenter - vExtent;
	m_WorldBBox.vMax	= vCenter + vExtent;
}

Matrix CRenderNode::getShadowMatrix(const Vec3D& vLight,float fHeight)const
{
	Matrix mLight;
	float fDot=-sqrtf(vLight.x*vLight.x+vLight.z*vLight.z)/vLight.y;
	//MessageBoxW(NULL,f2ws(fDot,6,6).c_str(),L"",0);
	Vec3D vEye(getPos().x,fHeight,getPos().z);
	mLight.MatrixLookAtLH(vEye,vEye+vLight,Vec3D(0,1.0f,0));
	Matrix mInvertLight=mLight;
	mInvertLight.Invert();
	Matrix mTransLight(
		1,0,0,0,
		0,1,0,0,
		0,fDot,0,0,
		0,0,0,1);

	Matrix mTrans;
	Matrix mRotate;
	Matrix mScale;
	mTrans.translation(getPos());
	mRotate.rotate(getRotate());
	mScale.scale(getScale());

	return mInvertLight*mTransLight*mLight*mTrans*mRotate*mScale;
}


void CRenderNode::renderShadow(const Matrix& mWorld, const Vec3D& vLight,float fHeight)const
{
	Matrix mNewWorld = mWorld*getShadowMatrix(vLight,fHeight);//m_mWorld;
	render(RF_GEOMETRY|RF_NO_TEXTURE);
}