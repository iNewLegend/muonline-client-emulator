#include "MeshCoordinate.h"
#include "RenderSystem.h"
#include "Intersect.h"

CMeshCoordinate::CMeshCoordinate()
{
	m_CoordLines[CLT_X].vBegin = Vec3D(0.75f,0,0);
	m_CoordLines[CLT_X].vEnd = Vec3D(0.2f,0,0);
	m_CoordLines[CLT_X_Y].vBegin = Vec3D(0.3f,0,0);
	m_CoordLines[CLT_X_Y].vEnd = Vec3D(0.3f,0.3f,0);
	m_CoordLines[CLT_X_Z].vBegin = Vec3D(0.3f,0,0);
	m_CoordLines[CLT_X_Z].vEnd = Vec3D(0.3f,0,0.3f);

	m_CoordLines[CLT_Y].vBegin = Vec3D(0,0.75f,0);
	m_CoordLines[CLT_Y].vEnd = Vec3D(0,0.2f,0);
	m_CoordLines[CLT_Y_X].vBegin = Vec3D(0,0.3f,0);
	m_CoordLines[CLT_Y_X].vEnd = Vec3D(0.3f,0.3f,0);
	m_CoordLines[CLT_Y_Z].vBegin = Vec3D(0,0.3f,0);
	m_CoordLines[CLT_Y_Z].vEnd = Vec3D(0,0.3f,0.3f);

	m_CoordLines[CLT_Z].vBegin = Vec3D(0,0,0.75f);
	m_CoordLines[CLT_Z].vEnd = Vec3D(0,0,0.2f);
	m_CoordLines[CLT_Z_X].vBegin = Vec3D(0,0,0.3f);
	m_CoordLines[CLT_Z_X].vEnd = Vec3D(0.3f,0,0.3f);
	m_CoordLines[CLT_Z_Y].vBegin = Vec3D(0,0,0.3f);
	m_CoordLines[CLT_Z_Y].vEnd = Vec3D(0,0.3f,0.3f);
}

CMeshCoordinate::~CMeshCoordinate()
{
}

void CMeshCoordinate::init()
{
	m_setSubMesh.resize(3);
	const size_t CIRCLE_LINE_COUNT=6;
	addPos(Vec3D(1,0,0));
	for(size_t i=0; i<CIRCLE_LINE_COUNT; ++i)
	{
		float fRadian = PI*i*2/CIRCLE_LINE_COUNT;
		Vec3D v(0.75f,sinf(fRadian)*0.05f,cosf(fRadian)*0.05f);
		addPos(v);
	}
	addPos(Vec3D(0,1,0));
	for (size_t i=0; i<CIRCLE_LINE_COUNT; ++i)
	{
		float fRadian = PI*i*2/CIRCLE_LINE_COUNT;
		Vec3D v(cosf(fRadian)*0.05f,0.75f,sinf(fRadian)*0.05f);
		addPos(v);
	}
	addPos(Vec3D(0,0,1));
	for (size_t i=0; i<CIRCLE_LINE_COUNT; ++i)
	{
		float fRadian = PI*i*2/CIRCLE_LINE_COUNT;
		Vec3D v(sinf(fRadian)*0.05f,cosf(fRadian)*0.05f,0.75f);
		addPos(v);
	}
	//
	addColor(0xFFFF0000);
	addColor(0xFF00FF00);
	addColor(0xFF0000FF);
	addColor(0xFF880000);
	addColor(0xFF008800);
	addColor(0xFF000088);
	//
	for (size_t i=0;i<3;++i)
	{
		size_t start = 0;//i*(CIRCLE_LINE_COUNT+1);
		for (size_t j=0;j<CIRCLE_LINE_COUNT-1;++j)
		{
			VertexIndex vertexIndex;
			vertexIndex.c=0;
			vertexIndex.p=0+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=1+j+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=2+j+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
		}
		{
			VertexIndex vertexIndex;
			vertexIndex.c=0;
			vertexIndex.p=0+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=CIRCLE_LINE_COUNT+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=1+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
		}
		for (size_t j=0;j<CIRCLE_LINE_COUNT-2;++j)
		{
			VertexIndex vertexIndex;
			vertexIndex.c=1;
			vertexIndex.p=1+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=2+j+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
			vertexIndex.p=3+j+start;
			m_setSubMesh[i].m_setVertexIndex.push_back(vertexIndex);
		}
	}
	//
	CLodMesh::init();
}

void CMeshCoordinate::setPos(const Vec3D& vPos)
{
	m_vPos = vPos;
}

void CMeshCoordinate::setScale(float fScale)
{
	m_fScale = fScale;
}

bool CMeshCoordinate::intersect(const Vec3D& vRayPos , const Vec3D& vRayDir,Vec3D& vCoord)const
{
	Vec3D vNewRayPos = vRayPos;
	Vec3D vNewRayDir = vRayDir;
	transformRay(vNewRayPos,vNewRayDir,getWorldMatrix());

	float fMin,fMax;
	float fR = 0.05f;
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_X].vBegin,m_CoordLines[CLT_X].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,0,0);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_X_Y].vBegin,m_CoordLines[CLT_X_Y].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,1,0);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_X_Z].vBegin,m_CoordLines[CLT_X_Z].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,0,1);
		return true;
	}

	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Y].vBegin,m_CoordLines[CLT_Y].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(0,1,0);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Y_X].vBegin,m_CoordLines[CLT_Y_X].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,1,0);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Y_Z].vBegin,m_CoordLines[CLT_Y_Z].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(0,1,1);
		return true;
	}

	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Z].vBegin,m_CoordLines[CLT_Z].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(0,0,1);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Z_X].vBegin,m_CoordLines[CLT_Z_X].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(1,0,1);
		return true;
	}
	if(LineCapsuleIntersector(vNewRayPos,vNewRayDir,m_CoordLines[CLT_Z_Y].vBegin,m_CoordLines[CLT_Z_Y].vEnd,fR,fMin,fMax)>0)
	{
		vCoord = Vec3D(0,1,1);
		return true;
	}

	Vec3D vOut;
	int nSubID  = -1;
	if (CLodMesh::intersect(vNewRayPos, vNewRayDir, vOut, nSubID))
	{
		switch(nSubID)
		{
		case 0:
			vCoord = Vec3D(1,0,0);
			break;
		case 1:
			vCoord = Vec3D(0,1,0);
			break;
		case 2:
			vCoord = Vec3D(0,0,1);
			break;
		default:
			break;
		}
		return true;
	}
	return false;
}
//#include "TextRender.h"
#include "Graphics.h"

#define COORD_X_COLOR 0xFFFF0000
#define COORD_Y_COLOR 0xFF00FF00
#define COORD_Z_COLOR 0xFF0000FF
#define COORD_SELECT_COLOR 0xFFFFFF00
#define COORD_PLANE_COLOR 0x44FFFF00
void CMeshCoordinate::render(const Vec3D& vCoordShow)
{
	if (m_setSubMesh.empty())
	{
		init();
	}
	CRenderSystem& R = GetRenderSystem();
	CGraphics& G=GetGraphics();

	R.setWorldMatrix(getWorldMatrix());

	if (R.prepareMaterial("Coordinate1"))
	{
		SetMeshSource();
		draw();

		G.DrawLine3D(m_CoordLines[CLT_X].vBegin,m_CoordLines[CLT_X].vEnd,vCoordShow.x>0?COORD_SELECT_COLOR:COORD_X_COLOR);
		G.DrawLine3D(m_CoordLines[CLT_X_Y].vBegin,m_CoordLines[CLT_X_Y].vEnd,(vCoordShow.x>0&&vCoordShow.y>0)?COORD_SELECT_COLOR:COORD_X_COLOR);
		G.DrawLine3D(m_CoordLines[CLT_X_Z].vBegin,m_CoordLines[CLT_X_Z].vEnd,(vCoordShow.x>0&&vCoordShow.z>0)?COORD_SELECT_COLOR:COORD_X_COLOR);

		G.DrawLine3D(m_CoordLines[CLT_Y].vBegin,m_CoordLines[CLT_Y].vEnd,vCoordShow.y>0?COORD_SELECT_COLOR:COORD_Y_COLOR);
		G.DrawLine3D(m_CoordLines[CLT_Y_X].vBegin,m_CoordLines[CLT_Y_X].vEnd,(vCoordShow.x>0&&vCoordShow.y>0)?COORD_SELECT_COLOR:COORD_Y_COLOR);
		G.DrawLine3D(m_CoordLines[CLT_Y_Z].vBegin,m_CoordLines[CLT_Y_Z].vEnd,(vCoordShow.y>0&&vCoordShow.z>0)?COORD_SELECT_COLOR:COORD_Y_COLOR);

		G.DrawLine3D(m_CoordLines[CLT_Z].vBegin,m_CoordLines[CLT_Z].vEnd,vCoordShow.z>0?COORD_SELECT_COLOR:COORD_Z_COLOR);
		G.DrawLine3D(m_CoordLines[CLT_Z_X].vBegin,m_CoordLines[CLT_Z_X].vEnd,(vCoordShow.x>0&&vCoordShow.z>0)?COORD_SELECT_COLOR:COORD_Z_COLOR);
		G.DrawLine3D(m_CoordLines[CLT_Z_Y].vBegin,m_CoordLines[CLT_Z_Y].vEnd,(vCoordShow.y>0&&vCoordShow.z>0)?COORD_SELECT_COLOR:COORD_Z_COLOR);

		R.finishMaterial();
	}
	if (R.prepareMaterial("Coordinate2"))
	{
		if (vCoordShow.x>0&&vCoordShow.y>0)
		{
		//	G.fillQuad(Vec3D(0,0,0),m_CoordLines[CLT_X_Y].vBegin,m_CoordLines[CLT_X_Y].vEnd,m_CoordLines[CLT_Y_X].vBegin,COORD_PLANE_COLOR);
		}
		if (vCoordShow.y>0&&vCoordShow.z>0)
		{
		//	G.fillQuad(Vec3D(0,0,0),m_CoordLines[CLT_Y_Z].vBegin,m_CoordLines[CLT_Y_Z].vEnd,m_CoordLines[CLT_Z_Y].vBegin,COORD_PLANE_COLOR);
		}
		if (vCoordShow.z>0&&vCoordShow.x>0)
		{
		//	G.fillQuad(Vec3D(0,0,0),m_CoordLines[CLT_Z_X].vBegin,m_CoordLines[CLT_Z_X].vEnd,m_CoordLines[CLT_X_Z].vBegin,COORD_PLANE_COLOR);
		}
		R.finishMaterial();
	}

// 	R.SetBlendFunc(true);
// 	R.SetTextureColorOP(0,TBOP_MODULATE);
// 	R.SetTextureAlphaOP(0,TBOP_MODULATE);

	//int nFontHeight = GetTextRender().GetCharHeight();
	//Pos2D posScreen;
	//R.world2Screen(getWorldMatrix()*Vec3D(1.0f,0.0f,0.0f),posScreen);
	//GetTextRender().drawText(L"X",posScreen.x,posScreen.y-nFontHeight,vCoordShow.x>0?COORD_SELECT_COLOR:COORD_X_COLOR);

	//R.world2Screen(getWorldMatrix()*Vec3D(0.0f,1.0f,0.0f),posScreen);
	//GetTextRender().drawText(L"Y",posScreen.x,posScreen.y-nFontHeight,vCoordShow.y>0?COORD_SELECT_COLOR:COORD_Y_COLOR);

	//R.world2Screen(getWorldMatrix()*Vec3D(0.0f,0.0f,1.0f),posScreen);
	//GetTextRender().drawText(L"Z",posScreen.x,posScreen.y-nFontHeight,vCoordShow.z>0?COORD_SELECT_COLOR:COORD_Z_COLOR);
}

Matrix CMeshCoordinate::getWorldMatrix()const
{
	Matrix mTrans,mScale;
	mTrans.translation(m_vPos);
	mScale.scale(Vec3D(m_fScale,m_fScale,m_fScale));

	return mTrans*mScale;
}