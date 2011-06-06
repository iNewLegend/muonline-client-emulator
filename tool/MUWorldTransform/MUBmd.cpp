#include "MUBmd.h"
#include "DecryptFuncs.h"
#include "FileSystem.h"
#include <algorithm>

Matrix CMUBmd::BmdSkeleton::getLocalMatrix(unsigned char uBoneID)
{
	if (setBmdBone.size()>uBoneID)
	{
		return setBmdBone[uBoneID].mLocal;
	}
	return Matrix::UNIT;
}

Matrix CMUBmd::BmdSkeleton::getRotateMatrix(unsigned char uBoneID)
{
	Matrix mRotate=getLocalMatrix(uBoneID);
	mRotate._14=0;mRotate._24=0;mRotate._34=0;
	return mRotate;
}

//左右手转换就是 z轴负一下
//Vec3D(v.x, v.y, -v.z)
//q.rotate(Vec3D(v.x,v.y,-v.z));
//
//然后剩下的就是 向上的问题
//mu是z向上， 换到我那里 就得y向上
//Vec3D(v.x, -v.z, v.y)
//Quaternion(q.x, -q.z, q.y, q.w)
//
//俩一合 就是
//Vec3D(v.x, v.z, v.y)
//
//Quaternion q;
//q.rotate(Vec3D(v.x,v.y,-v.z));
//return Quaternion(q.x, -q.z, q.y, q.w);

inline Vec3D fixCoordSystemPos(const Vec3D& v)
{
	return Vec3D(v.x, v.y, -v.z);
}
inline Vec3D fixCoordSystemNormal(const Vec3D& v)
{
	return Vec3D(v.x, v.y, -v.z);
}

inline Vec3D fixCoordSystemRotate(const Vec3D& v)
{
	return Vec3D(v.x,v.y,-v.z);
}

inline Quaternion fixCoordSystemRotate2(const Vec3D& v)
{
	Quaternion q;
	q.rotate(Vec3D(v.x,v.y,-v.z));
	return Quaternion(q);
}

void CMUBmd::BmdSkeleton::calcLocalMatrix(unsigned long uBoneID)
{
	//m_bCalc
	if (setBmdBone.size()<=uBoneID)
	{
		return;
	}
	BmdBone& b = setBmdBone[uBoneID];
	Matrix m = Matrix::UNIT;
	if (b.setTrans.size()>0)
	{
		m *= Matrix::newTranslation(b.setTrans[0]);
	}
	if (b.setRotate.size()>0)
	{
		Quaternion q;
		q.rotate(b.setRotate[0]);
		m *= Matrix::newQuatRotate(q);
	}
	if (b.nParent==-1)
	{
		b.mLocal = m;
	}
	else
	{
		calcLocalMatrix(b.nParent);
		b.mLocal = getLocalMatrix(b.nParent)*m;
	}
}

void CMUBmd::BmdSkeleton::getLocalMatrix(std::vector<Matrix>& setLocalMatrix)
{
	setLocalMatrix.resize(setBmdBone.size());
	for (size_t i=0;i<setBmdBone.size();++i)
	{
		calcLocalMatrix(i);
		setLocalMatrix[i]=setBmdBone[i].mLocal;
	}
}

void CMUBmd::BmdSub::skinMesh(std::vector<Matrix>& setBoneMatrix)
{
	for (size_t i=0;i<setVertex.size();++i)
	{
		setVertex[i].vPos=setBoneMatrix[setVertex[i].uBones]*setVertex[i].vPos;
	}
	for (size_t i=0;i<setNormal.size();++i)
	{
		Matrix mRotate=setBoneMatrix[setNormal[i].uBones];
		mRotate._14=0;mRotate._24=0;mRotate._34=0;
		setNormal[i].vNormal=mRotate*setNormal[i].vNormal;
	}
}

bool CMUBmd::loadFormBmd(const std::string& strFilename)
{
	IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
	if (!pRead)
	{
		return false;
	}
	size_t uFileSize = pRead->GetSize();
	CMemoryStream s;
	s.resize(uFileSize);

	pRead->Read(s.getBuffer(),uFileSize);
	IOReadBase::autoClose(pRead);

	unsigned long uTag;
	s.read(uTag);
	if (0x0a444d42==uTag)//BMD.
	{
	}
	else if (0x0c444d42==uTag)//BMD.
	{
		unsigned long uEncodeSize;
		s.read(uEncodeSize);
		if (uEncodeSize!=uFileSize-8)
		{
			return false;
		}
		decryptMuBuffer(s.getPt(), uFileSize-8);
	}
	else 
	{
		return false;
	}
	// Head
	s.read(head);
	assert(head.uSubCount<50);
	// Sub
	setBmdSub.resize(head.uSubCount);
	for (size_t uSubID=0; uSubID<setBmdSub.size(); ++uSubID)
	{
		BmdSub& bmdSub=setBmdSub[uSubID];

		s.read(bmdSub.head);
		s.readVector(bmdSub.setVertex,bmdSub.head.uVertexCount);
		s.readVector(bmdSub.setNormal,bmdSub.head.uNormal);
		s.readVector(bmdSub.setUV,bmdSub.head.uUVCount);
		s.readVector(bmdSub.setTriangle,bmdSub.head.uTriangleCount);
		s.read((unsigned char*)bmdSub.szTexture,32);

		for (size_t i=0;i<bmdSub.setVertex.size();++i)
		{
			bmdSub.setVertex[i].vPos=fixCoordSystemPos(bmdSub.setVertex[i].vPos);
		}
		for (size_t i=0;i<bmdSub.setNormal.size();++i)
		{
			bmdSub.setNormal[i].vNormal=fixCoordSystemNormal(bmdSub.setNormal[i].vNormal);
		}
	}
	//Skeleton
	bmdSkeleton.setBmdAnim.resize(head.uAnimCount);
	for (size_t i=0; i<head.uAnimCount;++i)
	{
		s.read(bmdSkeleton.setBmdAnim[i].uFrameCount);
		s.read(bmdSkeleton.setBmdAnim[i].bOffset);
		if (bmdSkeleton.setBmdAnim[i].bOffset)
		{
			s.readVector(bmdSkeleton.setBmdAnim[i].vOffset,bmdSkeleton.setBmdAnim[i].uFrameCount);
		}
	}
	bmdSkeleton.setBmdBone.resize(head.uBoneCount);
	for (size_t uBoneID=0; uBoneID<head.uBoneCount;++uBoneID)
	{
		s.read(bmdSkeleton.setBmdBone[uBoneID].bEmpty);
		if (bmdSkeleton.setBmdBone[uBoneID].bEmpty)
		{
			continue;
		}
		s.read((unsigned char*)bmdSkeleton.setBmdBone[uBoneID].szName,32);
		s.read(bmdSkeleton.setBmdBone[uBoneID].nParent);
		for (size_t uAnimID=0; uAnimID<head.uAnimCount;++uAnimID)
		{
			for (size_t j=0; j<bmdSkeleton.setBmdAnim[uAnimID].uFrameCount;++j)
			{
				Vec3D vTrans;
				s.read(vTrans);
				bmdSkeleton.setBmdBone[uBoneID].setTrans.push_back(fixCoordSystemPos(vTrans));
			}
			for (size_t j=0; j<bmdSkeleton.setBmdAnim[uAnimID].uFrameCount;++j)
			{
				Vec3D vRotate;
				s.read(vRotate);
				bmdSkeleton.setBmdBone[uBoneID].setRotate.push_back(fixCoordSystemRotate(vRotate));
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// LocalMatrix
	std::vector<Matrix> setLocalMatrix;
	bmdSkeleton.getLocalMatrix(setLocalMatrix);
	for (size_t i=0;i<setBmdSub.size();++i)
	{
		setBmdSub[i].skinMesh(setLocalMatrix);
	}
	return true;
}

struct SmdVertex
{
	unsigned char b;
	Vec3D p;
	Vec3D n;
	Vec2D t;
};

#include<iostream>
#include<iomanip>
#include<fstream>
bool CMUBmd::loadFormSmd(const std::string& strFilename)
{
	std::ifstream file;
	file.open(strFilename.c_str(),std::ios::in);
	if ( !file.is_open() )
	{
		return false;
	}
	std::string strLine;

	getline(file,strLine);

	if ("version 1"!=strLine)
	{
		return false;
	}

	// nodes
	getline(file,strLine);
	if ("nodes"==strLine)
	{
		int nBoneID;
		char szName[32];
		int nParent; 
		while (true)
		{
			getline(file,strLine);
			if ("end"==strLine)
			{
				break;
			}
			if (3 != sscanf(strLine.c_str(), "%d \"%[^\"]\" %d", &nBoneID, szName, &nParent))
			{
				continue;
			}
			if (bmdSkeleton.setBmdBone.size()<=nBoneID)
			{
				bmdSkeleton.setBmdBone.resize(nBoneID+1);
			}
			strcpy(bmdSkeleton.setBmdBone[nBoneID].szName,szName);
			//setBmdBone[nBoneID].szName=szName;
			bmdSkeleton.setBmdBone[nBoneID].nParent=nParent;
		}
		head.uBoneCount=nBoneID+1;
	}
	head.uAnimCount=1;
	// skeleton
	getline(file,strLine);
	bmdSkeleton.setBmdAnim.resize(1);
	bmdSkeleton.setBmdAnim[0].uFrameCount=1;
	bmdSkeleton.setBmdAnim[0].bOffset=false;
	if ("skeleton"==strLine)
	{
		while (true)
		{
			getline(file,strLine);
			if ("end"==strLine)
			{
				break;
			}
			for (size_t uBoneID=0;uBoneID<head.uBoneCount;++uBoneID)
			{
				getline(file,strLine);
				Vec3D vTrans,vRotate;
				int nID;
				if (7 != sscanf(strLine.c_str(), "%d %f %f %f %f %f %f",&nID, &vTrans.x, &vTrans.y, &vTrans.z, &vRotate.x, &vRotate.y, &vRotate.z))
				{
					//continue;
				}
				vTrans=fixCoordSystemPos(vTrans);
				vRotate=fixCoordSystemRotate(vRotate);

				bmdSkeleton.setBmdBone[uBoneID].setTrans.push_back(vTrans);
				bmdSkeleton.setBmdBone[uBoneID].setRotate.push_back(vRotate);
			}
		}
	}
	// triangles
	std::string strSubTexture;
	std::vector<std::string> setSubTexture;
	std::map<std::string,std::vector<SmdVertex>> setSub;// 排序问题
	getline(file,strLine);
	if ("triangles"==strLine)
	{
		while (true)
		{
			getline(file,strLine);
			if ("end"==strLine)
			{
				break;
			}
			if (strSubTexture!=strLine)
			{
				strSubTexture = strLine;
				setSubTexture.push_back(strSubTexture);
			}
			std::vector<SmdVertex>& sub = setSub[strLine];
			for (size_t i=0;i<3;++i)
			{
				getline(file,strLine);
				SmdVertex smdVertex;
				if (9 != sscanf(strLine.c_str(), "%d %f %f %f %f %f %f %f %f",
					&smdVertex.b, &smdVertex.p.x, &smdVertex.p.y, &smdVertex.p.z, &smdVertex.n.x, &smdVertex.n.y, &smdVertex.n.z, &smdVertex.t.x, &smdVertex.t.y))
				{
					//continue;
				}
				smdVertex.p=fixCoordSystemPos(smdVertex.p);
				smdVertex.n=fixCoordSystemNormal(smdVertex.n);
				smdVertex.t.y=1.0f-smdVertex.t.y;
				sub.push_back(smdVertex);
			}
		}
	}
	head.uSubCount=setSub.size();
	setBmdSub.resize(head.uSubCount);
	for (size_t uSubID=0;uSubID<head.uSubCount;++uSubID)
	{
		BmdSub& bmdSub = setBmdSub[uSubID];
		strSubTexture = setSubTexture[uSubID];
		strcpy(bmdSub.szTexture,strSubTexture.c_str());
		std::vector<SmdVertex>& sub = setSub[strSubTexture];

		bmdSub.head.uVertexCount=sub.size();
		bmdSub.head.uNormal=sub.size();
		bmdSub.head.uUVCount=sub.size();
		bmdSub.head.uTriangleCount=sub.size()/3;
		bmdSub.head.uID=uSubID;
		bmdSub.setVertex.resize(sub.size());
		bmdSub.setNormal.resize(sub.size());
		bmdSub.setUV.resize(sub.size());
		bmdSub.setTriangle.resize(sub.size()/3);
		for (size_t uVertexID=0;uVertexID<sub.size();++uVertexID)
		{
			bmdSub.setVertex[uVertexID].uBones=sub[uVertexID].b;
			bmdSub.setVertex[uVertexID].vPos=sub[uVertexID].p;
			bmdSub.setNormal[uVertexID].uBones=sub[uVertexID].b;
			bmdSub.setNormal[uVertexID].vNormal=sub[uVertexID].n;
			bmdSub.setNormal[uVertexID].uUnknown2=0;
			bmdSub.setUV[uVertexID]=sub[uVertexID].t;
			bmdSub.setTriangle[uVertexID/3].uUnknown1=0x03;
			bmdSub.setTriangle[uVertexID/3].indexVertex[uVertexID%3]=uVertexID;
			bmdSub.setTriangle[uVertexID/3].indexNormal[uVertexID%3]=uVertexID;
			bmdSub.setTriangle[uVertexID/3].indexUV[uVertexID%3]=uVertexID;
			bmdSub.setTriangle[uVertexID/3].uUnknown2=0xCDCD;
			bmdSub.setTriangle[uVertexID/3].uUnknown3=0xCDCD;
			memset(bmdSub.setTriangle[uVertexID/3].uUnknown,0xCD,40);
		}
	}
	file.close();
	//////////////////////////////////////////////////////////////////////////
	// LocalMatrix
	std::vector<Matrix> setLocalMatrix;
	bmdSkeleton.getLocalMatrix(setLocalMatrix);
	for (size_t i=0; i<setLocalMatrix.size();++i)
	{
		setLocalMatrix[i].Invert();
	}
	for (size_t i=0;i<setBmdSub.size();++i)
	{
		setBmdSub[i].skinMesh(setLocalMatrix);
	}
	return true;
}

void CMUBmd::saveToBmd(const std::string& strFilename)
{
	FILE* f=fopen(strFilename.c_str(),"wb+");
	if (NULL==f)
	{
		return;
	}
	// Tag
	unsigned long uTag=0x0a444d42;//BMD.
	fwrite(&uTag,4,1,f);
	CMemoryStream s;
	s.resize(4000000);
	// Head
	s.write(head);
	//fwrite(&head,sizeof(head),1,f);
	// Sub Mesh
// 	for (size_t i=0; i<setBmdSub.size(); ++i)
// 	{
// 		//setBmdSub[i].load(s);
// 	}
	for (size_t uSubID=0;uSubID<setBmdSub.size();++uSubID)
	{
		BmdSub& bmdSub = setBmdSub[uSubID];

		s.write(bmdSub.head);
		//std::transform(bmdSub.setVertex.begin(),bmdSub.setVertex.end(),bmdSub.setVertex.begin(),fixCoordSystemPos);
		//std::transform(bmdSub.setNormal.begin(),bmdSub.setNormal.end(),bmdSub.setNormal.begin(),fixCoordSystemNormal);
		for (size_t  i=0;i<bmdSub.setVertex.size();++i)
		{
			s.write(bmdSub.setVertex[i].uBones);
			s.write(fixCoordSystemPos(bmdSub.setVertex[i].vPos));
		}
		for (size_t  i=0;i<bmdSub.setVertex.size();++i)
		{
			s.write(bmdSub.setNormal[i].uBones);
			s.write(fixCoordSystemPos(bmdSub.setNormal[i].vNormal));
			s.write(bmdSub.setNormal[i].uUnknown2);
		}
		s.writeVector(bmdSub.setUV);
		s.writeVector(bmdSub.setTriangle);
		s.write((unsigned char*)bmdSub.szTexture,32);
	}
	//Skeleton
	bmdSkeleton.setBmdAnim.resize(head.uAnimCount);
	for (size_t i=0; i<head.uAnimCount;++i)
	{
		s.write(bmdSkeleton.setBmdAnim[i].uFrameCount);
		s.write(bmdSkeleton.setBmdAnim[i].bOffset);
		if (bmdSkeleton.setBmdAnim[i].bOffset)
		{
			s.writeVector(bmdSkeleton.setBmdAnim[i].vOffset);
		}
	}
	bmdSkeleton.setBmdBone.resize(head.uBoneCount);
	for (size_t uBoneID=0; uBoneID<head.uBoneCount;++uBoneID)
	{
		s.write(bmdSkeleton.setBmdBone[uBoneID].bEmpty);
		if (bmdSkeleton.setBmdBone[uBoneID].bEmpty)
		{
			continue;
		}
		s.write((unsigned char*)bmdSkeleton.setBmdBone[uBoneID].szName,32);
		s.write(bmdSkeleton.setBmdBone[uBoneID].nParent);
		for (size_t uAnimID=0; uAnimID<head.uAnimCount;++uAnimID)
		{
			for (size_t j=0; j<bmdSkeleton.setBmdAnim[uAnimID].uFrameCount;++j)
			{
				Vec3D vTrans=bmdSkeleton.setBmdBone[uBoneID].setTrans[j];
				s.write(fixCoordSystemPos(vTrans));
			}
			for (size_t j=0; j<bmdSkeleton.setBmdAnim[uAnimID].uFrameCount;++j)
			{
				Vec3D vRotate=bmdSkeleton.setBmdBone[uBoneID].setRotate[j];
				s.write(fixCoordSystemRotate(vRotate));
			}
		}
	}
	// Write buffer.
	size_t bufferSize = s.getCursorPos();
	fwrite(s.getBuffer(),bufferSize,1,f);
	fclose(f);
}

void CMUBmd::saveToSmd(const std::string& strFilename)
{
	std::ofstream file;
	file.open(strFilename.c_str(), std::ios::out);
	if ( file.is_open() )
	{
		file << "version 1"<< std::endl;
		file << "nodes"<< std::endl;
		for (size_t i=0;i<bmdSkeleton.setBmdBone.size();++i)
		{
			BmdSkeleton::BmdBone& bmdBone = bmdSkeleton.setBmdBone[i];
			if (!bmdBone.bEmpty)
			{
				file<<i<<" \""<<bmdBone.szName<<"\" "<<bmdBone.nParent<<std::endl;
			}
		}
		file << "end"<< std::endl;
		file << "skeleton"<< std::endl;
		file << "time 0"<< std::endl;
		for (size_t i=0;i<bmdSkeleton.setBmdBone.size();++i)
		{
			BmdSkeleton::BmdBone& bmdBone = bmdSkeleton.setBmdBone[i];
			if (!bmdBone.bEmpty)
			{
				Vec3D& vTrans = bmdBone.setTrans[0];
				Vec3D& vRotate = bmdBone.setRotate[0];
				vTrans.z=-vTrans.z;
				vRotate.z=-vRotate.z;
				file<<i<<" "<<vTrans.x<<" "<<vTrans.y<<" "<<vTrans.z<<" "<<vRotate.x<<" "<<vRotate.y<<" "<<vRotate.z<<std::endl;
			}
		}
		file << "end"<< std::endl;
		// triangles
		file << "triangles"<< std::endl;
		for (size_t uSubID=0;uSubID<setBmdSub.size();++uSubID)
		{
			BmdSub& bmdSub = setBmdSub[uSubID];
			size_t uBmdTriangleSize = bmdSub.setTriangle.size();
			for (size_t j=0;j<uBmdTriangleSize;++j)
			{
				BmdSub::BmdTriangle& bmdTriangle = bmdSub.setTriangle[j];
				file << bmdSub.szTexture << std::endl;
				for (size_t n=0;n<3;++n)
				{
					int nPos = bmdTriangle.indexVertex[n];
					int nNormal = bmdTriangle.indexNormal[n];
					int nUV = bmdTriangle.indexUV[n];

					int nBone = bmdSub.setVertex[nPos].uBones;
					Vec3D vPos = bmdSub.setVertex[nPos].vPos;
					Vec3D vNormal = bmdSub.setNormal[nNormal].vNormal;
					Vec2D vUV = bmdSub.setUV[nUV];
					// fix // 导出时 再把它纠正成openGl坐标系
					vPos.z=-vPos.z;
					vNormal.z=-vNormal.z;
					vUV.y = 1.0f-vUV.y;

					file<<nBone<<" "<<vPos.x<<" "<<vPos.y<<" "<<vPos.z<<" "<<vNormal.x<<" "<<vNormal.y<<" "<<vNormal.z<<" "<<vUV.x<<" "<<vUV.y<<std::endl;
				}
			}
		}
		file << "end";
	}
	file.close();
	//////////////////////////////////////////////////////////////////////////
	int nFrameIndex=0;
	for (size_t animID=0;animID<bmdSkeleton.setBmdAnim.size();++animID)
	{
		char szAnimName [256];
		std::string strAnimFilename;
		sprintf (szAnimName, "_%03d.smd", animID+1);
		strAnimFilename = ChangeExtension(strFilename,szAnimName);
		file.open(strAnimFilename.c_str(), std::ios::out);
		if ( file.is_open() )
		{
			file << "version 1"<< std::endl;
			file << "nodes"<< std::endl;
			for (size_t boneID=0;boneID<bmdSkeleton.setBmdBone.size();++boneID)
			{
				BmdSkeleton::BmdBone& bmdBone = bmdSkeleton.setBmdBone[boneID];
				if (!bmdBone.bEmpty)
				{
					file<<boneID<<" \""<<bmdBone.szName<<"\" "<<bmdBone.nParent<<std::endl;
				}
			}
			file << "end"<< std::endl;
			file << "skeleton"<< std::endl;

			size_t uFrameCount = bmdSkeleton.setBmdAnim[animID].uFrameCount;
			for (size_t frameID=0;frameID<uFrameCount;++frameID)
			{
				file<<"time "<<frameID<<std::endl;
				for (size_t boneID=0;boneID<bmdSkeleton.setBmdBone.size();++boneID)
				{
					BmdSkeleton::BmdBone& bmdBone = bmdSkeleton.setBmdBone[boneID];
					if (!bmdBone.bEmpty)
					{
						Vec3D& vTrans = bmdBone.setTrans[nFrameIndex];
						Vec3D& vRotate = bmdBone.setRotate[nFrameIndex];
						// fix // 导出时 再把它纠正成openGl坐标系
						//vTrans.z=-vTrans.z;
						//vRotate.z=-vRotate.z;
						file<<boneID<<" "<<vTrans.x<<" "<<vTrans.y<<" "<<vTrans.z<<" "<<vRotate.x<<" "<<vRotate.y<<" "<<vRotate.z<<std::endl;
					}
				}
				++nFrameIndex;
			}
			file << "end";
		}
		file.close();
	}
}

void Bmd2Smd(const std::string& strSrcFilename, const std::string& strDestFilename)
{
	CMUBmd bmd;
	bmd.loadFormBmd(strSrcFilename);
	bmd.saveToSmd(strDestFilename);
}

void Smd2Bmd(const std::string& strSrcFilename, const std::string& strDestFilename)
{
	CMUBmd bmd;
	bmd.loadFormSmd(strSrcFilename);
	bmd.saveToBmd(strDestFilename);
}