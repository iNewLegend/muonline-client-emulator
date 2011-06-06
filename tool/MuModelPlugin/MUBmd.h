#pragma once
#include "IORead.h"
#include "Vec2D.h"
#include "Vec3D.h"
#include "Vec4D.h"
#include "Matrix.h"
#include "MemoryStream.h"
#include <vector>

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

inline Vec3D fixCoordSystemPos(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y)*0.01f;
}
inline Vec3D fixCoordSystemNormal(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y);
}

inline Quaternion fixCoordSystemRotate(Vec3D v)
{
	Quaternion q;
	q.rotate(Vec3D(v.x,v.y,-v.z));
	return Quaternion(q.x, -q.z, q.y, q.w);
}

class CMUBmd
{
public:
	CMUBmd()
	{
		nFrameCount=0;
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
			void load(CMemoryStream& s);
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

			void load(CMemoryStream& s, const std::vector<BmdAnim>& setBmdAnim);
		};
		std::vector<BmdAnim> setBmdAnim;
		std::vector<BmdBone> setBmdBone;

		Matrix getLocalMatrix(unsigned char uBoneID);
		Matrix getRotateMatrix(unsigned char uBoneID);
		void calcLocalMatrix(unsigned long uBoneID);

		void load(CMemoryStream& s, unsigned short uBoneCount, unsigned short uAnimCount);
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

		char szTexture[32];// 纹理
		void load(CMemoryStream& s);
	};

	bool LoadFile(const std::string& strFilename);

	BmdHead head;
	std::vector<BmdSub> setBmdSub;
	BmdSkeleton bmdSkeleton;
	int nFrameCount;

};

#define MU_BMD_ANIM_FRAME_TIME 150

