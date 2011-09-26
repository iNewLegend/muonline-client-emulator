#pragma once
#include "LodMesh.h"

class CMeshCoordinate:public CMeshData
{
public:
	CMeshCoordinate();
	~CMeshCoordinate();
public:
	void init();
	void setPos(const Vec3D& vPos);
	void setScale(float fScale);
	void render(const Vec3D& vCoordShow);
	bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir,Vec3D& vCoord)const;
protected:
	Matrix getWorldMatrix()const;
	enum CoordLineType
	{
		CLT_X,
		CLT_X_Y,
		CLT_X_Z,

		CLT_Y,
		CLT_Y_X,
		CLT_Y_Z,

		CLT_Z,
		CLT_Z_X,
		CLT_Z_Y,

		CLT_MAX,
	};
	struct CoordLine
	{
		Vec3D vBegin,vEnd;
	};
	CoordLine m_CoordLines[CLT_MAX];
	Vec3D m_vPos;
	float m_fScale;
};