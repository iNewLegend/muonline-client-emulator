#include "RenderSystem.h"

static CRenderSystem* g_pRenderSystem = NULL;

CRenderSystem::CRenderSystem()
{
	g_pRenderSystem = this;
}
CRenderSystem::~CRenderSystem()
{
}

void CRenderSystem::setSingleton(CRenderSystem* pRenderSystem)
{
	g_pRenderSystem = pRenderSystem;
}

CRenderSystem& CRenderSystem::getSingleton()
{
	return *g_pRenderSystem;
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

CShader* CRenderSystem::registerShader(const char* szName, const char* szFilename)
{
	CShader* pShader = getShader(szName);
	if (pShader)
	{
		return pShader;
	}
	// New Shader
	pShader = CRenderSystem::getSingleton().newShader();
	// Create shader faild!
	if (!pShader->create(szFilename))
	{
		delete pShader;
		return NULL;
	}
	//
	m_mapShaders[szName] = pShader;
	return pShader;
}

CShader* CRenderSystem::getShader(const char* szName)
{
	auto it = m_mapShaders.find(szName);
	if (it!=m_mapShaders.end())
	{
		return it->second;
	}
	return NULL;
}