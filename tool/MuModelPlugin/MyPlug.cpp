#include "MyPlug.h"
#include "IORead.h"
#include "FileSystem.h"
#include "MUBmd.h"
#include "Material.h"

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
						bonsAnim.trans.addValue(i*MU_BMD_ANIM_FRAME_TIME,fixCoordSystemPos(bmdBone.setTrans[i+nFrameCount]));// �������ùؼ�֡�����ٶ�������
						bonsAnim.rot.addValue(i*MU_BMD_ANIM_FRAME_TIME,fixCoordSystemRotate(bmdBone.setRotate[i+nFrameCount]));
					}
					if (bFixFrame) // fuck here
					{
						// ��֡
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
				// ��֡
				pSkeletonAnim->setTotalFrames(uTotalFrames*MU_BMD_ANIM_FRAME_TIME);
			}
			else
			{
				pSkeletonAnim->setTotalFrames((uTotalFrames-1)*MU_BMD_ANIM_FRAME_TIME);
			}
		}
	}
}

iRenderNode* CMyPlug::importData(iRenderNodeMgr* pRenderNodeMgr, const char* szFilename)
{
	iSkeletonData* pSkeletonData = pRenderNodeMgr->getSkeletonData(szFilename);
	iLodMesh* pMesh = pRenderNodeMgr->getLodMesh(szFilename);
	// ----
	if (pSkeletonData==NULL || pMesh==NULL)
	{
		CMUBmd bmd;
		if (!bmd.LoadFile(szFilename))
		{
			return false;
		}
		// ----
		if (pMesh==NULL)
		{
			pMesh = pRenderNodeMgr->createLodMesh(szFilename);
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
				static CMUBmd* pPlayerBmd;
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
								subMesh.addBone(0);// ��취��bmd�Ĺ���ID������Ϊ0��ȥ��
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
						sprintf(szMaterialName,"%s%d",ChangeExtension(GetFilename(szFilename),".sub").c_str(),i);
						CMaterial& material = *pRenderNodeMgr->createMaterial(szMaterialName);
						std::string strTexFileName = GetParentPath(szFilename) + bmdSub.szTexture;
						{
							std::string strExt = GetExtension(bmdSub.szTexture); 
							if (".jpg"==strExt)			strExt = ".ozj";
							else if (".tga"==strExt)	strExt = ".ozt";
							else if (".bmp"==strExt)	strExt = ".ozb";
							strTexFileName = ChangeExtension(strTexFileName,strExt);
						}
						material.setTexture(0,strTexFileName.c_str());

						material.bLightingEnabled = true;
						// ----
						material.uCull					= CULL_NONE;
						// ----
						material.bBlend					= false;
						// ----
						material.bAlphaTest				= true;
						material.nAlphaTestCompare		= CMPF_GREATER_EQUAL;
						material.uAlphaTestValue		= 0x80;
						// ----
						material.bDepthTest				= true;
						material.bDepthWrite			= true;
						// ----
						CMaterial::TextureOP& texOP0	= material.textureOP[0];
						CMaterial::TextureOP& texOP1	= material.textureOP[1];
						// ----
						texOP0.nColorOP					= TBOP_MODULATE;
						texOP0.nColorSrc1				= TBS_CURRENT;
						texOP0.nColorSrc2				= TBS_TEXTURE;
						texOP0.nAlphaOP					= TBOP_MODULATE;
						texOP0.nAlphaSrc1				= TBS_CURRENT;
						texOP0.nAlphaSrc2				= TBS_TEXTURE;
						// ----
						texOP1.nColorOP					= TBOP_DISABLE;
						texOP1.nAlphaOP					= TBOP_DISABLE;
					}
					subMesh.setMaterial(szMaterialName);
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
		if (pSkeletonData==NULL)
		{
			pSkeletonData = pRenderNodeMgr->createSkeletonData(szFilename);
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
	if (pSkeletonData==NULL || pMesh==NULL)
	{
		// big error , need delete data!!
		return NULL;
	}

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
	pRenderNodeMgr->loadRenderNode(strMatFilename.c_str());
	pRenderNodeMgr->loadRenderNode(strParFilename.c_str());
	//////////////////////////////////////////////////////////////////////////
	iRenderNode* pSkeletonNode = pRenderNodeMgr->createRenderNode("Skeleton");
	pSkeletonNode->init(pSkeletonData);
	//----
	iRenderNode* pMeshNode = pRenderNodeMgr->createRenderNode("Mesh");
	pMeshNode->init(pMesh);
	//----
	pSkeletonNode->addChild(pMeshNode);
	return pSkeletonNode;
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