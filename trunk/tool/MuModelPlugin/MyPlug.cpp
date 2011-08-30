#include "MyPlug.h"
#include "IORead.h"
#include "FileSystem.h"
#include "MUBmd.h"
#include "Material.h"
#include "Windows.h"

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

void importSkeletonBons(iSkeletonData& skeletonData, CMUBmd& bmd)
{
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
				if (uAnimID>=15&&uAnimID<=20)
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

			std::string strAnimName;
			{
				char szID[256]={0};
				itoa(uAnimID,szID,10);
				strAnimName=szID;
			}
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
			pRenderNode->init(pSkeletonData);
		}
	}
	else
	{
		iLodMesh* pMesh = (iLodMesh*)m_pRenderNodeMgr->getRenderData("mesh",szFilename);
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
				pMesh = (iLodMesh*)m_pRenderNodeMgr->createRenderData("mesh",szFilename);
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
					for (size_t i=0;  i<bmd.setBmdSub.size(); ++i)
					{
						CMUBmd::BmdSub& bmdSub = bmd.setBmdSub[i];
						CSubMesh& subMesh=pMesh->allotSubMesh();
						// ----
						// # Vertex Index
						// ----
						VertexIndex vertexIndex;
						for(std::vector<CMUBmd::BmdSub::BmdTriangle>::iterator it=bmdSub.setTriangle.begin(); it!=bmdSub.setTriangle.end(); it++)
						{
							for (size_t j=0; j<3; ++j)
							{
								vertexIndex.p	= it->indexVertex[2-j];
								vertexIndex.b	= it->indexVertex[2-j];
								vertexIndex.w	= it->indexVertex[2-j];
								vertexIndex.n	= it->indexNormal[2-j];
								vertexIndex.uv1	= it->indexUV[2-j];
								subMesh.m_setVertexIndex.push_back(vertexIndex);
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
									subMesh.addBone(0);// 想办法把bmd的骨骼ID都设置为0的去掉
								}
								else
								{
									subMesh.addBone(it->uBones);
								}
								//assert((it->uBones&0xFFFFFF00)==0);
								subMesh.addWeight(0x000000FF);
							}
							subMesh.addPos(vPos);
						}
						// ----
						// # Normal
						// ----
						for(std::vector<CMUBmd::BmdSub::BmdNormal>::iterator it=bmdSub.setNormal.begin(); it!=bmdSub.setNormal.end(); it++)
						{
							Vec3D n = fixCoordSystemNormal(it->vNormal);
							n = bmd.bmdSkeleton.getRotateMatrix(it->uBones)*n;
							subMesh.addNormal(n);
						}
						// ----
						// # Texcoord
						// ----
						for(std::vector<Vec2D>::iterator it=bmdSub.setUV.begin(); it!=bmdSub.setUV.end(); it++)
						{
							subMesh.addTexcoord(*it);
						}
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
							// # lvl 1-3
							// ----
							sprintf(szMaterialName,"%s%d",ChangeExtension(GetFilename(szFilename),".sub").c_str(),i);
							CMaterial* pMaterial = (CMaterial*)m_pRenderNodeMgr->createRenderData("material",szMaterialName);
							pMaterial->setTexture(0,strTexFileName.c_str());
							pMaterial->setShader("EngineRes\\fx\\diffuseAlphaTest128.fx");
							subMesh.addMaterial(szMaterialName);
							// ----
							// # lvl 4
							// ----
							sprintf(szMaterialName,"%s%d_4",ChangeExtension(GetFilename(szFilename),".sub").c_str(),i);
							pMaterial = (CMaterial*)m_pRenderNodeMgr->createRenderData("material",szMaterialName);
							pMaterial->setTexture(0,strTexFileName.c_str());
							pMaterial->setShader("EngineRes\\fx\\MuLvl4.fx");
							subMesh.addMaterial(szMaterialName);
							// ----
							// # lvl 7
							// ----
							sprintf(szMaterialName,"%s%d_7",ChangeExtension(GetFilename(szFilename),".sub").c_str(),i);
							pMaterial = (CMaterial*)m_pRenderNodeMgr->createRenderData("material",szMaterialName);
							pMaterial->setTexture(0,strTexFileName.c_str());
							pMaterial->setShader("EngineRes\\fx\\MuLvl7.fx");
							subMesh.addMaterial(szMaterialName);
						}
					}
					pMesh->setBBox(bbox);
					pMesh->init();
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
				pRenderNode->init(pSkeletonData);
				//----
				iRenderNode* pMeshNode = m_pRenderNodeMgr->createRenderNode("mesh");
				if (pMeshNode)
				{
					pMeshNode->init(pMesh);
				}
				//----
				pRenderNode->addChild(pMeshNode);
			}
		}
		else if (pRenderNode->getType()==iRenderNode::NODE_MESH)
		{
			pRenderNode->init(pMesh);
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
		std::string strMatFilename = strMyPath+GetFilename(ChangeExtension(szFilename,".mat.csv"));
		std::string strParFilename = strMyPath+GetFilename(ChangeExtension(szFilename,".par.csv"));
		if (!IOReadBase::Exists(strMatFilename))
		{
			strMatFilename=strMyPath+strParentDirName+".mat.csv";
		}
		if (!IOReadBase::Exists(strParFilename))
		{
			strParFilename=strMyPath+strParentDirName+".par.csv";
		}
		m_pRenderNodeMgr->loadRenderNode(strMatFilename.c_str(),pRenderNode);
		m_pRenderNodeMgr->loadRenderNode(strParFilename.c_str(),pRenderNode);
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