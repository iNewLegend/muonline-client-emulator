#pragma once
#include "Matrix.h"
#include "Plane.h"
#include "vector"

//Cross Return
enum CrossRet
{
	//������
	cross_exclude
	//����
	,cross_include
	//����
	,cross_cross
};

class BBox
{
public:
	BBox();
	Vec3D vMin,vMax;
	BBox operator+ (const BBox &b) const;
	BBox& operator+= (const BBox &b);
	bool crossVertex(const Vec3D& vPos)const;
	bool interSectLine(const Vec3D &p,const Vec3D &d,float &tmin,float&tmax) const;
	bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax) const;
	CrossRet checkAABBVisible(const BBox& bbox) const;
};

// ƽ��ͷ��
class CFrustum
{
	Vec3D	m_vtx[8];	// ����ƽ��ͷ���8������
	std::vector<Plane> m_planes; // ����ƽ��ͷ���6��ƽ��
public:
	CFrustum();
	~CFrustum();

	bool operator==(const CFrustum & frustum)
	{
		for(int i = 0 ; i < 8 ; i ++)
		{
			if(m_vtx[i]!=frustum.m_vtx[i])
				return false;
		}
		return true;
	}
	//
	Vec3D getEyePoint()const;
	//������׶
	void Build(const Matrix& matProjView);
	void buildNoNear(const Matrix& matProjView);
	void Build(Vec3D vecMin,Vec3D vecMax);

	/// ��v��ƽ��ͷ���ڣ�����TRUE������ƽ��ͷ���ڣ�����FALSE.
	bool IsIn(const Vec3D& vector);

	/** ��������Բ�㣨v���Ͱ뾶��radius���ı߽����壨bounding sphere����ƽ��ͷ���ڣ�
	*  ����TRUE������ƽ��ͷ���ڣ�����FALSE.
	*/
	bool IsInSphere(const Vec3D& vector, float radius);

	CrossRet CheckAABBVisible(const BBox& bbox) const;
};