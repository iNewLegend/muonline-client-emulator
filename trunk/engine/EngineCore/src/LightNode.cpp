#include "LightNode.h"
#include "SkeletonNode.h"
#include "SkeletonData.h"
#include "RenderSystem.h"

CLightNode::CLightNode()
{
}

CLightNode::~CLightNode()
{
}

void CLightNode::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	// ----
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
			if (m_nBindingBoneID!=-1)
			{
				Matrix mBoneLocal = pModel->getSkeletonData()->m_Bones[m_nBindingBoneID].m_mInvLocal;
				mBoneLocal.Invert();
				Matrix mBone = pModel->getBonesMatrix()[m_nBindingBoneID]*mBoneLocal;
				mNewWorld *= mBone;
			}
		}
	}
	// ----
	CRenderNode::frameMove(mNewWorld,fTime,fElapsedTime);
}

void CLightNode::render(const Matrix& mWorld, int nRenderType)const
{
	if ((MATERIAL_LIGHT&nRenderType) == 0)
	{
		return;
	}
	Matrix mNewWorld = mWorld;
	if (m_pParent&&m_pParent->getType()==NODE_SKELETON)
	{
		CSkeletonNode* pModel = (CSkeletonNode*)m_pParent;
		if (pModel->getSkeletonData())
		{
			if (m_nBindingBoneID!=-1)
			{
				Matrix mBoneLocal = pModel->getSkeletonData()->m_Bones[m_nBindingBoneID].m_mInvLocal;
				mBoneLocal.Invert();
				Matrix mBone = pModel->getBonesMatrix()[m_nBindingBoneID]*mBoneLocal;
				mNewWorld *= mBone;
			}
		}
	}
	CRenderSystem& R = CRenderSystem::getSingleton();
	// ----
	Vec3D vPos = mNewWorld*m_vPos;
	R.setShaderFloatArray("g_vPointLight",	&vPos, 3);
	// ----
	CRect<int> rcDest;
	R.getViewport(rcDest);
	// ----
	float QuadVB[4][6] =
	{
		{(float)rcDest.left - 0.5f,		(float)rcDest.bottom - 0.5f,	0.0f, 1.0f, 0.0f, 1.0f},
		{(float)rcDest.left - 0.5f,		(float)rcDest.top - 0.5f,		0.0f, 1.0f, 0.0f, 0.0f},
		{(float)rcDest.right - 0.5f,	(float)rcDest.bottom - 0.5f,	0.0f, 1.0f, 1.0f, 1.0f},
		{(float)rcDest.right - 0.5f,	(float)rcDest.top - 0.5f,		0.0f, 1.0f, 1.0f, 0.0f}
	};
	// ----
	R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, QuadVB, 24);
	// ----
	CRenderNode::render(mNewWorld,nRenderType);
}