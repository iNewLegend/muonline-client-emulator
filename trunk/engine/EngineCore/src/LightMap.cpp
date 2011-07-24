#include "LightMap.h"
#include "RenderSystem.h"
#include "FileSystem.h"
#include "Scene.h"
#include "Graphics.h"

CLightMap::CLightMap()
{
	m_pBackRenderTarget		= NULL;
	m_pBackDepthStencil		= NULL;

	m_pZDepthStencil		= NULL;
	m_pLightMapRenderTarget	= NULL;
	m_pDepthRenderTarget	= NULL;

	m_uShaderID		=0;

	m_nWidth = 1024;
	m_nHeight = 1024;
	m_pScene = NULL;
}

CLightMap::~CLightMap()
{
	OnDestroyDevice();
}

void CLightMap::SetScene(CScene* pScene)
{
	m_pScene = pScene;
}

HRESULT CLightMap::OnResetDevice()
{
	CRenderSystem& R = GetRenderSystem();
	m_pBackRenderTarget = R.GetRenderTarget();
	m_pBackDepthStencil = R.GetDepthStencil();
	m_pZDepthStencil = R.GetTextureMgr().CreateDepthStencil(m_nWidth,m_nHeight);
	m_pLightMapRenderTarget = R.GetTextureMgr().CreateRenderTarget(m_nWidth,m_nHeight);
	m_pDepthRenderTarget = R.GetTextureMgr().CreateRenderTarget(m_nWidth,m_nHeight);

	//GetRenderSystem().GetDevice()->CreateDepthStencilSurface(
	//	m_nWidth,m_nHeight,
	//	D3DFMT_D16, 
	//	m_MaxMultiSampleType, m_dwMultiSampleQuality,
	//	TRUE, &m_pMSDS, NULL);

	m_uShaderID = R.GetShaderMgr().registerItem("data\\shader\\LightMap.fx");
	return S_OK;
}

void CLightMap::OnLostDevice()
{
	S_DEL(m_pBackRenderTarget);
	S_DEL(m_pBackDepthStencil);

	S_DEL(m_pZDepthStencil);

	S_DEL(m_pLightMapRenderTarget);
	S_DEL(m_pDepthRenderTarget);
}
void CLightMap::OnDestroyDevice()
{
	OnLostDevice();
}

void CLightMap::ClearLightMap()
{
	CRenderSystem& R = GetRenderSystem();
	R.SetRenderTarget(m_pLightMapRenderTarget);
	R.ClearBuffer(true, true, 0x0);
}

void CLightMap::CalcLightMap(int nCalcCount)
{
	//if (pShader==NULL)
	{
		OnLostDevice();
		OnResetDevice();
	}
	//m_pBackRenderTarget->SaveToFile(L"D:/m_pBackRenderTarget.bmp");
	CShader* pShader = GetRenderSystem().GetShaderMgr().getItem(m_uShaderID);
	if (pShader)
	{
		pShader->setFloat("g_fTerrainW", (float)m_pScene->getTerrainData()->getWidth());
		pShader->setFloat("g_fTerrainH", (float)m_pScene->getTerrainData()->getHeight());
	}

	CalcLightMapByShader(nCalcCount);

	// 纹理输出
	//GetRenderSystem().GetDevice()->GetRenderTargetData(m_pLightMapSurf, m_pDataSurf);

	//D3DLOCKED_RECT	LockedRect;
	//HRESULT Result = m_pDataSurf->LockRect(&LockedRect,NULL,0);
	//unsigned long* dst = (unsigned long*)LockedRect.pBits;

	////for (int i = 0; i < m_nWidth * m_nHeight; i++)
	////{
	////	m_LightMap[i] = dst[i]&0xFF;
	////}

	//for (int y = 0; y < m_nHeight; y++)
	//{
	//	for (int x = 0; x < m_nWidth; x++)
	//	{
	//		byte byLightMap = dst[ y*m_nWidth + x ]&0xFF;
	//	//	g_World.SetPixelLightMap(x, y, byLightMap);
	//	}
	//}

	//m_pDataSurf->UnlockRect();
	//m_pBackRenderTarget->SaveToFile(L"D:/m_pBackRenderTarget1.bmp");
	m_pDepthRenderTarget->SaveToFile("D:/Depth.bmp");
	m_pZDepthStencil->SaveToFile("D:/DepthStencil.bmp");
}

void CLightMap::CalcLightMapByShader(int nCalcCount)
{
	if (m_uShaderID==0) return;

	CRenderSystem& R = GetRenderSystem();
	// 灯光
	Vec3D vLightDir = m_pScene->getLight().vDirection;
	//m_pShader->SetFloat("g_fScale",1.0f/nCalcCount);
	//for (int i = 0; i < nCalcCount; i++)
	{
		// 地形阴影
		ClearLightMap();
		CTerrain::LIST_CUBES		renderChunkCubesList;
		//m_pScene->getTerrainData()->getCrunodeCubes(renderChunkCubesList);
		for (size_t i=0; i<renderChunkCubesList.size(); ++i)
		{
			const Cube& cube = *renderChunkCubesList[i];
			// Chunk阴影
			CalcChunkLightMap(vLightDir, cube);
		}
		std::string strFilename = m_pScene->getTerrainData()->getFilename();
		std::string strLightmapFilename = GetParentPath(strFilename)+"LightMap.bmp";
		m_pLightMapRenderTarget->SaveToFile(strLightmapFilename);


		GetRenderSystem().GetTextureMgr().releaseBuffer(strLightmapFilename);
		// 物件
		{
			//CScene::DEQUE_MAPOBJ&	setSceneObj;/// = GetSceneMgr().GetSceneObject();
			//for (auto it = setSceneObj.begin();
			//	it != setSceneObj.end(); ++it)
			//{
			//	ClearLightMap();
			//	CModelObject& modelObject = **it;
			//	CalcObjectLightMap(vLightDir, modelObject);
			//	std::wstring wstrObjectLightmapFilename =
			//		strPath+L"\\"+
			//		s2ws(GetFilename(modelObject.m_strName))+
			//		L".bmp";
			//	m_pLightMapRenderTarget->SaveToFile(wstrObjectLightmapFilename);
			//	modelObject.SetLightMap(ws2s(wstrObjectLightmapFilename));
			//}
		}
	}
	R.SetRenderTarget(m_pBackRenderTarget);
	R.SetDepthStencil(m_pBackDepthStencil);
}

void CLightMap::RenderDepthToTex(const Matrix& mLight)
{
	CShader* pShader = GetRenderSystem().GetShaderMgr().getItem(m_uShaderID);
	if (!pShader)
	{
		return;
	}
	CFrustum frustum;
	frustum.buildNoNear(mLight);
	//frustum.Build(mLight);
	CRenderSystem& R = GetRenderSystem();
	//
	R.SetRenderTarget(m_pDepthRenderTarget);
	R.SetDepthStencil(m_pZDepthStencil);
	R.ClearBuffer(false, true,0xFFFFFFFF);
	R.ClearBuffer(true, true, 0xFFFFFFFF);
	if (R.BeginFrame())
	{
		R.SetCullingMode(CULL_ANTI_CLOCK_WISE);
		R.SetBlendFunc(false);
		R.SetDepthBufferFunc(true,true);
		pShader->setMatrix("g_mViewProj",mLight);
		if (pShader->begin("RenderDepth"))// 地表
		{
			if (m_pScene->getTerrainData()->Prepare())
			{
				R.SetIndices(m_pScene->getTerrainData()->GetLightMapDecal().GetIB());
				CTerrain::LIST_CUBES		renderChunkCubesList;
				{
					CTerrain::LIST_CUBES		renderCubesList;
					m_pScene->getTerrainData()->getCubesByFrustum(frustum, renderCubesList);
					for (size_t i=0; i<renderCubesList.size(); ++i)
					{
						renderCubesList[i]->getChildCrunodeCubes(renderChunkCubesList);
					}
				}
				for (size_t i=0; i<renderChunkCubesList.size(); ++i)
				{
					const Cube& cube = *renderChunkCubesList[i];
					m_pScene->getTerrainData()->DrawChunk(cube);
				}
			}
			pShader->end();
		}
		//m_pDepthRenderTarget->SaveToFile(L"D:\\depth.bmp");

		// 物件
		{
			std::set<iRenderNode*>	setRenderSceneObj;
			m_pScene->getRenderNodes(frustum, setRenderSceneObj);
			FOR_IN(it,setRenderSceneObj)
			{
				//if ((*it)->GetObjType()==MAP_3DSIMPLE)
				{
					C3DMapSceneObj* p3DMapSceneObj =(C3DMapSceneObj*)(*it);
					Matrix mObjWorld = p3DMapSceneObj->getWorldMatrix();
					pShader->setMatrix("g_mWorld",mObjWorld);
					if(pShader->begin("RenderObjDepth"))
					{
						p3DMapSceneObj->frameMove(Matrix::UNIT,0.0f,0.0f);
						p3DMapSceneObj->render(Matrix::UNIT,MATERIAL_GEOMETRY);
						pShader->end();
					}
				}
			}
		}
		R.EndFrame();
	}
}

void CLightMap::RenderTerrainLightMapToTex(const Matrix& mLight, const Cube& cube)
{
	CShader* pShader = GetRenderSystem().GetShaderMgr().getItem(m_uShaderID);
	if (!pShader)
	{
		return;
	}
	CRenderSystem& R = GetRenderSystem();
	R.SetRenderTarget(m_pLightMapRenderTarget);
	pShader->setMatrix("g_mViewProj",mLight);
	pShader->setTexture("g_txDepth", m_pDepthRenderTarget);

	//R.SetTexture(0, m_pDepthRenderTarget);
	if (R.BeginFrame())
	{
		if (pShader->begin("RenderToLightMapTex"))
		{
			if (m_pScene->getTerrainData()->Prepare())
			{
				R.SetIndices(m_pScene->getTerrainData()->GetLightMapDecal().GetIB());

				R.SetCullingMode(CULL_NONE);
				R.SetDepthBufferFunc(false,false);
				m_pScene->getTerrainData()->DrawChunk(cube);
			}
			pShader->end();
		}
	}
	R.EndFrame();
}

void CLightMap::RenderObjectLightMapToTex(const Matrix& mLight, const C3DMapSceneObj& object)
{
	CShader* pShader = GetRenderSystem().GetShaderMgr().getItem(m_uShaderID);
	if (!pShader)
	{
		return;
	}

	CRenderSystem& R = GetRenderSystem();
	pShader->setMatrix("g_mViewProj",mLight);
	R.SetRenderTarget(m_pLightMapRenderTarget);
	if (R.BeginFrame())
	{
		pShader->setTexture("g_txDepth", m_pDepthRenderTarget);
		Matrix mObjWorld = object.getWorldMatrix();
		pShader->setMatrix("g_mWorld",mObjWorld);
		if (pShader->begin("RenderObjLightMapToTex"))
		{
			R.SetTexture(0, m_pDepthRenderTarget);
			object.render(Matrix::UNIT, E_MATERIAL_RENDER_TYPE(MATERIAL_GEOMETRY|MATERIAL_RENDER_ALPHA_TEST));
			pShader->end();
		}

		//
		pShader->setTexture("g_txDepth", m_pLightMapRenderTarget);
		if (pShader->begin("LightMapJoint"))
		{
			RECT rcQuad = {0,0,m_nWidth,m_nHeight};
			GetGraphics().DrawQuad(rcQuad,rcQuad,m_nWidth,m_nHeight,0x0);
			pShader->end();
		}
	}
	R.EndFrame();
}

Matrix CLightMap::CalcLightMatrix(const BBox& bbox, const Vec3D& vLightDir)
{
	float fLength = (bbox.vMax-bbox.vMin).length();
	Vec3D vLookAt=(bbox.vMax+bbox.vMin)*0.5f;
	Vec3D vEyePt = vLookAt - vLightDir*fLength*0.5f;

	Vec3D vUp(0,1,0);
	Matrix mView, mProj;
	mView.MatrixLookAtLH(vEyePt,vLookAt,vUp);
	mProj.MatrixOrthoLH(fLength,fLength, 0, fLength);
	Matrix mLight = mProj*mView;
	return mLight;
}

void CLightMap::CalcChunkLightMap(const Vec3D& vLightDir, const Cube& cube)
{
	Matrix mLight = CalcLightMatrix(cube.bbox, vLightDir);
	RenderDepthToTex(mLight);
	RenderTerrainLightMapToTex(mLight, cube);
}

void CLightMap::CalcObjectLightMap(const Vec3D& vLightDir, const C3DMapSceneObj& object)
{
	const BBox& bbox = object.getLocalBBox();
	Matrix mLight = CalcLightMatrix(bbox, vLightDir);
	RenderDepthToTex(mLight);
	RenderObjectLightMapToTex(mLight, object);
}