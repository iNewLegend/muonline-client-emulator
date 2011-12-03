#include "stdafx.h"
#include "MDebug.h"
#include "RealSpace2.h"
#include "RSolidBsp.h"
#include <crtdbg.h>

_USING_NAMESPACE_REALSPACE2

bool RImpactPlanes::Add(rplane &p)
{
	for(iterator i=begin();i!=end();i++)
	{
		if(*i==p) return false;
	}
	push_back(p);
	return  true;
}


RCOLLISIONMETHOD RSolidBspNode::m_ColMethod;
float	RSolidBspNode::m_fColRadius;
float	RSolidBspNode::m_fColHeight;
rvector	RSolidBspNode::m_ColOrigin;
rvector	RSolidBspNode::m_ColTo;
RImpactPlanes* RSolidBspNode::m_pOutList;
float	RSolidBspNode::fImpactDist;
rvector	RSolidBspNode::impact;
rplane	RSolidBspNode::impactPlane;

RSolidBspNode::RSolidBspNode()
{
	m_pPositive=NULL;
	m_pNegative=NULL;
#ifndef _PUBLISH
	pVertices=NULL;
	pNormals=NULL;
#endif
}

RSolidBspNode::~RSolidBspNode()
{
#ifndef _PUBLISH
	SAFE_DELETE(pNormals);
#endif
}

#ifndef _PUBLISH
void RSolidBspNode::DrawPolygon()
{
	//	if(!isInViewFrustum(&m_bb,RGetViewFrustum())) return;

	if(nPolygon && !m_bSolid)
		RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,nPolygon,pVertices,sizeof(rvector));

	if(m_pPositive)
		m_pPositive->DrawPolygon();

	if(m_pNegative)
		m_pNegative->DrawPolygon();

}

void RSolidBspNode::DrawPolygonWireframe()
{
	//	if(!isInViewFrustum(&m_bb,RGetViewFrustum())) return;

	if(nPolygon && !m_bSolid)
	{
		rvector v[4];
		for(int i=0;i<nPolygon;i++)
		{
			memcpy(v,pVertices[i*3],3*sizeof(rvector));
			v[3]=v[0];

			RGetDevice()->DrawPrimitiveUP(D3DPT_LINESTRIP,3,&v,sizeof(rvector));
		}
	}

	if(m_pPositive)
		m_pPositive->DrawPolygonWireframe();

	if(m_pNegative)
		m_pNegative->DrawPolygonWireframe();

}

void RSolidBspNode::DrawPolygonNormal()
{
	//	if(!isInViewFrustum(&m_bb,RGetViewFrustum())) return;

	if(nPolygon && !m_bSolid)
	{
		rvector v[2];
		for(int i=0;i<nPolygon;i++)
		{
			rvector center=1.f/3.f*(pVertices[i*3]+pVertices[i*3+1]+pVertices[i*3+2]);

			v[0]=center;
			v[1]=center+pNormals[i]*50.f;
			RGetDevice()->DrawPrimitiveUP(D3DPT_LINESTRIP,1,&v,sizeof(rvector));
		}
	}

	if(m_pPositive)
		m_pPositive->DrawPolygonNormal();

	if(m_pNegative)
		m_pNegative->DrawPolygonNormal();

}

void RSolidBspNode::DrawSolidPolygon()
{
	//	if(!isInViewFrustum(&m_bb,RGetViewFrustum())) return;

	if(nPolygon && m_bSolid)
		RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,nPolygon,pVertices,sizeof(rvector));

	if(m_pPositive)
		m_pPositive->DrawSolidPolygon();

	if(m_pNegative)
		m_pNegative->DrawSolidPolygon();

}

void RSolidBspNode::DrawSolidPolygonWireframe()
{
	//	if(!isInViewFrustum(&m_bb,RGetViewFrustum())) return;

	if(nPolygon && m_bSolid)
	{
		rvector v[4];
		for(int i=0;i<nPolygon;i++)
		{
			memcpy(v,pVertices[i*3],3*sizeof(rvector));
			v[3]=v[0];

			RGetDevice()->DrawPrimitiveUP(D3DPT_LINESTRIP,3,&v,sizeof(rvector));
		}
	}

	if(m_pPositive)
		m_pPositive->DrawSolidPolygonWireframe();

	if(m_pNegative)
		m_pNegative->DrawSolidPolygonWireframe();

}

void RSolidBspNode::DrawSolidPolygonNormal()
{
	//	if(!isInViewFrustum(&m_bb,RGetViewFrustum())) return;

	if(nPolygon && m_bSolid)
	{
		rvector v[2];
		for(int i=0;i<nPolygon;i++)
		{
			rvector center=1.f/3.f*(pVertices[i*3]+pVertices[i*3+1]+pVertices[i*3+2]);

			v[0]=center;
			v[1]=center+pNormals[i]*50.f;
			RGetDevice()->DrawPrimitiveUP(D3DPT_LINESTRIP,1,&v,sizeof(rvector));
		}
	}

	if(m_pPositive)
		m_pPositive->DrawSolidPolygonNormal();

	if(m_pNegative)
		m_pNegative->DrawSolidPolygonNormal();

}

void RSolidBspNode::DrawPos(rvector &pos)
{
	if(nPolygon) {
		if((timeGetTime()/500) %2 == 0) {
			RGetDevice()->SetFVF( D3DFVF_XYZ );
			RGetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE );
			RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			RGetDevice()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			RGetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			RGetDevice()->SetRenderState(D3DRS_TEXTUREFACTOR ,   0xff00ffff);
			RGetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,nPolygon,pVertices,sizeof(rvector));

			float fSize=10.f;
			RGetDevice()->SetRenderState(D3DRS_POINTSIZE,   *(DWORD*)&fSize);
			RGetDevice()->DrawPrimitiveUP(D3DPT_POINTLIST,nPolygon*3,pVertices,sizeof(rvector));

			//			float fArea=GetArea(pVertices[0],pVertices[1],pVertices[2]);
		}
		return;
	}

	if(D3DXPlaneDotCoord(&m_Plane,&pos)<0) {
		if(m_pNegative)
			m_pNegative->DrawPos(pos);
	}else {
		if(m_pPositive)
			m_pPositive->DrawPos(pos);
	}
}

void RSolidBspNode::DrawPlaneVertices(rplane &plane)
{
	if(nPolygon) {
		if((timeGetTime()/500) %2 == 0) {
			RGetDevice()->SetFVF( D3DFVF_XYZ );
			RGetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE );
			RGetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			RGetDevice()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			RGetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			float fSize=10.f;
			RGetDevice()->SetRenderState(D3DRS_POINTSIZE,   *(DWORD*)&fSize);
			for(int i=0;i<nPolygon*3;i++)
			{
				if(fabs(D3DXPlaneDotCoord(&plane,pVertices+i))<5.f)
					RGetDevice()->DrawPrimitiveUP(D3DPT_POINTLIST,1,pVertices+i,sizeof(rvector));
			}
		}
		return;
	}

	if(m_pNegative)
		m_pNegative->DrawPlaneVertices(plane);
	if(m_pPositive)
		m_pPositive->DrawPlaneVertices(plane);
}

void RSolidBspNode::ConstructBoundingBox()
{
	if(!m_pPositive && !m_pNegative) return;

	if(nPolygon)
	{
		int i,j;
		m_bb.vmin=pVertices[0];
		m_bb.vmax=pVertices[0];
		for(i=1;i<nPolygon*3;i++){
			for(j=0;j<3;j++)
			{
				m_bb.vmin[j]=min(m_bb.vmin[j],pVertices[i][j]);
				m_bb.vmax[j]=max(m_bb.vmax[j],pVertices[i][j]);
			}
		}
		return;
	}

	if(m_pPositive)
		m_pPositive->ConstructBoundingBox();

	if(m_pNegative)
		m_pNegative->ConstructBoundingBox();

	if(m_pPositive)
	{
		m_bb=m_pPositive->m_bb;
		if(m_pNegative)
			MergeBoundingBox(&m_bb,&m_pNegative->m_bb);
	}else
	{
		if(m_pNegative)
			m_bb=m_pNegative->m_bb;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
//  ���⼭���� �浹üũ�� ���� �κ�

//  solid bsp + shift plane ��� ( ���� �β��� ������� �˻� )
//  Dynamic Plane Shifting BSP Traversal ( by Stan Melax ) ����


#define TOLERENCE 0.001f

bool RSolidBspNode::m_bTracePath = false;

// TODO : ����� ����ִ´�
#define MAX_DEPTH	256
rplane	m_SolidPlanes[MAX_DEPTH];		// ������ solid ��带 �̷�� ���� (���)

// ����� m_pNegative �� cross �Ǵ����� ����
bool IsCross(const rplane &plane,const rvector &v0,const rvector &v1,float *fParam)
{
#define CSIGN(x) ( (x)<-0.1? -1 : (x)>0.1? 1 : 0 )
	float dotv0=D3DXPlaneDotCoord(&plane,&v0);
	float dotv1=D3DXPlaneDotCoord(&plane,&v1);
	int signv0=CSIGN(dotv0),signv1=CSIGN(dotv1);

	if (signv0==1 && signv1==1) return false;

	rvector dir=v1-v0;
	if(D3DXPlaneDotNormal(&plane,&dir)>0) {
		*fParam=-1;
	}else
		if(signv0==0) {

			if(signv1==0)
				*fParam=1;
			else
				*fParam=0;
		}else
			if(dotv0*dotv1<=0) {
				// ������ ����� �����ϴ� ���
				*fParam = ( dotv0 / ( dotv0-dotv1));
				*fParam = min(1,*fParam);
			}else
			{
				*fParam=-1;
			}

			return true;
}

bool RSolidBspNode::GetColPlanes_Recurse(int nDepth)
{
	bool bHit=false;

	if(!m_pPositive && !m_pNegative) {	// leaf
		if(m_bSolid) {

//			_ASSERT(nDepth<MAX_DEPTH);

			bool bInSolid=true;
			for(int i=0;i<nDepth;i++) {
				rplane *pPlane=m_SolidPlanes+i;
				float dotv0=D3DXPlaneDotCoord(pPlane,&m_ColOrigin);
				if(dotv0>-0.1f) {
					bInSolid=false;
					break;
				}
			}
//			_ASSERT(!bInSolid);	// �̹� solid ���� ���� �ִ� ����
 
			rvector dir=m_ColTo-m_ColOrigin;

			// solid ������ ��� ������ �������� ã�´�
			float fMaxParam = 0;
			for(int i=0;i<nDepth;i++) {
				rplane *pPlane=m_SolidPlanes+i;

				float dotv0=D3DXPlaneDotCoord(pPlane,&m_ColOrigin);
				float dotv1=D3DXPlaneDotCoord(pPlane,&m_ColTo);

				// solid ������ �̷�� ����� ���󰡸� �浹�� �ƴϴ�
				if(fabs(dotv0)<0.1f && fabs(dotv1)<0.1f) {
					m_pOutList->Add(*pPlane);
					return false;	
				}

				if(D3DXPlaneDotNormal(pPlane,&dir)>0) continue;	// �޸� ����

				if(0<dotv0 && dotv0<dotv1) continue; // solid ���� �־����� �ִ� ��� ����
				if(fabs(dotv0-dotv1)<0.01f) continue; // ���� �����ϰ� �����̴� ���
				float fParam = ( dotv0 / ( dotv0-dotv1));

				fMaxParam = max(fMaxParam,fParam);
			}

			rvector colPos = m_ColOrigin+(m_ColTo-m_ColOrigin)*fMaxParam;
			
			float fDist = Magnitude(colPos-m_ColOrigin);
/*
			if(fDist>fImpactDist+0.1) 
				return false;	// �� �ָ��ִ°͵��� ���ʿ� ����.	// ���� ���װ��ִ� -_-;
*/
			int nCount=0;
			for(int i=0;i<nDepth;i++) {
				rplane *pPlane=m_SolidPlanes+i;
				if(D3DXPlaneDotNormal(pPlane,&dir)>0) continue;	// �޸� ����
				if(fabs(D3DXPlaneDotCoord(pPlane,&colPos))<0.1f) {
					m_pOutList->Add(*pPlane);
					if(fDist<fImpactDist)
					{
						fImpactDist = fDist;
						impact = colPos;
						impactPlane = *pPlane;
					}
					nCount++;
				}
			}

			return true;
		}
		return false;
	}

	float fShift;
	if(m_ColMethod==RCW_CYLINDER)
	{
		rvector rimpoint=rvector(-m_Plane.a,-m_Plane.b,0);
		if(IS_ZERO(rimpoint.x) && IS_ZERO(rimpoint.y))
			rimpoint.x=1.f;
		Normalize(rimpoint);
		rimpoint= rimpoint*m_fColRadius;
		rimpoint.z +=  (m_Plane.c < 0 ) ? m_fColHeight : -m_fColHeight;
		fShift=-D3DXPlaneDotNormal(&m_Plane,&rimpoint);
	}else
	{
		fShift=m_fColRadius;
	}

	rplane shiftPlane=m_Plane;
	shiftPlane.d=m_Plane.d-fShift;

	float fCurParam;
	if(m_pNegative!=NULL && IsCross(shiftPlane,m_ColOrigin,m_ColTo,&fCurParam))
	{
		m_SolidPlanes[nDepth]=shiftPlane;
		bHit=m_pNegative->GetColPlanes_Recurse(nDepth+1);
	}

	shiftPlane.d=m_Plane.d+fShift;
	shiftPlane=-shiftPlane;

	if(m_pPositive!=NULL && IsCross(shiftPlane,m_ColOrigin,m_ColTo,&fCurParam))
	{
		m_SolidPlanes[nDepth]=shiftPlane;
		bHit|=m_pPositive->GetColPlanes_Recurse(nDepth+1);
	}

	return bHit;
}

bool RSolidBspNode::GetColPlanes_Sphere(RImpactPlanes *pOutList,const rvector &origin,const rvector &to,float fRadius)
{
	m_ColMethod = RCW_SPHERE;
	m_ColOrigin = origin;
	m_ColTo = to;
	m_pOutList = pOutList;

	m_fColRadius = fRadius;

	fImpactDist=FLT_MAX;
	impact=rvector(0,0,0);

	return GetColPlanes_Recurse();    
}

bool RSolidBspNode::GetColPlanes_Cylinder(RImpactPlanes *pOutList,const rvector &origin,const rvector &to,float fRadius,float fHeight)
{
	m_ColMethod = RCW_CYLINDER;
	m_ColOrigin = origin;
	m_ColTo = to;
	m_pOutList = pOutList;

	m_fColRadius = fRadius;
	m_fColHeight = fHeight;

	fImpactDist=FLT_MAX;
	impact=rvector(0,0,0);

	return GetColPlanes_Recurse();    
}


rvector checkwallorigin;
rvector checkwalldir;

// ���𰡿� �ɷ��� 1case Ȥ�� 2case �� ���� �� ������� ���´�
bool RSolidBspNode::CheckWall2(RSolidBspNode *pRootNode,RImpactPlanes &impactPlanes,rvector &origin,rvector &targetpos,float fRadius,float fHeight,RCOLLISIONMETHOD method)
{
	checkwallorigin=origin;

	if(m_bTracePath) {	// ������ ����ϴ� �κ�
		rvector dif=targetpos-origin;
		mlog(" from ( %3.5f %3.5f %3.5f ) by ( %3.3f %3.3f %3.3f ) "
			,origin.x,origin.y,origin.z,dif.x,dif.y,dif.z);
	}

	bool bIntersectThis;

	impactPlanes.erase(impactPlanes.begin(),impactPlanes.end());

	if(method==RCW_SPHERE)
		bIntersectThis=pRootNode->GetColPlanes_Sphere(&impactPlanes,origin,targetpos,fRadius);
	else
		bIntersectThis=pRootNode->GetColPlanes_Cylinder(&impactPlanes,origin,targetpos,fRadius,fHeight);

	RImpactPlanes::iterator i;
/*
	// ���¹���� ������� ���� ���� �����Ѵ�
	rvector godir=targetpos-origin;
	for(i=impactPlanes.begin();i!=impactPlanes.end();)
	{
		rplane p=*i;
		if(D3DXPlaneDotNormal(&p,&godir)>TOLERENCE)
		{
			if(m_bTracePath) {
				float fDist = D3DXPlaneDotCoord(&p,&origin);
				mlog(" del_back{d = %3.3f [%3.3f %3.3f %3.3f %3.3f]}",fDist,p.a,p.b,p.c,p.d);
			}
			i=impactPlanes.erase(i)	;
		}else
			i++;
	}
*/

	if(m_bTracePath) {	// ������ ����ϴ� �κ�
		if(impactPlanes.size())
		{
			mlog(" :::: %d planes ",impactPlanes.size());
			for(RImpactPlanes::iterator i=impactPlanes.begin();i!=impactPlanes.end();i++)
			{
				rplane p=*i;
				float fDist = D3DXPlaneDotCoord(&p,&origin);
				mlog(" d = %3.3f [%3.3f %3.3f %3.3f %3.3f] ",fDist,p.a,p.b,p.c,p.d);
			}
		}
	}

	if(impactPlanes.size())
	{
		bool bFound=false;
		rplane simulplanes[100];	// �ִ� ���ÿ� �´�� ���� ����

		rvector diff=targetpos-origin;
		float fMinProjDist;
		float fMinDistToOrigin;

		for(i=impactPlanes.begin();i!=impactPlanes.end();i++)
		{
			rplane plane = *i;

			if(plane==rplane(0,0,0,0)) 
			{
				_ASSERT(FALSE);	// ���� �̷����� �߻����� �ʴ´�.
				i=impactPlanes.erase(i);
				continue;	// �Ƹ��� ���ӿ� �� ���� Ȯ���� ����
			}

			float fDistToOrigin = D3DXPlaneDotCoord(&plane,&origin);		// origin ���� �������� �ִܰŸ�
			float fDistToTarget = D3DXPlaneDotCoord(&plane,&targetpos);		// origin ���� �������� �ִܰŸ�

			// 
			if(fDistToOrigin>-.1f && fDistToTarget>-.1f )		//  �������� ������ ����� ���
			{
				continue;
			}


			float fProjDist = -D3DXPlaneDotNormal(&plane,&diff);			// diff �� ����� �������� projection �� ����

			if(fDistToOrigin<=0 && fDistToTarget<fDistToOrigin) { // �̹� ����� �ڿ� �ִ� ����̴�.
				fProjDist = 1;
				fDistToOrigin = 0;
			}

			if(fProjDist==0) {  // ������ �����Ѱ���̴�.
				// ������ 0�� ��쿡�� floating point exception �� �Ͼ��.
				fProjDist = 1;
				fDistToOrigin = 1;
			}

			// ���� �����ϴ� ������ origin + (fDistToOrigin/fProjDist) * diff �̴�.
			// (fDistToOrigin/fProjDist) �� �ּҰ� �Ǵ� ����� ã���� ���� ���� �´�� ���̴�.
			if(!bFound)
			{
				bFound = true;
				fMinProjDist = fProjDist;
				fMinDistToOrigin = fDistToOrigin;
			}else
			{
				// ��Ȯ���� ���̱� ���� ���� ������ ��ſ� �̷��� ���Ѵ�. (Gaming Programming Gems 3 �� 2.2 ���ͺм� ����)
				//				if( fDistToOrigin/fProjDist < fMinDistToOrigin/fMinProjDist )
				if( fDistToOrigin * fMinProjDist < fMinDistToOrigin * fProjDist  )
				{
					fMinProjDist = fProjDist;
					fMinDistToOrigin = fDistToOrigin;
				}
			}
		}

		if(!bFound) {
			return false;
		}

		float fInter = max(0,min(1.f,fMinDistToOrigin/fMinProjDist));
		rvector currentorigin;
		currentorigin = origin + fInter * diff;

		targetpos = currentorigin;

		// �� origin ���������� �Ÿ��� ���� ����� ã�´�

		int nSimulCount=0;

		for(i=impactPlanes.begin();i!=impactPlanes.end();i++) 
		{
			//			if(fabs(fDist-i->first)<0.01f) { // ���ÿ� ��� ���̴�.
			rplane p=*i;
			if(fabs(D3DXPlaneDotCoord(&p,&currentorigin))<0.05) {
				simulplanes[nSimulCount++]=p;
			}
		}

	if(m_bTracePath) {
		mlog("%d simul ",nSimulCount);
	}

		return true;
	}

	// �ƹ��͵� �ɸ��°� ������ �׳� �����Ѵ�

	return false;
}

bool g_bchecktest=false;
rvector g_checkdebug;

// origin ���� targetpos �� �̵��ϴµ� �̲������� �����ؼ� targetpos �� �����ؼ� �������ش�.
bool RSolidBspNode::CheckWall(RSolidBspNode *pRootNode,rvector &origin,rvector &targetpos,float fRadius,float fHeight,RCOLLISIONMETHOD method,int nDepth,rplane *pimpactplane)
{
	checkwallorigin=origin;
	checkwalldir=targetpos-origin;
	Normalize(checkwalldir);

	bool bIntersectThis;

	RImpactPlanes impactPlanes;
	impactPlanes.erase(impactPlanes.begin(),impactPlanes.end());

	if(method==RCW_SPHERE)
		bIntersectThis=pRootNode->GetColPlanes_Sphere(&impactPlanes,origin,targetpos,fRadius);
	else
		bIntersectThis=pRootNode->GetColPlanes_Cylinder(&impactPlanes,origin,targetpos,fRadius,fHeight);

//	if(impactPlanes.size()) 
	{
		if(m_bTracePath) {	// ������ ����ϴ� �κ�
			mlog("\n");
			for(int i=0;i<nDepth;i++) mlog("    ");

			rvector dif=targetpos-origin;
			mlog("d%d from ( %3.5f %3.5f %3.5f ) by ( %3.3f %3.3f %3.3f ) ",nDepth
				,origin.x,origin.y,origin.z,dif.x,dif.y,dif.z);
		}

		/*
		if(g_bchecktest)
		{
			_ASSERT(Magnitude(g_checkdebug-origin)<1.f);
		}
		g_bchecktest=false;
		*/
	}

	RImpactPlanes::iterator i;

	/*
	// ���¹���� ������� ���� ���� �����Ѵ�
	rvector godir=targetpos-origin;
	for(i=impactPlanes.begin();i!=impactPlanes.end();)
	{
		rplane p=*i;
		if(D3DXPlaneDotNormal(&p,&godir)>TOLERENCE)
		{
			if(m_bTracePath) {
				float fDist = D3DXPlaneDotCoord(&p,&origin);
				mlog(" del_back{d = %3.3f [%3.3f %3.3f %3.3f %3.3f]}",fDist,p.a,p.b,p.c,p.d);
			}
			i=impactPlanes.erase(i)	;
		}else
			i++;
	}
	*/


	if(m_bTracePath) {	// ������ ����ϴ� �κ�
		if(impactPlanes.size())
		{
			mlog(" :::: %d planes ",impactPlanes.size());
			for(RImpactPlanes::iterator i=impactPlanes.begin();i!=impactPlanes.end();i++)
			{
				rplane p=*i;
				float fDist = D3DXPlaneDotCoord(&p,&origin);
				mlog(" d = %3.3f [%3.3f %3.3f %3.3f %3.3f] ",fDist,p.a,p.b,p.c,p.d);
			}
		}
	}

	if(impactPlanes.size())
	{
		bool bFound=false;
		rplane simulplanes[100];	// �ִ� ���ÿ� �´�� ���� ����

		rvector diff=targetpos-origin;
		float fMinProjDist = 1.f;
		float fMinDistToOrigin = 0.f;

		// ��󳻾��� ���� �߿� ���� ���� �´�� ����� ��󳽴�
		for(i=impactPlanes.begin();i!=impactPlanes.end();i++)
		{
			rplane plane = *i;

			if(plane==rplane(0,0,0,0)) 
			{
				_ASSERT(FALSE);
				i=impactPlanes.erase(i);
				continue;	// �Ƹ��� ���ӿ� �� ���� Ȯ���� ����
			}

			float fProjDist = -D3DXPlaneDotNormal(&plane,&diff);			// diff �� ����� �������� projection �� ����
			float fDistToOrigin = D3DXPlaneDotCoord(&plane,&origin);		// origin ���� �������� �ִܰŸ�

			if(fDistToOrigin<=-0.1) { // �̹� ����� �ڿ� �ִ� ����̴�.
				fProjDist = 1;
				fDistToOrigin = 0;
			}
			if(fProjDist==0) {  // ������ �����Ѱ���̴�.
				// ������ 0�� ��쿡�� floating point exception �� �Ͼ��.
				fProjDist = 1;
				fDistToOrigin = 1;
			}

			// ���� �����ϴ� ������ origin + (fDistToOrigin/fProjDist) * diff �̴�.
			// (fDistToOrigin/fProjDist) �� �ּҰ� �Ǵ� ����� ã���� ���� ���� �´�� ���̴�.
			if(!bFound)
			{
				bFound = true;
				fMinProjDist = fProjDist;
				fMinDistToOrigin = fDistToOrigin;
			}else
			{
				if( fDistToOrigin/fProjDist < fMinDistToOrigin/fMinProjDist )

// ��ȣ�� ����� �ϱ� ������ ����.. (�̵��� �ϳ��� -���� ������ �ε�ȣ�� ������ �ٲ��.)
// ��Ȯ���� ���̱� ���� ���� ������ ��ſ� �̷��� ���Ѵ�. (Gaming Programming Gems 3 �� 2.2 ���ͺм� ����)
//				if( fDistToOrigin * fMinProjDist < fMinDistToOrigin * fProjDist  )
				{
					fMinProjDist = fProjDist;
					fMinDistToOrigin = fDistToOrigin;
				}
			}
		}

		if(!bFound) {
			return false;
		}

		// ���� ���� �´�� ������ ������ ���Ѵ� (currentorigin)
		float fInter = max(0.f,min(1.f,fMinDistToOrigin/fMinProjDist));
		rvector currentorigin = origin + fInter * diff;

		if(nDepth==0 && pimpactplane)
			*pimpactplane=simulplanes[0];

		// �� origin ���������� �Ÿ��� ���� ����� ã�´�

		int nSimulCount=0;

		for(i=impactPlanes.begin();i!=impactPlanes.end();i++) 
		{
			//			if(fabs(fDist-i->first)<0.01f) { // ���ÿ� ��� ���̴�.
			rplane p=*i;
			if(fabs(D3DXPlaneDotCoord(&p,&currentorigin))<0.1) {
				simulplanes[nSimulCount++]=p;
			}
		}

		if(m_bTracePath) {
			mlog("fInter = %3.3f %d simul ",fInter,nSimulCount);
		}

		float fBestCase=0.f;
		rvector newtargetpos=currentorigin;

		// �� ���� Ÿ�� �̲������� ��찡 1case �̰�..
		// �� ���� ������ Ÿ�� �̲������� ��찡 2case �̴�.

		bool b1Case=false;
		rplane plane1case;
		{
			for(int i=0;i<nSimulCount;i++)
			{
				rplane plane=simulplanes[i];

				// �̲�����.. diff ���� ����� ������������ ��鿡�������� �Ÿ���ŭ ���Ѵ�

				// Ni �� ����� ��������
				rvector Ni=rvector(plane.a,plane.b,plane.c);

				// targetpos �� ������� ������ ���� ���������� ����
				rvector adjtargetpos =  targetpos  + Ni * -D3DXPlaneDotCoord(&plane,&targetpos);

				// ���� ������
				if(m_bTracePath) {
					mlog("\n    check 1 : ");
				}
				rvector checktargetpos = adjtargetpos;
				CheckWall2(pRootNode,impactPlanes,currentorigin,checktargetpos,fRadius,fHeight,method);
				float fDot=DotProduct(checkwalldir,checktargetpos-origin);
				if(m_bTracePath) {
					mlog("dot = %3.3f ",fDot);
				}

				if(fDot>fBestCase)
				{
					b1Case=true;
					fBestCase=fDot;
					newtargetpos=adjtargetpos;
					plane1case=plane;
				}
			}
		}

//		if(!b1Case)

		bool b2Case=false;
		{
			// �ΰ��� ��鿡 ���ÿ� ��� ����̴�.  (��������� ���� �̹� ����)
			// �̷����� �ΰ��� ��鿡 ���ÿ� ������ �������� �����Ѵ�.


			rvector dif=targetpos-currentorigin;
			for(int i=0;i<nSimulCount;i++)
			{
				for(int j=i+1;j<nSimulCount;j++)
				{
					rvector aPoint,dir;
					if(GetIntersectionOfTwoPlanes(&dir,&aPoint,simulplanes[i],simulplanes[j]))
					{
						Normalize(dir);
						rvector aPointToTarget = targetpos-aPoint;
						rvector adjtargetpos =  aPoint + DotProduct(dir,aPointToTarget)*dir;
					

						// ���� ������
						if(m_bTracePath) {
							mlog("\n   check 2 : ");
						}
						rvector checktargetpos = adjtargetpos;
						CheckWall2(pRootNode,impactPlanes,currentorigin,checktargetpos,fRadius,fHeight,method);
						float fDot=DotProduct(checkwalldir,checktargetpos-origin);
						//						float fDot=DotProduct(checkwalldir,adjtargetpos-origin);
						if(fDot>fBestCase)
						{
							fBestCase=fDot;
							newtargetpos=adjtargetpos;
							b2Case=true;
						}
					}


					/*
					rvector dir;
					CrossProduct(&dir,rvector(simulplanes[i].a,simulplanes[i].b,simulplanes[i].c),
						rvector(simulplanes[j].a,simulplanes[j].b,simulplanes[j].c));
					if(Magnitude(dir)<TOLERENCE) continue;
					
					Normalize(dir);

					rvector prjdif=dir*DotProduct(dir,dif);

					// targetpos �� ���ÿ� ������ ���⺤�ͷ� ������ ���� ������
					rvector adjtargetpos=currentorigin+prjdif;
					*/

				}
			}
		}

		if(m_bTracePath) {
			mlog("\n");
			if(b2Case)
				mlog("2 case ");
			else if(b1Case)
			{
				mlog("1 case ( %3.3f %3.3f %3.3f %3.3f)",plane1case.a,plane1case.b,plane1case.c,plane1case.d);
			}
		}

		rvector newdir=targetpos-currentorigin;
		if(DotProduct(newdir,checkwalldir)<-0.01) {
		if(m_bTracePath) {
			mlog(" -> over dir.");
		}
			targetpos=currentorigin;
			return false;
		}

		// ����������ε� ������ �Ұ���
		if(!b2Case && !b1Case) {
			targetpos = currentorigin;
			if(m_bTracePath) {
				mlog("@ %3.3f = final ( %3.3f %3.3f %3.3f )",fInter,targetpos.x,targetpos.y,targetpos.z);
				}

			g_checkdebug = targetpos;
			g_bchecktest=true;
			return true;
		}

		rvector checktargetpos = newtargetpos;
		CheckWall2(pRootNode,impactPlanes,currentorigin,checktargetpos,fRadius,fHeight,method);
		for(i=impactPlanes.begin();i!=impactPlanes.end();i++)
		{
			rplane plane = *i;
			float fDist = D3DXPlaneDotCoord(&plane,&newtargetpos);
			if(fDist<-0.05) {
				targetpos = checktargetpos;
				if(m_bTracePath) {
					mlog("! final ( %3.3f %3.3f %3.3f )",targetpos.x,targetpos.y,targetpos.z);
				}
				return true;
			}
		}

		targetpos = newtargetpos;
		if(m_bTracePath) {
			mlog("!! final ( %3.3f %3.3f %3.3f )",targetpos.x,targetpos.y,targetpos.z);
		}
		return true;
	}

	// �ƹ��͵� �ɸ��°� ������ �׳� �����Ѵ�

	return false;
}