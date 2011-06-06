#pragma once
#include "IORead.h"
#include "Vec2D.h"
#include "Vec3D.h"
#include "Vec4D.h"
#include "Matrix.h"
#include "MemoryStream.h"

class CMUBmd
{
public:
	CMUBmd()
	{
	}
	struct BmdHead
	{
		BmdHead()
		{
			memset(this,0,sizeof(*this));
		}
		char strFile[32];
		unsigned short uSubCount;
		unsigned short uBoneCount;
		unsigned short uAnimCount;
	};

	struct BmdSkeleton
	{
		struct BmdAnim
		{
			BmdAnim()
			{
				uFrameCount=0;
				bOffset=false;
			}
			unsigned short uFrameCount;
			bool bOffset;
			std::vector<Vec3D> vOffset;
		};
		struct BmdBone
		{
			BmdBone()
			{
				bEmpty=0;
				szName[0]=0;
				nParent=-1;
			}
			bool bEmpty;
			char szName[32];
			short nParent;
			std::vector<Vec3D> setTrans;
			std::vector<Vec3D> setRotate;
			Matrix	mLocal;
		};
		Matrix	getLocalMatrix(unsigned char uBoneID);
		Matrix	getRotateMatrix(unsigned char uBoneID);
		void	calcLocalMatrix(unsigned long uBoneID);
		void	getLocalMatrix(std::vector<Matrix>& setLocalMatrix);

		std::vector<BmdAnim> setBmdAnim;
		std::vector<BmdBone> setBmdBone;
	};

	struct BmdSub
	{
		struct BmdSubHead
		{
			unsigned short uVertexCount;
			unsigned short uNormal;
			unsigned short uUVCount;
			unsigned short uTriangleCount;
			unsigned short uID;//?
		};
		struct BmdPos
		{
			unsigned long uBones;
			Vec3D vPos;
		};
		struct BmdNormal
		{
			unsigned long uBones;//?
			Vec3D vNormal;
			unsigned long uUnknown2;
		};
		struct BmdTriangle
		{
			unsigned short uUnknown1;//03
			unsigned short indexVertex[3];
			unsigned short uUnknown2;//CD
			unsigned short indexNormal[3];
			unsigned short uUnknown3;//CD
			unsigned short indexUV[3];
			unsigned short uUnknown[20];//CDCDCDCD
		};

		BmdSubHead head;
		std::vector<BmdPos> setVertex;
		std::vector<BmdNormal> setNormal;
		std::vector<Vec2D> setUV;
		std::vector<BmdTriangle> setTriangle;

		char szTexture[32];// Œ∆¿Ì
		void skinMesh(std::vector<Matrix>& setBoneMatrix);
	};

	bool loadFormBmd(const std::string& strFilename);
	bool loadFormSmd(const std::string& strFilename);

	void saveToBmd(const std::string& strFilename);
	void saveToSmd(const std::string& strFilename);

	BmdHead head;
	std::vector<BmdSub> setBmdSub;
	BmdSkeleton bmdSkeleton;
};

void Bmd2Smd(const std::string& strSrcFilename, const std::string& strDestFilename);
void Smd2Bmd(const std::string& strSrcFilename, const std::string& strDestFilename);