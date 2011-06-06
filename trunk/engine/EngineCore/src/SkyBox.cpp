#include "SkyBox.h"
#include "RenderSystem.h"

CSkyBox::CSkyBox()
{
	m_vBox[0][0].p=Vec3D(1, 1, 1);
	m_vBox[0][1].p=Vec3D(1, 1,-1);
	m_vBox[0][2].p=Vec3D(1,-1,-1);
	m_vBox[0][3].p=Vec3D(1,-1, 1);

	m_vBox[1][0].p=Vec3D(-1, 1,-1);
	m_vBox[1][1].p=Vec3D(-1, 1, 1);
	m_vBox[1][2].p=Vec3D(-1,-1, 1);
	m_vBox[1][3].p=Vec3D(-1,-1,-1);

	m_vBox[2][0].p=Vec3D(-1, 1,-1);
	m_vBox[2][1].p=Vec3D(1, 1,-1);
	m_vBox[2][2].p=Vec3D(1, 1, 1);
	m_vBox[2][3].p=Vec3D(-1, 1, 1);

	m_vBox[3][0].p=Vec3D(-1,-1, 1);
	m_vBox[3][1].p=Vec3D(1,-1, 1);
	m_vBox[3][2].p=Vec3D(1,-1,-1);
	m_vBox[3][3].p=Vec3D(-1,-1,-1);

	m_vBox[4][0].p=Vec3D(-1, 1, 1);
	m_vBox[4][1].p=Vec3D(1, 1, 1);
	m_vBox[4][2].p=Vec3D(1,-1, 1);
	m_vBox[4][3].p=Vec3D(-1,-1, 1);

	m_vBox[5][0].p=Vec3D(1, 1,-1);
	m_vBox[5][1].p=Vec3D(-1, 1,-1);
	m_vBox[5][2].p=Vec3D(-1,-1,-1);
	m_vBox[5][3].p=Vec3D(1,-1,-1);

	m_vBox[0][0].t=Vec3D(1, 1, 1);
	m_vBox[0][1].t=Vec3D(1, 1,-1);
	m_vBox[0][2].t=Vec3D(1,-1,-1);
	m_vBox[0][3].t=Vec3D(1,-1, 1);

	m_vBox[1][0].t=Vec3D(-1, 1,-1);
	m_vBox[1][1].t=Vec3D(-1, 1, 1);
	m_vBox[1][2].t=Vec3D(-1,-1, 1);
	m_vBox[1][3].t=Vec3D(-1,-1,-1);

	m_vBox[2][0].t=Vec3D(-1, 1,-1);
	m_vBox[2][1].t=Vec3D(1, 1,-1);
	m_vBox[2][2].t=Vec3D(1, 1, 1);
	m_vBox[2][3].t=Vec3D(-1, 1, 1);

	m_vBox[3][0].t=Vec3D(-1,-1, 1);
	m_vBox[3][1].t=Vec3D(1,-1, 1);
	m_vBox[3][2].t=Vec3D(1,-1,-1);
	m_vBox[3][3].t=Vec3D(-1,-1,-1);

	m_vBox[4][0].t=Vec3D(-1, 1, 1);
	m_vBox[4][1].t=Vec3D(1, 1, 1);
	m_vBox[4][2].t=Vec3D(1,-1, 1);
	m_vBox[4][3].t=Vec3D(-1,-1, 1);

	m_vBox[5][0].t=Vec3D(1, 1,-1);
	m_vBox[5][1].t=Vec3D(-1, 1,-1);
	m_vBox[5][2].t=Vec3D(-1,-1,-1);
	m_vBox[5][3].t=Vec3D(1,-1,-1);
	for(int i=0; i<6; i++)
	{
		//m_vBox[i][0].t=Vec3D(0, 0, 1);
		//m_vBox[i][1].t=Vec3D(1, 0, 1);
		//m_vBox[i][2].t=Vec3D(1, 1, 1);
		//m_vBox[i][3].t=Vec3D(0, 1, 1);
		for (int j=0; j<4; j++)
		{
			m_vBox[i][j].p*=128;
		}
	}
	m_pCubeMap = NULL;
}
CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render()
{
	CRenderSystem& R = GetRenderSystem();
	if (m_pCubeMap==NULL)
	{
		m_pCubeMap = R.GetTextureMgr().CreateCubeTextureFromFile("Data\\Textures\\SkyBox\\morning.jpg");
	}

	R.SetLightingEnabled(false);
	R.SetBlendFunc(false);
	R.SetAlphaTestFunc(false);
	R.SetDepthBufferFunc(false, false);
	R.SetCullingMode(CULL_NONE);
	R.SetTextureColorOP(0, TBOP_SOURCE1, TBS_TEXTURE);
	R.SetTextureAlphaOP(0, TBOP_DISABLE);

	//R.SetRenderState(D3DRS_FOGENABLE, false);

	R.SetFVF(SkyboxVertex::FVF);
	Matrix mView;
	R.getViewMatrix(mView);
	Matrix	mMyView = mView;
	mMyView._14 = mMyView._34 = 0.0f;
	mMyView._24 *= 0.005f;
	R.setViewMatrix(mMyView);
	R.SetTexture(0,m_pCubeMap);
	for (int i=0; i<6; i++)
	{
		R.DrawPrimitiveUP(
			VROT_TRIANGLE_FAN,
			2,
			&m_vBox[i],
			sizeof(SkyboxVertex));
	}
	R.setViewMatrix(mView);
	return;
}