#include "stdafx.h"
#define INITGUID

#include "RTypes.h"
#include "RealSpace2.h"
#include "crtdbg.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <dxdiag.h>

_NAMESPACE_REALSPACE2_BEGIN

// �������� �ٿ���ڽ��� �ִܰŸ�
float GetDistance(const rboundingbox &bb,const rvector &point)
{
	rvector closest;

	if( bb.minx<=point.x && point.x<=bb.maxx ) closest.x=point.x;
		else closest.x = fabs(point.x-bb.minx) < fabs(point.x-bb.maxx) ? bb.minx : bb.maxx;
	
	if( bb.miny<=point.y && point.y<=bb.maxy ) closest.y=point.y;
		else closest.y = fabs(point.y-bb.miny) < fabs(point.y-bb.maxy) ? bb.miny : bb.maxy;

	if( bb.minz<=point.z && point.z<=bb.maxz ) closest.z=point.z;
		else closest.z = fabs(point.z-bb.minz) < fabs(point.z-bb.maxz) ? bb.minz : bb.maxz;

	return Magnitude(closest-point);
}

// �������� �������� �Ÿ� .. normalized plane ���� ����.
float GetDistance(const rvector &position,const rplane &plane)		
{
	return fabsf(D3DXPlaneDotCoord(&plane,&position));
}

// ����(a,aa) ���� �������� ���� ����� �������� ��.
rvector GetNearestPoint(const rvector &a,const rvector &aa,const rplane &plane)
{
	rvector b=aa-a;

	float fDot=D3DXPlaneDotNormal(&plane,&b);
	if(IS_ZERO(fDot))
		return a;

	float t=-(plane.d+D3DXPlaneDotNormal(&plane,&a))/fDot;
	if(t<0) t=0;
	if(t>1) t=1;

	return a+t*b;
}

// ����(a,aa) ���� �������� �Ÿ�
float GetDistance(const rvector &a,const rvector &aa,const rplane &plane) 
{
	return GetDistance(GetNearestPoint(a,aa,plane),plane);
}

// ������ ������ �Ÿ�
float GetDistance(const rvector &position,const rvector &line1,const rvector &line2)
{
	rvector a=position-line1;
	rvector b=line2-line1;
	float asq=DotProduct(a,a);
	float bsq=DotProduct(b,b);
	return sqrtf(asq-powf(DotProduct(a,b),2)/bsq);
}

// �������� ���� ����� �������� ��
rvector GetNearestPoint(const rvector &position,const rvector &a,const rvector &b)
{
	rvector dir=b-a;

	float d=-DotProduct(position,dir); 
	// position�������� dir �� �������� ������ ����� ��������, x*dir.x+y*dir.y+z*dir.z+d=0 �̴�.

	// �������� position ���� ����� ���� a+t*dir �� ��Ÿ����,
	float mdir = D3DXVec3LengthSq(&dir);
	if(mdir<0.001f) return a;

	float t=-(DotProduct(a,dir)+d)/mdir;

	if(t<0) t=0;
	if(t>1) t=1;

	return a+t*dir;
}

// �������� ���б����� �Ÿ�
float GetDistanceLineSegment(const rvector &position,const rvector &a,const rvector &b)
{
	return Magnitude(GetNearestPoint(position,a,b)-position);
}

// ���а� ���� ������ �Ÿ�.. ���� (a,aa) �� ���� (c,cc)�� �Ÿ�.
float GetDistanceBetweenLineSegment(const rvector &a,const rvector &aa,const rvector &c,const rvector &cc,rvector *ap,rvector *cp)
{
	rvector b=aa-a;
	rvector d=cc-c;

	// �� ���п� ���� ������ ������ b x d
	rvector cm;
	CrossProduct(&cm,b,d);

	float fMagnitude=Magnitude(cm);

	if(fMagnitude<1)	// �� ������ �����ϴٸ�...
	{

		rvector x;	// �� ������ �����ϴ� �� ������ ��.. �̰� ���ϴ� ����� ���� ������ �Ÿ����ϴ¹��� ����.

		rvector edge=a-c;
		float temp=DotProduct(edge,d)/Magnitude(d);
		rvector dir=d;
		Normalize(dir);
		x=c+temp*dir-a;

		float st0,st1;		// t=0 �϶��� s �� ��, t=1 �϶��� s �� ��

		st0=DotProduct(a+x-c,d)/DotProduct(d,d);
		st1=DotProduct(aa+x-c,d)/DotProduct(d,d);

        if(st0<0 && st1<0)
		{
			*cp=c;
			if(fabs(st0)>fabs(st1))
				*ap=aa;
			else
				*ap=a;
		}
		else
		if(st0>1 && st1>1)
		{
			*cp=cc;
			if(fabs(st0)>fabs(st1))
				*ap=aa;
			else
				*ap=a;
		}else
		{
			if(st0>=0 && st0<=1)
			{
				*ap=a;
				*cp=c+st0*d;
			}else
			if(st1>=0 && st1<=1)
			{
				*ap=aa;
				*cp=c+st1*d;
			}else
			{
				*cp=c;
				*ap=*cp-x;
			}
		}
		return Magnitude(*ap - *cp);
	}

//	Normalize(cm);
	cm/=fMagnitude;

	// c, c+d, c+cm �� ���� ������ ����� �������� rp-rc=0 ( r = d x cm , ������ ������ ����� ��������� ������ crossproduct �̸� ��c�� ������.)
	// �� ���� a+tb ������ �������� r(a+tb)-rc=0   ->  t=(rc-ra)/(rb)
	
	rvector r;
	CrossProduct(&r,d,cm);
	Normalize(r);

	float t=(DotProduct(r,c)-DotProduct(r,a))/DotProduct(r,b);

	// a, a+b, a+cm �� ���� ������ ����� �������� rp-ra=0 ( r = b x cm , ������ ������ ����� ��������� ������ crossproduct �̸� ��a�� ������.)
	// �� ���� c+sd ������ �������� r(c+sd)-ra=0    ->  s=(ra-rc)/(rd)

	CrossProduct(&r,b,cm);
	Normalize(r);

	float s=(DotProduct(r,a)-DotProduct(r,c))/DotProduct(r,d);

	if(t<0) t=0;
	if(t>1) t=1;

	if(s<0) s=0;
	if(s>1) s=1;

	*ap=a+t*b;
	*cp=c+s*d;
	return Magnitude(*ap - *cp);
}
// �� ������ �Ÿ��� ���� ������.. a+tb+k(cm)=c+sd ��� ����.. �纯�� cm �� DotProduct ���ָ� ok.. (cm �� b,d �� �����̱� ������) k=(a-c)cm


float GetDistance(rboundingbox *bb,rplane *plane)		// ��鿡�� boundingbox���� �ִ�Ÿ�
{
	float a,b,c;
	a=(plane->a>0)?bb->m[1][0]:bb->m[0][0];
	b=(plane->b>0)?bb->m[1][1]:bb->m[0][1];
	c=(plane->c>0)?bb->m[1][2]:bb->m[0][2];
	return plane->a*a+plane->b*b+plane->c*c+plane->d;
}

// ��鿡�� boundingbox���� �ּ�,�ִ�Ÿ�
void GetDistanceMinMax(rboundingbox &bb,rplane &plane,float *MinDist,float *MaxDist)
{
	float a,b,c,a2,b2,c2;
	if(plane.a>0) { a=bb.m[1][0];a2=bb.m[0][0]; } else { a=bb.m[0][0];a2=bb.m[1][0]; }
	if(plane.b>0) { b=bb.m[1][1];b2=bb.m[0][1]; } else { b=bb.m[0][1];b2=bb.m[1][1]; }
	if(plane.c>0) { c=bb.m[1][2];c2=bb.m[0][2]; } else { c=bb.m[0][2];c2=bb.m[1][2]; }
	*MaxDist=plane.a*a+plane.b*b+plane.c*c+plane.d;
	*MinDist=plane.a*a2+plane.b*b2+plane.c*c2+plane.d;
}

// �ﰢ���� ����
float GetArea(rvector &v1,rvector &v2,rvector &v3)
{
	float a,b,c;	// �ﰢ���� ������ ����.
	a=Magnitude(v1-v2);
	b=Magnitude(v2-v3);
	c=Magnitude(v3-v1);

	float p=(a+b+c)/2;
	return (float)sqrt(p*(p-a)*(p-b)*(p-c));
}

rvector InterpolatedVector(rvector &a,rvector &b,float x)
{
	float ab,theta,theta1,theta2,costheta1,costheta2,u,v;
	
	ab=(float)min(max(DotProduct(a,b),-1.),1.);
	if(ab==1.0f) return b;	// ������ 0�̸� �׳� ����

//	else if(ab==-1.0f) return TransformVector(a, RotateZMatrix(pi*x));		// ������ 180�̸� Z������ ȸ���Ѵ�.

	theta=(float)acos(ab);
	//if(theta==0.0f) return a;	// 0���̸� a ����

	theta1=theta*x;
	theta2=theta*(1.0f-x);
	costheta1=cosf(theta1);
	costheta2=cosf(theta2);
	u=costheta1-ab*costheta2;
	v=costheta2-ab*costheta1;
	float D = (1.0f-ab*ab);
//	_ASSERT(D!=0.0f);	// �տ��� ab�� �˻��ϹǷ� 0�� ���� �� ����.
	if(D==0) return a;

	rvector vReturn=(1.0f/D*(u*a+v*b));
//	_ASSERT(!_isnan(vReturn.z));
	return vReturn;
}

bool isInPlane(rboundingbox *bb,rplane *plane)
{
	return (GetDistance(bb,plane)>=0);
}

bool IsIntersect(rboundingbox *bb1,rboundingbox *bb2)
{

    if(bb1->minx>bb2->maxx) return false;
	if(bb1->miny>bb2->maxy) return false;
	if(bb1->minz>bb2->maxz) return false;

	if(bb2->minx>bb1->maxx) return false;
	if(bb2->miny>bb1->maxy) return false;
	if(bb2->minz>bb1->maxz) return false;

	return true;
}

bool IsInSphere(const rboundingbox &bb,const rvector &point,float radius)
{

	for(int i=0;i<3;i++)				// �ٿ���ڽ��� 6���� ������ �ѽ��� ���鿡 ����
	{
		rvector nearest;				// �� �ѽ��� ������������Ǿ��� ������ ���� ����� ���� ���,

		if(fabs(bb.m[0][i]-point[i])<fabs(bb.m[1][i]-point[i]))
			nearest[i]=bb.m[0][i];
		else
			nearest[i]=bb.m[1][i];

		int au=(i+1)%3,av=(i+2)%3;		// (����� ����� ������ �Ǵ� ���� �� ������ �� ���� ��´�)
		
		nearest[au]=min(max(point[au],bb.m[0][au]),bb.m[1][au]);
		nearest[av]=min(max(point[av],bb.m[0][av]),bb.m[1][av]);

		if(Magnitude(nearest-point)<radius)		// �� �Ÿ��� radius �����̸� true..
			return true;
	}

	
	// �Ǵ� ������ �̹� �ٿ���ڽ� �ȿ� ���ԵǾ� �־ true !
	return (bb.minx<=point.x && bb.miny<=point.y && bb.minz<=point.z && 
			bb.maxx>=point.x && bb.maxy>=point.y && bb.maxz>=point.z);
}

bool isInViewFrustum(const rvector &point,rplane *plane)
{
#define FN(i) ((plane[i].a*point.x+plane[i].b*point.y+plane[i].c*point.z+plane[i].d)>=0)
	return FN(0) && FN(1) && FN(2) && FN(3);
}

bool isInViewFrustum(const rvector &point,float radius,rplane *plane)		// bounding sphere
{
	if((GetDistance(point,plane[0])>-radius) &&
		(GetDistance(point,plane[1])>-radius) &&
		(GetDistance(point,plane[2])>-radius) &&
		(GetDistance(point,plane[3])>-radius) &&
		(GetDistance(point,plane[5])>-radius))
		return true;
	return false;
}

bool isInViewFrustum(rboundingbox *bb,rplane *plane)
{
	if(isInPlane(bb,plane)&&isInPlane(bb,plane+1)&&
		isInPlane(bb,plane+2)&&isInPlane(bb,plane+3)) return true;
	return false;
}

bool isInViewFrustumWithZ(rboundingbox *bb,rplane *plane)
{
	if(isInPlane(bb,plane)&&isInPlane(bb,plane+1)&&
		isInPlane(bb,plane+2)&&isInPlane(bb,plane+3)&&
		isInPlane(bb,plane+4)&&isInPlane(bb,plane+5)) return true;
	return false;
}

bool isInViewFrustumwrtnPlanes(rboundingbox *bb,rplane *plane,int nplane)
{
	for(int i=0;i<nplane;i++)
	{
		if(!isInPlane(bb,plane+i)) return false;
	}
	return true;
}

void MergeBoundingBox(rboundingbox *dest,rboundingbox *src)
{
	for(int i=0;i<3;i++)
	{
		dest->vmin[i]=min(dest->vmin[i],src->vmin[i]);
		dest->vmax[i]=max(dest->vmax[i],src->vmax[i]);
	}
}

bool IsIntersect( const rvector& orig, const rvector& dir, rvector& v0, rvector& v1, rvector& v2, float* t)//, FLOAT* u, FLOAT* v )
{
	float u,v;

	return D3DXIntersectTri(&v0,&v1,&v2,&orig,&dir,&u,&v,t)!=FALSE;
}

bool isLineIntersectBoundingBox(rvector &origin,rvector &dir,rboundingbox &bb)
{
	return D3DXBoxBoundProbe(&bb.vmin,&bb.vmax,&origin,&dir)!=FALSE;
}

void MakeWorldMatrix(rmatrix *pOut,rvector pos,rvector dir,rvector up) 
{
	D3DXMatrixIdentity(pOut);

	rvector right;

	D3DXVec3Normalize(&dir,&dir);

	D3DXVec3Cross(&right,&dir,&up);
	D3DXVec3Normalize(&right,&right);

	D3DXVec3Cross(&up,&right,&dir);
	D3DXVec3Normalize(&up,&up);

	pOut->_11 = right.x;
	pOut->_12 = right.y;
	pOut->_13 = right.z;

	pOut->_21 = up.x;
	pOut->_22 = up.y;
	pOut->_23 = up.z;

	pOut->_31 = -dir.x;
	pOut->_32 = -dir.y;
	pOut->_33 = -dir.z;

	pOut->_41 = pos.x;
	pOut->_42 = pos.y;
	pOut->_43 = pos.z;
}

bool IsIntersect( rvector& line_begin_, rvector& line_end_, rboundingbox& box_)
{
	// transform line to box space
	rvector box_center	= ( box_.vmin + box_.vmax ) * 0.5;
	rvector begin	= line_begin_ - box_center;
	rvector end		= line_end_ - box_center;
	float hx, hy, hz;
	hx = ( box_.maxx - box_.minx ) * 0.5f;
	hy = ( box_.maxy - box_.miny ) * 0.5f;
	hz = ( box_.maxz - box_.minz ) * 0.5f;

	// get center of line
	rvector center	= ( begin + end ) * 0.5f;

	float wx			= begin.x - center.x;
	float lengthX		= fabs( wx );
	if( fabs( center.x )  > lengthX )
	{
		return false;
	}

	float wy			= begin.y - center.y;
	float lengthY		= fabs( wy );
	if( fabs( center.y )  > lengthY )
	{
		return false;
	}

	float wz			= begin.z - center.z;
	float lengthZ		= fabs( wz );
	if( fabs( center.z )  > lengthZ )
	{
		return false;
	}

	if( fabs( center.y * wz - center.z * wy ) > hy * lengthZ + hz * lengthY )
	{
		return false;
	}
	if( fabs( center.x * wz - center.z * wx ) > hx * lengthZ + hz * lengthX )
	{
		return false;
	}
	if( fabs( center.x * wy - center.y * wx ) > hx * lengthY + hy * lengthX )
	{
		return false;
	}

	return true;
}

bool IsIntersect( rvector& line_begin_, rvector& line_dir_, rvector& center_, float radius_, float* dist /* = NULL */, rvector* p /* = NULL  */)
{
	rvector	l = center_ - line_begin_;
	float		s = D3DXVec3Dot( &l, &line_dir_ );
	float l_sq = D3DXVec3Dot( &l, &l );
	float r_sq = (radius_ * radius_);
	if( s < 0 && l_sq > r_sq )
		return false;
	float m_sq = l_sq - s*s;
	if( m_sq > r_sq )
		return false;
	if( dist == NULL || p == NULL )
		return true;
	float q = sqrt( r_sq- m_sq );
	if( l_sq > r_sq )
		*dist = s - q;
	//else
	//	*dist = s + q;
	*p = line_begin_ + (*dist) * line_dir_;
	return true;
}

float GetAngle(rvector &a)
{
	if(a.x>=1.0f) return 0.0f;
	if(a.x<=-1.0f) return -pi;
	if(a.y>0)
		return (float)acos(a.x);
	else
		return (float)-acos(a.x);
}

float GetAngleOfVectors(rvector &ta,rvector &tb)
{
	if((ta.x==0.0f)&&(ta.y==0.0f)&&(ta.z==0.0f))return 0;
	if((tb.x==0.0f)&&(tb.y==0.0f)&&(tb.z==0.0f))return 0;
	rvector a=ta,b=tb;
	a.z=0;Normalize(a);b.z=0;Normalize(b);
	float aa=GetAngle(a);
	float x,y;
	x=(float)(b.x*cos(aa)+b.y*sin(aa));
	y=(float)(b.x*(-sin(aa))+b.y*cos(aa));

	float ret=GetAngle(rvector(x,y,0));
	return ret;
}

bool IsIntersect(const rvector& orig, const rvector& dir, const rvector& center, const float radius, rvector* p)
{
	rvector center2orig = orig - center;
	float r2 = radius*radius;
	float tmp1 = D3DXVec3Dot(&center2orig,&dir);
	float tmp2 = D3DXVec3Dot(&center2orig,&center2orig);
	if(tmp1 < 0 && tmp2 > r2 ) return false; // dir�������� ������ �Ÿ��� �����̰� �������� �ۿ� ������ ���� ����
	float tmp3 = tmp2 - tmp1;
	if( tmp3 > r2 ) return false; // ������ ���б����� �Ÿ��� ���� ������ ���� ũ�� ���� ����
	
	if(p==NULL) return true;

	float q = sqrtf(r2-tmp3*tmp3);
	float t;
	if(tmp2>r2) t = tmp1 - q;
	else t = tmp1 + q;

	*p = orig + t*dir;
	return true;
}

/*
�� ����� ������ ������ �������� ���Ѵ� 
���� http://astronomy.swin.edu.au/~pbourke/geometry/planeplane/
*/

bool GetIntersectionOfTwoPlanes(rvector *pOutDir,rvector *pOutAPoint,rplane &plane1,rplane &plane2)
{
	rvector n1=rvector(plane1.a,plane1.b,plane1.c);
	rvector n2=rvector(plane2.a,plane2.b,plane2.c);

	rvector dir;
	CrossProduct(&dir,n1,n2);

	if(IS_ZERO(DotProduct(dir,dir))) return false;	// �����ϸ� ����

	float determinant=DotProduct(n1,n1)*DotProduct(n2,n2)-DotProduct(n1,n2)*DotProduct(n1,n2);
	float c1=(-plane1.d*DotProduct(n2,n2)+plane2.d*DotProduct(n1,n2))/determinant;
	float c2=(-plane2.d*DotProduct(n1,n1)+plane1.d*DotProduct(n1,n2))/determinant;

	*pOutAPoint=c1*n1+c2*n2;
	*pOutDir=dir;

	return true;
}
/*
void SetPlane(rplane& plane, rvector& point1, rvector& point2, rvector& point3)
{
	rvector v1 = point2 - point1;
	rvector v2 = point3 - point1;
	rvector normal;
	CrossProduct(&normal, v1, v2);

	plane.a = normal.x;
	plane.b = normal.y;
	plane.c = normal.z;
	plane.d = - DotProduct(&point1, &normal);
}
*/

_NAMESPACE_REALSPACE2_END
