#include "stdafx.h"
#include ".\tbcylinder.h"

TBCylinder::TBCylinder(void)
: vertexIndex(0), mpMesh(0)
{
}

TBCylinder::~TBCylinder(void)
{
	SAFE_DELETE_ARRAY( vertexIndex );
	SAFE_RELEASE(mVertexBuffer);
	SAFE_RELEASE(mIndexBuffer);
}

bool TBCylinder::setBCylinder( RealSpace2::RMeshNode * pMeshNode_, DWORD flag_ )
{
	vertexIndex = new int[pMeshNode_->m_point_num];
	rvector* v;

	miNumVertex = 0;

	for( int i = 0 ; i < pMeshNode_->m_point_num; ++i )
	{
		v = &pMeshNode_->m_point_list[i];
        
		if( RIGHT_LEG == flag_ )
		{
			if( v->x < 0 && v->y > -23 && v->y < 25 )
			{
				vertexIndex[miNumVertex++] = i;
			}
		}
		else if( LEFT_LEG == flag_ )
		{
			if( v->x > 0 && v->y > -23 && v->y < 25 )
			{
				vertexIndex[miNumVertex++] = i;
			}
		}
		else
		{
			return false;
		}
	}

	mpMesh = pMeshNode_;

	return true;
}

void TBCylinder::update( rvector* point_list_ )
{
	// ���� vertex���� x,y,z�� ���� ū ���� ���� ���� ���� ���Ѵ�
	// x�� ���� ū���� ���� �������� �߰����� base_center�� x��ǥ
	// ���������� z�� ���� ���Ѵ�
	// y�� ���� r���� ���� ���̴�
	// highet�� ���� ū ������ ���� ���� ���� ����
	// radius�� ����? ���� �����..����
    
	float min_x = 100000;
	float min_y = 100000;
	float min_z = 100000;
	float max_x = -100000;
	float max_y = -100000;
	float max_z = -100000;

//	rvector* pList = mpMesh->m_point_list;

	for( int i = 0 ; i < miNumVertex ; ++i )
	{
		if( point_list_[vertexIndex[i]].x < min_x )
		{
			min_x = point_list_[vertexIndex[i]].x;
		}
		if( point_list_[vertexIndex[i]].y < min_y )
		{
			min_y = point_list_[vertexIndex[i]].y;
		}
		if( point_list_[vertexIndex[i]].z < min_z )
		{
			min_z = point_list_[vertexIndex[i]].z;
		}

		if( point_list_[vertexIndex[i]].x > max_x )
		{
			max_x = point_list_[vertexIndex[i]].x;
		}
		if( point_list_[vertexIndex[i]].y > max_y )
		{
			max_y = point_list_[vertexIndex[i]].y;
		}
		if( point_list_[vertexIndex[i]].z > max_z )
		{
			max_z = point_list_[vertexIndex[i]].z;
		}
	}

	base_centre.x = (min_x + max_x) * 0.5;
	base_centre.z = (min_z + max_z) * 0.5;
	base_centre.y = min_y;

	height = max_y - min_y;
	radius = max( max_x - min_x, max_z - min_z ) * 0.5;
	radius = 6;
}

void TBCylinder::render(rmatrix* pWorld_)
{
	rmatrix init;
	rvector max = base_centre + rvector(radius, height, radius);
	rvector min = base_centre + rvector(-radius, 0, -radius);
	D3DXMatrixIdentity( &init );

	//RealSpace2::RGetDevice()->SetTransform( D3DTS_WORLD, pWorld_ );
	RealSpace2::RGetDevice()->SetTransform( D3DTS_WORLD, &init );
	draw_box( &init, max, min, 0xffff0000 );
	RealSpace2::RGetDevice()->SetTransform( D3DTS_WORLD, &init );
/*
	max.y += height;
	min.y += height;
	D3DXMatrixIdentity( &init );

	//RealSpace2::RGetDevice()->SetTransform( D3DTS_WORLD, pWorld_ );
	RealSpace2::RGetDevice()->SetTransform( D3DTS_WORLD, &init );
	draw_box( &init, max, min, 0xffff0000 );
	RealSpace2::RGetDevice()->SetTransform( D3DTS_WORLD, &init );
	//*/
}

//Helper Function : Shortest Distance Between a Line and a Point
// return distance
// true - shortest point is exist in a line , false - does not
// 
bool getDistanceBetLineSegmentAndPoint( rvector& lineStart_, rvector& lineEnd_, rvector& point_, rvector* intersection_, rvector* direction_, float& distance_ )
{
    rvector line = lineEnd_ - lineStart_;
	rvector cross_line = point_ - lineStart_;
	float line_length_square = D3DXVec3LengthSq( &line );

	float u = ( D3DXVec3Dot( &cross_line, &line ) ) / line_length_square ;

	if( u < 0.0f || u > 1.0f )
	{
		return false;
	}

	rvector intersection = lineStart_ + u*(lineEnd_ - lineStart_);

	if( intersection_ != NULL )
	{
		*intersection_ = intersection;
	}

	if( direction_ != NULL )
	{
		D3DXVec3Normalize( direction_, &( point_ - intersection ) );
	}

	distance_ = D3DXVec3Length( &( point_ - intersection ) );

	return true;
}

bool TBCylinder::isCollide( rvector& v_, rvector& n_, rvector& pos_ )
{
	// TODO : axis alligned cylinder���� �ش�Ǵ� �ڵ� ����
	rvector top_centre = base_centre;
	top_centre.y += height;
	// TODO : axis alligned cylinder���� �ش�Ǵ� �ڵ� �� 

	rvector intersection;
	rvector direction;
	float distance;
	
	if( !getDistanceBetLineSegmentAndPoint( base_centre, top_centre, v_, &intersection, &direction, distance ))
	{
		return false;
	}

	if( distance > radius )
	{
		return false;
	}

	if( 0 > D3DXVec3Dot( &n_, &direction ) )
	{
		direction *= -1;
	}

	pos_ = intersection + ( radius * n_ );

	return true;
}