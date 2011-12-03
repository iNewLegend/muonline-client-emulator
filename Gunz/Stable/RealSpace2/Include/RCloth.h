#ifndef _RCLOTH_H
#define _RCLOTH_H

//////////////////////////////////////////////////////////////////////////
//	RCloth - Base Class
//	Magicbell
//  [10/21/2003]
//////////////////////////////////////////////////////////////////////////
//#pragma once

// Includes
#include "vector"
#include "RMesh.h"
#include "RBoundary.h"

// Structures
typedef struct constraint {
	int refA; 
	int refB;
	float restLength;
}
sConstraint;

// Declare
struct rvector;
struct RVertex;

#define	CLOTH_VALET_ONLY    0x00
#define CLOTH_HOLD			0x01
#define CLOTH_FORCE			0x02
#define CLOTH_COLLISION		0x04

class RCloth
{
public:
	RCloth(void);
	virtual ~RCloth(void);

protected:
	
	virtual void accumulateForces();			// �� ��� - �߷� �̿ܿ� ������ �ʹǸ� ������ ���� ����.. ���� ����Ǵ� ��Ȳ ����

	virtual void varlet();						// Interpolation bet. current frame and last frame

	virtual void satisfyConstraints();			// satisfy constraints - �浹 �� ���� ������ ������Ų��

public:
	
	virtual void update();						// update - call every frame
	virtual void render();						// render
	
	virtual void UpdateNormal();				// update normal

	void addCollisionObject( RBoundary* co_ );	// add collision object

	void	SetAccelationRatio( float t_ ) { m_AccelationRatio = min( max( t_, 0 ), 1 ); }
	float	GetAccelationRatio() const { return m_AccelationRatio;	}

	void	SetTimeStep( float t_ ) { m_fTimeStep = t_; }
	float	GetTimeStep() const { return m_fTimeStep; }

	void	SetNumIteration( int n )  { m_nCntIter	= n; }
	int		GetNumIteration( ) const { return m_nCntIter; }

protected:

	int		m_nCntP;							// number of partices
	int		m_nCntC;							// number of Constraints

	int		m_nCntIter;							// Number of Iteration - more iteration is more accurate and slow 	-	!Adjustable in Realtime for Debug version
	float	m_fTimeStep;						// Time Steps	-	!Adjustable in Realtime for Debug version
	float	m_AccelationRatio;					// Ratio of Accelation

	
	rvector*		m_pX;						// Current Position
	rvector*		m_pOldX;					// Last Position
	rvector*		m_pForce;					// Force ( include Gravity )
	int*			m_pHolds;					// be Holded Particles
	float*			m_pWeights;					// weight list 
	rvector*		m_pNormal;					// normal

	sConstraint*	m_pConst;					// Constraints


	
	COList		mCOList;						// Destination Collision Object List

	//////////////////////////////////////////////////////////////////////////
	//	<<<	Rendering Primitives >>>
	
	int			m_nNumVertices;
	RVertex*	m_pVertices;

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9	m_pIndexBuffer;
};

// �ð��� ���� �ٶ��� ����� �ִ� �ٶ� ������

enum WIND_TYPE
{
	NO_WIND = 0,
	RANDOM_WIND,	
	CALM_WIND,					//���
	LIGHT_AIR_WIND,				//�ǹٶ�
	SLIGHT_BREEZE_WIND,			//���ǹٶ�
	GENTLE_BREEZE_WIND,			//���ٶ�
	MODERATE_BREEZE_WIND,		//�ǵ�ٶ�
	FRESH_BREEZE_WIND,			//���ٶ�
	STRONG_BREEZE_WIND,			//�ȹٶ�
	NEAR_GALE_WIND,				//���ٶ�
	GALE_WIND,					//ū�ٶ�
	STRONG_GALE_WIND,			//ū���ٶ�
	STROM_WIND,					//���ٶ�
	VIOLENT_STROM_WIND,			//�չٶ�
	HURRICANE_WIND,				//�Ͼ��̹ٶ�
	NUM_WIND_TYPE,		
};


class RWindGenerator
{
public:
	RWindGenerator():m_WindDirection(0,0,0), m_WindPower(0.f), m_Time(0), m_WindType(RANDOM_WIND), m_ResultWind(0,0,0), m_bFlag(false), m_fTemp1(0), m_fTemp2(0) {}
	virtual ~RWindGenerator() {}

public:

	rvector	GetWind() { return m_ResultWind; }

	void		Update( DWORD time );

	void		SetWindDirection( rvector& inDir ) { m_WindDirection	= inDir; }
	void		SetWindPower( float inPower ) { m_WindPower	= inPower; }
	void		SetDelayTime( DWORD inDelay ) { m_DelayTime	= inDelay; }
	void		SetWindType( WIND_TYPE type ) { m_WindType	= type;	}

protected:

	rvector		m_WindDirection;
	float		m_WindPower;
	DWORD		m_Time;
	rvector		m_ResultWind;
	WIND_TYPE	m_WindType;
	bool		m_bFlag;
	float		m_fTemp1, m_fTemp2;
	DWORD		m_DelayTime;
};


#endif