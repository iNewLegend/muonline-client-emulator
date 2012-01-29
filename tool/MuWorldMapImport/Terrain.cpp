#include "MyPlug.h"
#include "IORead.h"
#include "FileSystem.h"
#include "windows.h"

#define MAP_FILE_SIZE 65536*3+2
#define HEIGHT_HEAD_SIZE 1082
#define OZJ_HEAD_SIZE 24
#define LIGHT_MAP_SIZE 65536*3
#define HEIGHT_BUFFER_SIZE 65536
#define ATT_FILE_129KB_SIZE 65536*2+4
#define ATT_FILE_65KB_SIZE 65536+4
#define ATT_FILE_SERVER_SIZE 65536+3

inline void decrypt2(char* buffer, size_t size)
{
	const char xorKeys[] = {0xFC, 0xCF, 0xAB};
	for (size_t i=0; i<size; ++i)
	{
		*buffer ^= xorKeys[i%3];
		buffer++;
	}
}

inline void decrypt(char* buffer, size_t size)
{
	const char xorKeys[] = {
		0xd1, 0x73, 0x52, 0xf6,
		0xd2, 0x9a, 0xcb, 0x27,
		0x3e, 0xaf, 0x59, 0x31,
		0x37, 0xb3, 0xe7, 0xa2
	};
	char key = 0x5E;
	for (size_t i=0; i<size; ++i)
	{
		char encode = *buffer;
		*buffer ^= xorKeys[i%16];
		*buffer -= key;
		key = encode+0x3D;
		buffer++;
	}
}

inline void encrypt(char* buffer, size_t size)
{
	const char xorKeys[] = {
		0xd1, 0x73, 0x52, 0xf6,
		0xd2, 0x9a, 0xcb, 0x27,
		0x3e, 0xaf, 0x59, 0x31,
		0x37, 0xb3, 0xe7, 0xa2
	};
	char key = 0x5E;
	for (size_t i=0; i<size; ++i)
	{
		*buffer += key;
		*buffer ^= xorKeys[i%16];
		key = *buffer+0x3D;
		buffer++;
	}
}

bool CMyPlug::importTerrainData(iSceneData * pSceneData, const std::string& strFilename)
{
	pSceneData->clear();
	if (pSceneData->resize(253,253))
	{
		// EncTerrain
		IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
		if (pRead)
		{
			if (MAP_FILE_SIZE==pRead->GetSize())
			{
				char buffer[MAP_FILE_SIZE];
				pRead->Read(buffer,MAP_FILE_SIZE);
				decrypt(buffer,MAP_FILE_SIZE);
				char* p = buffer;
				unsigned short uMuFlgMap = *((unsigned short*)p);
				p+=2;
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						pSceneData->setCellTileID(x,y,*p,0);
						p++;
					}
					p+=3;
				}
				p+=256*3;
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						pSceneData->setCellTileID(x,y,*p,1);
						p++;
					}
					p+=3;
				}
				p+=256*3;
				for (int y=0; y<254; ++y)
				{
					for (int x=0; x<254; ++x)
					{
						pSceneData->setVertexColor(x,y,Color32(*p,255,255,255));
						p++;
					}
					p+=2;
				}
			}
			IOReadBase::autoClose(pRead);
		}
		// EncTerrain.att
		pRead = IOReadBase::autoOpen(ChangeExtension(strFilename,".att"));
		if (pRead)
		{
			if (ATT_FILE_129KB_SIZE==pRead->GetSize())
			{
				char buffer[ATT_FILE_129KB_SIZE];
				pRead->Read(buffer,ATT_FILE_129KB_SIZE);
				decrypt(buffer,ATT_FILE_129KB_SIZE);
				decrypt2(buffer,ATT_FILE_129KB_SIZE);
				char* p = buffer;
				unsigned long uMuFlgAtt = *((unsigned long*)p);
				p+=4;
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						pSceneData->setCellAttribute(x,y,*p);
						p+=2;
					}
					p+=6;
				}
			}
			else if (ATT_FILE_65KB_SIZE==pRead->GetSize())
			{
				char buffer[ATT_FILE_65KB_SIZE];
				pRead->Read(buffer,ATT_FILE_65KB_SIZE);
				decrypt(buffer,ATT_FILE_65KB_SIZE);
				decrypt2(buffer,ATT_FILE_65KB_SIZE);
				char* p = buffer;
				unsigned long uMuFlgAtt = *((unsigned long*)p);
				p+=4;
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						pSceneData->setCellAttribute(x,y,*p);
						p++;
					}
					p+=3;
				}
			}
			IOReadBase::autoClose(pRead);
		}
		// TerrainHeight
		std::string strHeightFilename = GetParentPath(strFilename)+"TerrainHeight.ozb";
		pRead = IOReadBase::autoOpen(strHeightFilename);
		if (pRead)
		{
			if (HEIGHT_HEAD_SIZE+HEIGHT_BUFFER_SIZE<=pRead->GetSize())
			{
				pRead->Move(HEIGHT_HEAD_SIZE);
				for (int y=0; y<254; ++y)
				{
					pRead->Move(2);
					for (int x=0; x<254; ++x)
					{
						unsigned char uVal;
						pRead->Read(&uVal,1);
						pSceneData->setVertexHeight(x,y,uVal*0.015f);
					}
				}
			}
			IOReadBase::autoClose(pRead);
		}
	}
	return true;
}

bool CMyPlug::importSceneTerrainData(iRenderNode* pRenderNode, iSceneData* pSceneData, const char* szFilename)
{
	importTerrainData(pSceneData,szFilename);
	const char* szTerrainMaterial[30][2]={
		{"TileGrass01.ozj","terrainTileX4"},
		{"TileGrass02.ozj","terrainTileX2"},
		{"TileGround01.ozj","terrainTileX4"},
		{"TileGround02.ozj","terrainTileX2"},
		{"TileGround03.ozj","terrainTileX2"},
		{"TileWater01.ozj","terrainWater"},
		{"TileWood01.ozj","terrainTileX2"},
		{"TileRock01.ozj","terrainTileX2"},
		{"TileRock02.ozj","terrainTileX2"},
		{"TileRock03.ozj","terrainTileX2"},
		{"TileRock04.ozj","terrainTileX2"},
		{"TileRock05.ozj","terrainTileX2"},
		{"TileRock06.ozj","terrainTileX2"},
		{"TileRock07.ozj","terrainTileX2"},
		{"TileRock08.ozj","terrainTileX2"},
		{"TileGrass01.ozj","terrainTileAlphaX4"},
		{"TileGrass02.ozj","terrainTileAlphaX2"},
		{"TileGround01.ozj","terrainTileAlphaX4"},
		{"TileGround02.ozj","terrainTileAlphaX2"},
		{"TileGround03.ozj","terrainTileAlphaX2"},
		{"TileWater01.ozj","terrainWaterAlpha"},
		{"TileWood01.ozj","terrainTileAlphaX2"},
		{"TileRock01.ozj","terrainTileAlphaX2"},
		{"TileRock02.ozj","terrainTileAlphaX2"},
		{"TileRock03.ozj","terrainTileAlphaX2"},
		{"TileRock04.ozj","terrainTileAlphaX2"},
		{"TileRock05.ozj","terrainTileAlphaX2"},
		{"TileRock06.ozj","terrainTileAlphaX2"},
		{"TileRock07.ozj","terrainTileAlphaX2"},
		{"TileRock08.ozj","terrainTileAlphaX2"}
	};
	CMaterial mat;
	// ----
	// # Create Grasses
	// ----
	for (int startY=0; startY<pSceneData->getHeight(); startY+=16)
	{
		for (int startX=0; startX<pSceneData->getWidth(); startX+=16)
		{
			int nWidth = min(pSceneData->getWidth()-startX,16);
			int nHeight = min(pSceneData->getHeight()-startY,16);
			int nEndX = startX+nWidth;
			int nEndY = startY+nHeight;
			char szMeshName[256];
			// 
			sprintf(szMeshName,"%s_%d_%d",szFilename,startX,startY);
			iMeshData* pMesh = (iMeshData*)m_pRenderNodeMgr->createRenderData("mesh",szMeshName);

			BBox bbox;

			std::vector<RigidNolightVertex>& setVertices = pMesh->getRigidNolightVertices();

			// ----
			// # Ground
			// ----
			for (int y=startY; y<=nEndY; ++y)
			{
				for (int grassX=startX; grassX<=nEndX; ++grassX)
				{
					RigidNolightVertex vertex;

					float fHeight = pSceneData->getVertexHeight(grassX,y);
					Vec3D vNormal = pSceneData->getVertexNormal(grassX,y);
					Color32 color = pSceneData->getVertexColor(grassX,y);

					bbox.vMin.y = min(fHeight,bbox.vMin.y);
					bbox.vMax.y = max(fHeight,bbox.vMax.y);

						/*Vec3D vBinormal = vertex.n.cross(Vec3D(0,0,1)).normalize();
	Vec3D vTangent =  Vec3D(1,0,0).cross(vertex.n).normalize();
	Matrix mTangent;
	mTangent.Zero();
	mTangent._11=vBinormal.x;
	mTangent._21=vBinormal.y;
	mTangent._31=vBinormal.z;

	mTangent._12=vertex.n.x;
	mTangent._22=vertex.n.y;
	mTangent._32=vertex.n.z;

	mTangent._13=vTangent.x;
	mTangent._23=vTangent.y;
	mTangent._33=vTangent.z;

	vertex.n = mTangent*GetLightDir();
	vertex.n=vertex.n.normalize();*/

					vertex.p.set( (float)grassX, fHeight, (float)y );
					vertex.n = vNormal;
					vertex.c = color;
					vertex.uv.set( (float)grassX,(float)y );
					setVertices.push_back(vertex);
				}
			}
			// ----
			std::map<unsigned char,std::vector<int>> mapLayer[2];
			int i=0;
			for (int y=startY; y<nEndY; ++y)
			{
				for (int grassX=startX; grassX<nEndX; ++grassX)
				{
					unsigned char title0 = pSceneData->getCellTileID(grassX,y,0);
					unsigned char title1 = pSceneData->getCellTileID(grassX,y,1);
					//TerrainCell& cell = *m_pTerrainData->getCell(x,y);
					//if ((cell.uAttribute&0x8)==0)// Í¸Ã÷
					if (title0!=0xFF)// Í¸Ã÷
					{
						mapLayer[0][title0].push_back(i);
					}
					if (title1!=0xFF)// Í¸Ã÷
					{
						mapLayer[1][title1].push_back(i);
					}
					i++;
				}
				i++;
			}
			// ----
			IndexedSubset subset;
			for (int nLayer=0;nLayer<2;++nLayer)
			{
				for (auto it= mapLayer[nLayer].begin(); it!=mapLayer[nLayer].end(); ++it)
				{
					// ----
					// # Subset
					// ----
					subset.vstart = 0;
					subset.vbase = 0;
					subset.istart += subset.icount;
					subset.vcount = setVertices.size();
					subset.icount = it->second.size()*6;
					pMesh->getSubsets().push_back(subset);
					// ----
					// # Material
					// ----
					char szTexture[256];
					sprintf(szTexture,"%s%s",GetParentPath(szFilename).c_str(),szTerrainMaterial[it->first+nLayer*15][0]);
					mat.strTexture[0] = szTexture;
					mat.strShader = szTerrainMaterial[it->first+nLayer*15][1];
					pMesh->getMaterials().resize(pMesh->getSubsets().size());
					pMesh->getMaterials()[pMesh->getSubsets().size()-1].push_back(mat);
					// ----
					// # Indices
					// ----
					for (auto it2=it->second.begin(); it2!=it->second.end(); ++it2)
					{
						// 1	 2
						//	*---*
						//	| / |
						//	*---*
						// 0	 3
						int i = *it2;
						pMesh->getIndices().push_back(i);
						pMesh->getIndices().push_back(i+nWidth+1);
						pMesh->getIndices().push_back(i+nWidth+2);
						pMesh->getIndices().push_back(i);
						pMesh->getIndices().push_back(i+nWidth+2);
						pMesh->getIndices().push_back(i+1);
					}
				}
			}
			// ----
			// # Grass
			// ----
			for (int y=startY; y<nEndY; ++y)
			{
				for (int x=startX; x<nEndX; ++x)
				{
					if (pSceneData->hasGrass(x,y))
					{
						float fHeight1 = pSceneData->getVertexHeight(x,y);
						float fHeight2 = pSceneData->getVertexHeight(x+1,y+1);

						Vec3D vNormal1 = pSceneData->getVertexNormal(x,y);
						Vec3D vNormal2 = pSceneData->getVertexNormal(x+1,y+1);

						Color32 color1 = pSceneData->getVertexColor(x,y);
						Color32 color2 = pSceneData->getVertexColor(x+1,y+1);

						int	nRand = (((y*(pSceneData->getWidth()+1)+x+x*y)*214013L+2531011L)>>16)&0x7fff;   
						float fTexU = (nRand%4)*0.25f;
						RigidNolightVertex vertex;

						vertex.p.set( (float)x, fHeight1, (float)y );
						vertex.n = vNormal1;
						vertex.c = color1;
						vertex.uv.set( (float)fTexU,1.0f );
						setVertices.push_back(vertex);

						vertex.p.set( (float)x, fHeight1+1.5f, (float)y );
						vertex.n = vNormal1;
						vertex.c = color1;
						vertex.uv.set( (float)fTexU,0.0f );
						setVertices.push_back(vertex);

						vertex.p.set( (float)(x+1), fHeight2+1.5f, (float)(y+1) );
						vertex.n = vNormal2;
						vertex.c = color2;
						vertex.uv.set( (float)fTexU+0.25f,0.0f );
						setVertices.push_back(vertex);

						vertex.p.set( (float)(x+1), fHeight2, (float)(y+1) );
						vertex.n = vNormal2;
						vertex.c = color2;
						vertex.uv.set( (float)fTexU+0.25f,1.0f );
						setVertices.push_back(vertex);

						bbox.vMin.x = min((float)x,bbox.vMin.x);
						bbox.vMin.y = min(fHeight1,bbox.vMin.y);
						bbox.vMin.z = min((float)y,bbox.vMin.z);

						bbox.vMax.x = max((float)(x+1),bbox.vMax.x);
						bbox.vMax.y = max(fHeight2+1.5f,bbox.vMax.y);
						bbox.vMax.z = max((float)(y+1),bbox.vMax.z);

						// 1	 2
						//	*---*
						//	| / |
						//	*---*
						// 0	 3
						int i = setVertices.size()-4;
						pMesh->getIndices().push_back(i);
						pMesh->getIndices().push_back(i+1);
						pMesh->getIndices().push_back(i+2);
						pMesh->getIndices().push_back(i);
						pMesh->getIndices().push_back(i+2);
						pMesh->getIndices().push_back(i+3);
					}
				}
			}
			// ----
			// # Subset
			// ----
			subset.vstart = 0;
 			subset.vbase = 0;
 			subset.istart += subset.icount;
 			subset.vcount = setVertices.size();
 			subset.icount = pMesh->getIndices().size()-subset.istart;
			pMesh->getSubsets().push_back(subset);
			// ----
			// # Material
			// ----
			char szGrassTexture[256];
			sprintf(szGrassTexture,"%s%s",GetParentPath(szFilename).c_str(),"TileGrass01.OZT");
			mat.strTexture[0] = szGrassTexture;
			mat.strShader = "terrainGrass";
			pMesh->getMaterials().resize(pMesh->getSubsets().size());
			pMesh->getMaterials()[pMesh->getSubsets().size()-1].push_back(mat);
			// ----
			// # Box
			// ----
			bbox.vMin.x = (float)startX;
			bbox.vMin.z = (float)startY;

			bbox.vMax.x = (float)(nEndX);
			bbox.vMax.z = (float)(nEndY);
			pMesh->setBBox(bbox);

			iRenderNode* pMeshNode = m_pRenderNodeMgr->createRenderNode("mesh");
			if (pMeshNode)
			{
				pMeshNode->setLocalBBox(bbox);
				pMeshNode->setData(pMesh);
				pRenderNode->addChild(pMeshNode);
			}
			//----
		}
	}
	return true;
}

bool CMyPlug::exportTerrainAtt(iSceneData * pSceneData, const std::string& strFilename)
{
	// Calc MU's map id from filename.
	int nMapID = getMapIDFromFilename(strFilename);
	// att
	// for server.att
	char szServerAttFile[256];
	sprintf(szServerAttFile,"%s(Server)Terrain%d.att",GetParentPath(strFilename).c_str(),nMapID);
	FILE* f=fopen(szServerAttFile,"wb+");
	if (f)
	{
		char buffer[ATT_FILE_SERVER_SIZE];
		char* p = buffer;
		*((unsigned char*)p)=0x0;++p;
		*((unsigned char*)p)=0xFF;++p;
		*((unsigned char*)p)=0xFF;++p;
		for (int y=0; y<253; ++y)
		{
			for (int x=0; x<253; ++x)
			{
				*p = pSceneData->getCellAttribute(x,y);
				p++;
			}
			for (int x=253; x<256; ++x)
			{
				*p =0;++p;
			}
		}
		for (int x=0; x<256*3; ++x)
		{
			*p =0;++p;
		}
		fwrite(buffer,ATT_FILE_SERVER_SIZE,1,f);
		fclose(f);
	}
	// EncTerrain.att
	std::string strEncTerrain=ChangeExtension(strFilename,".att");
	bool bAttFileSize128=true;
	{
		IOReadBase* pRead = IOReadBase::autoOpen(strEncTerrain);
		if (pRead)
		{
			bAttFileSize128 = (ATT_FILE_129KB_SIZE==pRead->GetSize());
			IOReadBase::autoClose(pRead);
		}
		else
		{
			bAttFileSize128 = (MessageBoxA(NULL,"Select the correct file size.\"OK\" as 128KB; \"Cancel\" as 64KB.",
				"Saving EncTerrain.att!",1)==1);
		}
	}
	f=fopen(strEncTerrain.c_str(),"wb+");
	if (f)
	{
		if (bAttFileSize128)
		{
			char buffer[ATT_FILE_129KB_SIZE];
			char* p = buffer;
			*((unsigned char*)p)=0x0;++p;
			*((unsigned char*)p)=nMapID;++p;
			*((unsigned char*)p)=0xFF;++p;
			*((unsigned char*)p)=0xFF;++p;
			{
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						*p = pSceneData->getCellAttribute(x,y);
						p++;
						*p =0;++p;
					}
					for (int x=253; x<256; ++x)
					{
						*p =0;++p;
						*p =0;++p;
					}
				}
				for (int x=0; x<256*6; ++x)
				{
					*p =0;++p;
				}
			}
			decrypt2(buffer,ATT_FILE_129KB_SIZE);
			encrypt(buffer,ATT_FILE_129KB_SIZE);
			fwrite(buffer,ATT_FILE_129KB_SIZE,1,f);
		}
		else
		{
			char buffer[ATT_FILE_65KB_SIZE];
			char* p = buffer;
			*((unsigned char*)p)=0x0;++p;
			*((unsigned char*)p)=nMapID;++p;
			*((unsigned char*)p)=0xFF;++p;
			*((unsigned char*)p)=0xFF;++p;
			{
				for (int y=0; y<253; ++y)
				{
					for (int x=0; x<253; ++x)
					{
						*p = pSceneData->getCellAttribute(x,y);
						p++;
					}
					for (int x=253; x<256; ++x)
					{
						*p =0;++p;
					}
				}
				for (int x=0; x<256*3; ++x)
				{
					*p =0;++p;
				}
			}
			decrypt2(buffer,ATT_FILE_65KB_SIZE);
			encrypt(buffer,ATT_FILE_65KB_SIZE);
			fwrite(buffer,ATT_FILE_65KB_SIZE,1,f);
		}
		fclose(f);
	}
	//////////////////////////////////////////////////////////////////////////
	f=fopen(ChangeExtension(strFilename,".64KB.att").c_str(),"wb+");
	if (f)
	{
		char buffer[ATT_FILE_65KB_SIZE];
		char* p = buffer;
		*((unsigned char*)p)=0x0;++p;
		*((unsigned char*)p)=nMapID;++p;
		*((unsigned char*)p)=0xFF;++p;
		*((unsigned char*)p)=0xFF;++p;
		{
			for (int y=0; y<253; ++y)
			{
				for (int x=0; x<253; ++x)
				{
					*p = pSceneData->getCellAttribute(x,y);
					p++;
				}
				for (int x=253; x<256; ++x)
				{
					*p =0;++p;
				}
			}
			for (int x=0; x<256*3; ++x)
			{
				*p =0;++p;
			}
		}
		decrypt2(buffer,ATT_FILE_65KB_SIZE);
		encrypt(buffer,ATT_FILE_65KB_SIZE);
		fwrite(buffer,ATT_FILE_65KB_SIZE,1,f);
		fclose(f);
	}

	f=fopen(ChangeExtension(strFilename,".128KB.att").c_str(),"wb+");
	if (f)
	{
		char buffer[ATT_FILE_129KB_SIZE];
		char* p = buffer;
		*((unsigned char*)p)=0x0;++p;
		*((unsigned char*)p)=nMapID;++p;
		*((unsigned char*)p)=0xFF;++p;
		*((unsigned char*)p)=0xFF;++p;
		{
			for (int y=0; y<253; ++y)
			{
				for (int x=0; x<253; ++x)
				{
					*p = pSceneData->getCellAttribute(x,y);
					p++;
					*p =0;++p;
				}
				for (int x=253; x<256; ++x)
				{
					*p =0;++p;
					*p =0;++p;
				}
			}
			for (int x=0; x<256*6; ++x)
			{
				*p =0;++p;
			}
		}
		decrypt2(buffer,ATT_FILE_129KB_SIZE);
		encrypt(buffer,ATT_FILE_129KB_SIZE);
		fwrite(buffer,ATT_FILE_129KB_SIZE,1,f);

		fclose(f);
	}
	return true;
}

bool CMyPlug::exportTerrainLightmap(iSceneData * pSceneData, const std::string& strFilename)
{
	return true;
}

bool CMyPlug::exportTerrainHeight(iSceneData * pSceneData, const std::string& strFilename)
{
	FILE* f=fopen(strFilename.c_str(),"wb+");
	if (f)
	{
		fseek(f,HEIGHT_HEAD_SIZE,SEEK_SET);
		char buffer[HEIGHT_BUFFER_SIZE];
		char* p = buffer;
		for (int y=0; y<254; ++y)
		{
			*p =0;++p;
			*p =0;++p;
			for (int x=0; x<254; ++x)
			{
				*p = max(min(pSceneData->getVertexHeight(x,y)/0.015f,255),0);
				p++;
			}
		}
		for (int x=0; x<256*2; ++x)
		{
			*p =0;++p;
		}
		fwrite(buffer,HEIGHT_BUFFER_SIZE,1,f);
		fclose(f);
	}
	return true;
}

bool CMyPlug::exportTerrainData(iSceneData * pSceneData, const std::string& strFilename)
{
	//////////////////////////////////////////////////////////////////////////
	exportTerrainAtt(pSceneData, strFilename);
	exportTerrainHeight(pSceneData, GetParentPath(strFilename)+"TerrainHeight.ozb");

	// Calc MU's map id from filename.
	int nMapID = getMapIDFromFilename(strFilename);

	// map
	FILE* f=fopen(strFilename.c_str(),"wb");
	if (f)
	{
		char buffer[MAP_FILE_SIZE];
		char* p = buffer;
		*((unsigned char*)p)=0;++p;
		*((unsigned char*)p)=nMapID;++p;
		// tile
		for (size_t layer=0;layer<2;++layer)
		{
			for (int y=0; y<253; ++y)
			{
				for (int x=0; x<253; ++x)
				{
					*p = pSceneData->getCellTileID(x,y,layer);
					p++;
				}
				*p =0;++p;
				*p =0;++p;
				*p =0;++p;
			}
			for (int x=0; x<256*3; ++x)
			{
				*p =0;++p;
			}
		}
		// alpha
		{
			for (int y=0; y<254; ++y)
			{
				for (int x=0; x<254; ++x)
				{
					*p = pSceneData->getVertexColor(x,y).a;
					p++;
				}
				*p =0;++p;
				*p =0;++p;
			}
			for (int x=0; x<256*2; ++x)
			{
				*p =0;++p;
			}
		}
		encrypt(buffer,MAP_FILE_SIZE);
		fwrite(buffer,MAP_FILE_SIZE,1,f);
		fclose(f);
	}

	return true;
}

bool CMyPlug::exportTiles(iSceneData * pTerrain, const std::string& strFilename, const std::string& strPath)
{
	return true;
}