#include "MUBmd.h"

void decryptMuBuffer(unsigned char* buffer, size_t size)
{
	static const unsigned char xorKeys[16] = {
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

void CMUBmd::BmdSkeleton::BmdAnim::load(CMemoryStream& s)
{
	s.read(uFrameCount);
	s.read(bOffset);
	if (bOffset)
	{
		s.readVector(vOffset,uFrameCount);
	}
}

void CMUBmd::BmdSkeleton::BmdBone::load(CMemoryStream& s, const std::vector<BmdAnim>& setBmdAnim)
{
	s.read(bEmpty);
	if (bEmpty)
	{
		return;
	}
	s.read((unsigned char*)szName,32);
	s.read(nParent);
	for (size_t i=0; i<setBmdAnim.size();++i)
	{
		for (size_t j=0; j<setBmdAnim[i].uFrameCount;++j)
		{
			Vec3D vTrans;
			s.read(vTrans);
			setTrans.push_back(vTrans);
		}
		for (size_t j=0; j<setBmdAnim[i].uFrameCount;++j)
		{
			Vec3D vRotate;
			s.read(vRotate);
			setRotate.push_back(vRotate);
		}
	}
}

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
		m *= Matrix::newTranslation(fixCoordSystemPos(b.setTrans[0]));
	}
	if (b.setRotate.size()>0)
	{
		m *= Matrix::newQuatRotate(fixCoordSystemRotate(b.setRotate[0]));
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

void CMUBmd::BmdSkeleton:: load(CMemoryStream& s, unsigned short uBoneCount, unsigned short uAnimCount)
{
	setBmdAnim.resize(uAnimCount);
	setBmdBone.resize(uBoneCount);
	for (size_t i=0; i<setBmdAnim.size();++i)
	{
		setBmdAnim[i].load(s);
	}
	for (size_t i=0; i<setBmdBone.size();++i)
	{
		setBmdBone[i].load(s,setBmdAnim);
	}
}

void CMUBmd::BmdSub::load(CMemoryStream& s)
{
	s.read(head);

	s.readVector(setVertex,head.uVertexCount);
	s.readVector(setNormal,head.uNormal);
	s.readVector(setUV,head.uUVCount);
	s.readVector(setTriangle,head.uTriangleCount);

	s.read((unsigned char*)szTexture,32);
}

bool CMUBmd::LoadFile(const std::string& strFilename)
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

	s.read(head);
	assert(head.uSubCount<50);
	setBmdSub.resize(head.uSubCount);
	for (size_t i=0; i<setBmdSub.size(); ++i)
	{
		setBmdSub[i].load(s);
	}
	bmdSkeleton.load(s, head.uBoneCount,head.uAnimCount);

	for (size_t i=0;i<bmdSkeleton.setBmdBone.size();++i)
	{
		bmdSkeleton.calcLocalMatrix(i);
	}

	nFrameCount = 0;
	for (size_t i=0; i<head.uAnimCount; ++i)
	{
		nFrameCount+=bmdSkeleton.setBmdAnim[i].uFrameCount;
	}
	return true;
}