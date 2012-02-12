#include "MyPlug.h"
#include "IORead.h"
#include "FileSystem.h"
#include "MUBmd.h"
#include "Material.h"
#include "Windows.h"
#include "IniFile.h"
#include <map>

extern "C" __declspec(dllexport) bool Data_Plug_CreateObject(void ** pobj){
	*pobj = new CMyPlug;
	return *pobj != NULL;
}

CMyPlug::CMyPlug(void)
{
}

CMyPlug::~CMyPlug(void)
{
}

template <class _T, class _T2>
void  transformRedundance(const std::vector<_T>& setIn, std::vector<_T>& setOut, std::vector<_T2>& index)
{
	std::map<_T,int> mapSort;
	size_t start = setOut.size();
	for (auto it=setIn.begin();it!=setIn.end();it++)
	{
		mapSort[*it];
	}
	int id = setOut.size();
	for (auto it=mapSort.begin();it!=mapSort.end();it++)
	{
		setOut.push_back(it->first);
		it->second=id;
		id++;
	}
	for (auto it=setIn.begin();it!=setIn.end();it++)
	{
		index.push_back(mapSort[*it]);
	}
}

#include <fstream>

void importSkeletonBons(iSkeletonData& skeletonData, CMUBmd& bmd)
{
	if (bmd.bmdSkeleton.setBmdBone.size()>50)
	{
		int id=0;
		std::ofstream file3("D:\Skeleton.txt");//以输出方式打开文件
		for (std::vector<CMUBmd::BmdSkeleton::BmdBone>::iterator itBmdBone=bmd.bmdSkeleton.setBmdBone.begin();itBmdBone!=bmd.bmdSkeleton.setBmdBone.end();itBmdBone++)
		{
			file3<<id<<","<<itBmdBone->szName<<"\n";
			id++;
		}
	}
	for (std::vector<CMUBmd::BmdSkeleton::BmdBone>::iterator itBmdBone=bmd.bmdSkeleton.setBmdBone.begin();itBmdBone!=bmd.bmdSkeleton.setBmdBone.end();itBmdBone++)
	{
		iBoneInfo* pBoneInfo = skeletonData.allotBoneInfo();
		if (pBoneInfo && !itBmdBone->bEmpty)
		{
			Matrix	mInvLocal = itBmdBone->mLocal;
			mInvLocal.Invert();
			int nParent = itBmdBone->nParent;
			if (nParent<0||nParent>255)
			{
				nParent = 255;
			}
			// ----
			pBoneInfo->setName(itBmdBone->szName);
			pBoneInfo->setInvLocal(mInvLocal);
			pBoneInfo->setParent(nParent);
		}
	}
}

void importSkeletonAnims(iSkeletonData& skeletonData, CMUBmd& bmd)
{
	if (bmd.nFrameCount>1)// if there one frame only, free the animlist
	{
		int nFrameCount = 0;
		for (size_t uAnimID=0; uAnimID<bmd.head.uAnimCount; ++uAnimID)
		{
			bool bFixFrame = true;
			bool bFixMove = false;
			if (bmd.head.uAnimCount>100) // player
			{
				if ((uAnimID>=15&&uAnimID<=20)
					||(uAnimID>=25&&uAnimID<=30))
				{
					bFixMove=true;
					bFixFrame=false;
				}
				//if (uAnimID==2||uAnimID==6||uAnimID>8)
				//{
				//	bFixFrame=false;
				//}
			}
			else
			{
				if (uAnimID==2) // monster 3
				{
					bFixMove=true;
					bFixFrame=false;
				}
				if (uAnimID==6) // die
				{
					bFixFrame=false;
				}
			}
			long uTotalFrames = bmd.bmdSkeleton.setBmdAnim[uAnimID].uFrameCount;
			
			long timeEnd = nFrameCount;

			char szTemp[256]={0};
			std::string strAnimName = itoa(uAnimID,szTemp,10);

			/*switch(uAnimID)
			{
			case 0:strAnimName+="Idle0";
				break;
			case 1:strAnimName+="Idle1";
				break;
			case 2:strAnimName+="Walk";
				break;
			case 3:strAnimName+="Attack0";
				break;
			case 4:strAnimName+="Attack1";
				break;
			case 5:strAnimName+="Hurt";
				break;
			case 6:strAnimName+="Dead";
				break;
			default:strAnimName+="Unknown";
				break;
			}*/
			
			std::vector<CMUBmd::BmdSkeleton::BmdBone>& setBmdBone = bmd.bmdSkeleton.setBmdBone;
			size_t uBoneSize = setBmdBone.size();
			iSkeletonAnim* pSkeletonAnim = skeletonData.allotAnimation(strAnimName);
			if(NULL==pSkeletonAnim)
			{
				// do some thing!
			}
			// ----
			// # Set Anim Name
			// ----
			pSkeletonAnim->setName(strAnimName.c_str());
			// ----
			std::vector<BoneAnim>& setBonesAnim = pSkeletonAnim->setBonesAnim;
			setBonesAnim.resize(uBoneSize);
			for (size_t uBoneID = 0;uBoneID<uBoneSize;++uBoneID)
			{
				CMUBmd::BmdSkeleton::BmdBone& bmdBone=setBmdBone[uBoneID];
				if (!bmdBone.bEmpty)
				{
					BoneAnim& bonsAnim = setBonesAnim[uBoneID];
					for (size_t i=0;i<uTotalFrames;++i)
					{
						bonsAnim.trans.addValue(i*MU_BMD_ANIM_FRAME_TIME,fixCoordSystemPos(bmdBone.setTrans[i+nFrameCount]));// 可以设置关键帧播放速度来调控
						bonsAnim.rot.addValue(i*MU_BMD_ANIM_FRAME_TIME,fixCoordSystemRotate(bmdBone.setRotate[i+nFrameCount]));
					}
					if (bFixFrame) // fuck here
					{
						// 补帧
						bonsAnim.trans.addValue(uTotalFrames*MU_BMD_ANIM_FRAME_TIME,fixCoordSystemPos(bmdBone.setTrans[nFrameCount]));
						bonsAnim.rot.addValue(uTotalFrames*MU_BMD_ANIM_FRAME_TIME,fixCoordSystemRotate(bmdBone.setRotate[nFrameCount]));
					}
				}
			}
			// Fix Move
			if (bFixMove)
			{
				BoneAnim& bonsAnim = setBonesAnim[0];
				size_t uSize = bonsAnim.trans.m_KeyData.size();
				float fMoveLength = bonsAnim.trans.m_KeyData[uSize-1].z-bonsAnim.trans.m_KeyData[0].z;
				for (size_t i=0;i<uSize;++i)
				{
					bonsAnim.trans.m_KeyData[i].z-=(float)i/(float)(uTotalFrames-1)*fMoveLength;
				}
			}
			nFrameCount+=uTotalFrames;
			if (bFixFrame) // fuck here
			{
				// 补帧
				pSkeletonAnim->setTotalFrames(uTotalFrames*MU_BMD_ANIM_FRAME_TIME);
			}
			else
			{
				pSkeletonAnim->setTotalFrames((uTotalFrames-1)*MU_BMD_ANIM_FRAME_TIME);
			}
		}
	}
}

bool CMyPlug::importData(iRenderNode* pRenderNode, const char* szFilename)
{
	static CMUBmd* pPlayerBmd;
	if (GetFilename(szFilename)=="player.bmd")
	{
		if (pPlayerBmd==NULL)
		{
			pPlayerBmd = new CMUBmd;
			if (!pPlayerBmd->LoadFile(szFilename))
			{
				return false;
			}
		}
		if (pRenderNode->getType()==iRenderNode::NODE_SKELETON)
		{
			iSkeletonData*	pSkeletonData	= (iSkeletonData*)	m_pRenderNodeMgr->getRenderData("skeleton",szFilename);
			if (pSkeletonData==NULL)
			{
				pSkeletonData = (iSkeletonData*)m_pRenderNodeMgr->createRenderData("skeleton",szFilename);
				if (pSkeletonData)
				{
					//m_Mesh.m_Lods.resize(1);
					importSkeletonBons(*pSkeletonData,*pPlayerBmd);
					importSkeletonAnims(*pSkeletonData,*pPlayerBmd);
				}
				else
				{
					// createSkeletonData error!
				}
			}
			pRenderNode->setData(pSkeletonData);
		}
	}
	else
	{
		iMeshData* pMesh = (iMeshData*)m_pRenderNodeMgr->getRenderData("mesh",szFilename);
		// ----
		if (pMesh==NULL)
		{
			CMUBmd bmd;
			if (!bmd.LoadFile(szFilename))
			{
				return false;
			}
			// ----
			if (pMesh==NULL)
			{
				pMesh = (iMeshData*)m_pRenderNodeMgr->createRenderData("mesh",szFilename);
				if (pMesh)
				{
					bool bIsPlayerPart = false;
					if (GetFilename(szFilename)!="player.bmd")
					{
						if (GetFilename(GetParentPath(szFilename))=="player")
						{
							bIsPlayerPart= true;
						}
					}
					if (bIsPlayerPart&&pPlayerBmd==NULL)
					{
						pPlayerBmd = new CMUBmd;
						bIsPlayerPart = pPlayerBmd->LoadFile(GetParentPath(szFilename)+"player.bmd");
					}
					// ----
					// # Mesh
					// ----
					BBox bbox;

					IndexedSubset subset;

					std::vector<unsigned short>& setIndex = pMesh->getIndices();
					pMesh->getMaterials().resize(bmd.setBmdSub.size());
					for (size_t i=0;  i<bmd.setBmdSub.size(); ++i)
					{
						CMUBmd::BmdSub& bmdSub = bmd.setBmdSub[i];

						std::vector<Vec3D>			pos;
						std::vector<unsigned long>	weight;
						std::vector<unsigned long>	bone;
						std::vector<Vec3D>			normal;
						std::vector<Vec2D>			texcoord;
						std::vector<VertexIndex>	setVecVertexIndex;
						std::vector<VertexIndex>	face;

						// ----
						// # Vertex Index
						// ----
						VertexIndex vertexIndex;
						for(std::vector<CMUBmd::BmdSub::BmdTriangle>::iterator it=bmdSub.setTriangle.begin(); it!=bmdSub.setTriangle.end(); it++)
						{
							for (size_t j=0; j<3; ++j)
							{
								vertexIndex.p	= pos.size()+it->indexVertex[2-j];
								vertexIndex.b	= bone.size()+it->indexVertex[2-j];
								vertexIndex.w	= weight.size()+it->indexVertex[2-j];
								vertexIndex.n	= normal.size()+it->indexNormal[2-j];
								vertexIndex.uv1	= texcoord.size()+it->indexUV[2-j];
								face.push_back(vertexIndex);
							}
						}
						// ----
						// # Pos
						// ----
						for(std::vector<CMUBmd::BmdSub::BmdPos>::iterator it=bmdSub.setVertex.begin(); it!=bmdSub.setVertex.end(); it++)
						{
							Vec3D vPos = fixCoordSystemPos(it->vPos);
							if (bIsPlayerPart)
							{
								vPos = pPlayerBmd->bmdSkeleton.getLocalMatrix(it->uBones)*vPos;
							}
							else
							{
								vPos = bmd.bmdSkeleton.getLocalMatrix(it->uBones)*vPos;
							}
							bbox.vMin.x = min(vPos.x,bbox.vMin.x);
							bbox.vMin.y = min(vPos.y,bbox.vMin.y);
							bbox.vMin.z = min(vPos.z,bbox.vMin.z);

							bbox.vMax.x = max(vPos.x,bbox.vMax.x);
							bbox.vMax.y = max(vPos.y,bbox.vMax.y);
							bbox.vMax.z = max(vPos.z,bbox.vMax.z);
							if (1<bmd.nFrameCount||bIsPlayerPart)
							{
								unsigned char uBone = it->uBones&0xFF;
								if (bmd.bmdSkeleton.setBmdBone.size()<=uBone||bmd.bmdSkeleton.setBmdBone[uBone].bEmpty)
								{
									bone.push_back(0);// 想办法把bmd的骨骼ID都设置为0的去掉
								}
								else
								{
									bone.push_back(it->uBones);
								}
								//assert((it->uBones&0xFFFFFF00)==0);
								weight.push_back(0x000000FF);
							}
							pos.push_back(vPos);
						}
						// ----
						// # Normal
						// ----
						for(std::vector<CMUBmd::BmdSub::BmdNormal>::iterator it=bmdSub.setNormal.begin(); it!=bmdSub.setNormal.end(); it++)
						{
							Vec3D n = fixCoordSystemNormal(it->vNormal);
							n = bmd.bmdSkeleton.getRotateMatrix(it->uBones)*n;
							normal.push_back(n);
						}
						// ----
						// # Texcoord
						// ----
						for(std::vector<Vec2D>::iterator it=bmdSub.setUV.begin(); it!=bmdSub.setUV.end(); it++)
						{
							texcoord.push_back(*it);
						}
						// ----
 						transformRedundance(face,setVecVertexIndex,setIndex);

 						subset.vstart = 0;	// 因为每个sub都是独立的ib索引编号 从0开始，所以只需要设置vbase（vb地址偏移），有些显卡在误设置vstart（IB范围后）会不显示。
 						subset.vbase += subset.vcount;
 						subset.istart += subset.icount;
 						subset.vcount = setVecVertexIndex.size();
 						subset.icount = setIndex.size()-subset.istart;
						pMesh->getSubsets().push_back(subset);

						if (1<bmd.nFrameCount||bIsPlayerPart)
						{
							for (size_t n=0;n<setVecVertexIndex.size();++n)
							{
								VertexIndex& vertexIndex=setVecVertexIndex[n];
								SkinVertex vertex;
								vertex.p = pos[vertexIndex.p];
								vertex.n = normal[vertexIndex.n];
								vertex.uv = texcoord[vertexIndex.uv1];
								vertex.w4 = weight[vertexIndex.w];
								vertex.b4 = bone[vertexIndex.b];
								vertex.w4 = weight[vertexIndex.w];
								vertex.b4 = bone[vertexIndex.b];
								pMesh->getSkinVertices().push_back(vertex);
							}
						}
						else
						{
							for (size_t n=0;n<setVecVertexIndex.size();++n)
							{
								VertexIndex& vertexIndex=setVecVertexIndex[n];
								RigidVertex vertex;
								vertex.p = pos[vertexIndex.p];
								vertex.n = normal[vertexIndex.n];
								vertex.uv = texcoord[vertexIndex.uv1];
								pMesh->getRigidVertices().push_back(vertex);
							}
						}
// 					}
// 					transformRedundance(setIndex,m_Lods[0].IndexLookup,m_Lods[0].Indices);
						// ----
						// # Material
						// ----
						char szMaterialName[255];
						{
							std::string strTexFileName = GetParentPath(szFilename) + bmdSub.szTexture;
							{
								std::string strExt = GetExtension(bmdSub.szTexture); 
								if (".jpg"==strExt)			strExt = ".ozj";
								else if (".tga"==strExt)	strExt = ".ozt";
								else if (".bmp"==strExt)	strExt = ".ozb";
								strTexFileName = ChangeExtension(strTexFileName,strExt);
							}
							// ----
							// # lvl 0-2
							// ----
							CMaterial mat;
							mat.strTexture[0] = strTexFileName;
							mat.strShader="diffuseAlphaTest128";
							pMesh->getMaterials()[i].push_back(mat);
							// ----
							// # lvl 3-4
							// ----
							mat.strShader="EquipEffect1";
							pMesh->getMaterials()[i].push_back(mat);
							// ----
							// # lvl 5-6
							// ----
							mat.strShader="EquipEffect2";
							pMesh->getMaterials()[i].push_back(mat);
							// ----
							// # lvl 7-
							// ----
							mat.strShader="EquipEffect3";
							pMesh->getMaterials()[i].push_back(mat);
						}
					}
					pMesh->setBBox(bbox);
				}
				else
				{
					// createLodMesh error!
				}
			}
			// ----
			if (pRenderNode->getType()==iRenderNode::NODE_SKELETON)
			{
				iSkeletonData*	pSkeletonData	= (iSkeletonData*)	m_pRenderNodeMgr->getRenderData("skeleton",szFilename);
				if (pSkeletonData==NULL)
				{
					pSkeletonData = (iSkeletonData*)m_pRenderNodeMgr->createRenderData("skeleton",szFilename);
					if (pSkeletonData)
					{
						//m_Mesh.m_Lods.resize(1);
						importSkeletonBons(*pSkeletonData,bmd);
						importSkeletonAnims(*pSkeletonData,bmd);
					}
					else
					{
						// createSkeletonData error!
					}
				}
			}
		}
		// ----
		if (pRenderNode->getType()==iRenderNode::NODE_SKELETON)
		{
			iSkeletonData*	pSkeletonData	= (iSkeletonData*)	m_pRenderNodeMgr->getRenderData("skeleton",szFilename);
			if (pSkeletonData)
			{
				pRenderNode->setData(pSkeletonData);
				//----
				iRenderNode* pMeshNode = m_pRenderNodeMgr->createRenderNode("mesh");
				if (pMeshNode)
				{
					pMeshNode->setData(pMesh);
				}
				//----
				pRenderNode->addChild(pMeshNode);
			}
		}
		else if (pRenderNode->getType()==iRenderNode::NODE_MESH)
		{
			pRenderNode->setData(pMesh);
		}
		else
		{
			// ----error type
		}
		// 	if (pSkeletonData==NULL || pMesh==NULL)
		// 	{
		// 		// big error , need delete data!!
		// 		return false;
		// 	}
		// ----
		std::string strParentDir = GetParentPath(szFilename);
		std::string strParentDirName = GetFilename(strParentDir);

		std::string strMyPath ="Plugins\\Data\\"+strParentDirName+"\\";
		std::string strMatFilename = strMyPath+GetFilename(ChangeExtension(szFilename,".mat"));
		if (!IOReadBase::Exists(strMatFilename))
		{
			strMatFilename=strMyPath+strParentDirName+".mat";
		}
		// set material
		{
			CIniFile ini;
			if (ini.readIniFile(strMatFilename.c_str()))
			{
				for (auto itSection=ini.m_isSection.begin(); itSection!=ini.m_isSection.end(); ++itSection)
				{
					if (itSection->m_strName.length()<2)
					{
						continue;
					}

					switch (itSection->m_strName[0])
					{
					case 'm':
						{
							int matID = atoi(&itSection->m_strName[1]);
							if (matID<0||matID>=pMesh->getMaterials().size())
							{
								continue;
							}
							auto& mats = pMesh->getMaterials()[matID];
							if (mats.size()<=0)
							{
								mats.resize(1);
							}
							auto& mat = mats[0];
							for (auto iteEntry=itSection->m_ieEntry.begin(); iteEntry!=itSection->m_ieEntry.end(); ++iteEntry)
							{
								if (iteEntry->m_strName=="shader") mat.strShader=iteEntry->m_strValue;
								else if (iteEntry->m_strName=="s0") mat.strTexture[0]=iteEntry->m_strValue;
								else if (iteEntry->m_strName=="s1") mat.strTexture[1]=iteEntry->m_strValue;
								else if (iteEntry->m_strName=="s2") mat.strTexture[2]=iteEntry->m_strValue;
								else if (iteEntry->m_strName=="s3") mat.strTexture[3]=iteEntry->m_strValue;
								else if (iteEntry->m_strName=="s4") mat.strTexture[4]=iteEntry->m_strValue;
								else if (iteEntry->m_strName=="s5") mat.strTexture[5]=iteEntry->m_strValue;
								else if (iteEntry->m_strName=="s6") mat.strTexture[6]=iteEntry->m_strValue;
								else if (iteEntry->m_strName=="s7") mat.strTexture[7]=iteEntry->m_strValue;
							}
						}
						break;

					case 'p':
						{
							iRenderNode* pParticleRenderNode = (iRenderNode*)m_pRenderNodeMgr->createRenderNode("particle");
							if (pParticleRenderNode)
							{
								int nBone;
								for (auto iteEntry=itSection->m_ieEntry.begin(); iteEntry!=itSection->m_ieEntry.end(); ++iteEntry)
								{
									if (iteEntry->m_strName=="bone")
									{
										pParticleRenderNode->setBindingBoneID(atoi(iteEntry->m_strValue.c_str()));
									}
									else if (iteEntry->m_strName=="pos")
									{
										Vec3D vPos;
										sscanf(iteEntry->m_strValue.c_str(), "%f,%f,%f", &vPos.x, &vPos.y, &vPos.z);
										pParticleRenderNode->setPos(vPos);
									}
									else if (iteEntry->m_strName=="par")
									{
										std::string strParFilanem = iteEntry->m_strValue;
										if (!IOReadBase::Exists(strParFilanem))
										{
											strParFilanem=strMyPath+strParFilanem;
										}
										pParticleRenderNode->setFilename(strParFilanem.c_str());
									}
								}
								pRenderNode->addChild(pParticleRenderNode);
							}
						}
						break;
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//iRenderNode* pRenderNode = m_pRenderNodeMgr->createRenderNode("Skeleton");
	}
	return true;
}
/*
bool CMyPlug::exportData(iModelData * pModelData, const std::string& szFilename)
{
	assert(pModelData);
	FILE* fp= fopen(szFilename.c_str(),"wb");
	if (fp==NULL)
	{
		return false;
	}
	// tag
	unsigned long uTag=0x0a444d42;
	fwrite(&uTag,4,1,fp);
	// head
	CMUBmd::BmdHead bmdHead;
	strcpy(bmdHead.strFile,GetFilename(pModelData->getItemName()).c_str());
	bmdHead.uSubCount=pModelData->getMesh().getSubCount();
	bmdHead.uBoneCount=pModelData->getSkeleton().m_Bones.size();
	bmdHead.uAnimCount=pModelData->getSkeleton().m_Bones.size();
	fwrite(&bmdHead,sizeof(CMUBmd::BmdHead),1,fp);
	// sub
	for (size_t i=0; i<bmdHead.uSubCount; ++i)
	{
		//size_t uFaceIndexCount=getFaceIndexCount(i);
		//// sub head
		//CMUBmd::BmdSub::BmdSubHead subHead;
		//subHead.uVertexCount=uFaceIndexCount*3;
		//subHead.uNormal=uFaceIndexCount*3;
		//subHead.uUVCount=uFaceIndexCount*3;
		//subHead.uTriangleCount=uFaceIndexCount*3;

		//fwrite(&bmdHead,sizeof(CMUBmd::BmdHead),1,fp);
		//for (size_t n=0;n<uFaceIndexCount;++n)
		//{
		//	if (getFaceIndex(i,n,faceIndex))
		//	{
		//	}
		//}
		//setBmdSub[i].load(s);
	}

	return true;
}
*/
void CMyPlug::release()
{
	delete this;
}