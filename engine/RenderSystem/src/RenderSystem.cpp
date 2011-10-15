#include "RenderSystem.h"

static CRenderSystem* g_pRenderSystem = NULL;

void SetRenderSystem(CRenderSystem* pRenderSystem)
{
	g_pRenderSystem = pRenderSystem;
}

CRenderSystem& GetRenderSystem()
{
	//assert(g_pRenderSystem);
	return *g_pRenderSystem;
}

CRenderSystem::CRenderSystem()
{
}
CRenderSystem::~CRenderSystem()
{
}

void CRenderSystem::world2Screen(const Vec3D& vWorldPos, Pos2D& posScreen)
{
	Matrix mProj;
	Matrix View;
	getProjectionMatrix(mProj);
	getViewMatrix(View);
	Vec4D vOut = mProj*View*Vec4D(vWorldPos,1);
	float fW = vOut.w;
	CRect<int> rc;
	getViewport(rc);
	posScreen.x = int(rc.left+(rc.right-rc.left)*(0.5f+vOut.x*0.5f/fW));
	posScreen.y = int(rc.top+(rc.bottom-rc.top)*(0.5f-vOut.y*0.5f/fW));
}

#include "Intersect.h"
void CRenderSystem::GetPickRay(Vec3D& vRayPos, Vec3D& vRayDir,int x, int y)
{
	Matrix mProj;
	getProjectionMatrix(mProj);
	CRect<int> rc;
	getViewport(rc);
	Matrix mView;
	getViewMatrix(mView);
	::GetPickRay(vRayPos,vRayDir,x,y,mView,mProj,rc.getRECT());
}

CMaterialMgr& CRenderSystem::getMaterialMgr()
{
	return m_MaterialMgr;
}

bool CRenderSystem::prepareMaterial(const char* szMaterialName, float fOpacity)
{
	return prepareMaterial(getMaterialMgr().getItem(szMaterialName),fOpacity);
}

bool CRenderSystem::prepareMaterial(/*const */CMaterial& material, float fOpacity) // 由于使用了自动注册纹理的机制,很遗憾的导致不能用“const”
{
	CTextureMgr& TM = GetTextureMgr();
	for (size_t i=0;i<8;++i)
	{
		if (material.uTexture[i]==-1)
		{
			material.uTexture[i] = TM.RegisterTexture(material.getTexture(i));
		}
		// ----
		SetTexture(i, material.uTexture[i]);
		// ----
		if (material.uTexture[i]==0)
		{
			break;
		}
	}
	if (material.uShader==-1)
	{
		material.uShader = GetShaderMgr().registerItem(material.getShader());
	}
	// ----
	if (material.uShader!=0)
	{
		SetShader(material.uShader);
	}
	return true;
}
void CRenderSystem::finishMaterial()
{
	SetShader((CShader*)NULL);
	setResultARGToTemp(0,false);
	setResultARGToTemp(1,false);
	setResultARGToTemp(2,false);
}