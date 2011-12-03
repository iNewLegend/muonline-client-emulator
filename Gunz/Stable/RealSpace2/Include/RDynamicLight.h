#pragma once

#include "d3d9.h"
#include "RMeshUtil.h"
#include "vector"

using namespace std;

//////////////////////////////////////////////////////////////////////////
//	Enum
//////////////////////////////////////////////////////////////////////////
enum MAP_LIGHT_TYPE				// PRE DEFINED ����Ʈ �Ӽ�
{
	GUNFIRE,			// �� ����Ʈ
	EXPLOSION,		// ���� ����Ʈ
	MAP_LIGHT_NUM,
};

//////////////////////////////////////////////////////////////////////////
//	Define
//////////////////////////////////////////////////////////////////////////
#define MAX_EXPLOSION_LIGHT	10

//////////////////////////////////////////////////////////////////////////
//	Struct
//////////////////////////////////////////////////////////////////////////
struct sMapLightObj
{
	rvector		vLightColor;
	float		fRange;
	float		fLife;
	rvector		vPos;
	bool		bUsing;
	sMapLightObj();
};


typedef struct 
{
	int			iType;
	rvector		vLightColor;
	float		fRange;
	float		fLife;				// ����
} sMapLight;


//////////////////////////////////////////////////////////////////////////
//	RDynamicLightManager			
//	���̳��� ����Ʈ ������ - �̱���
//	�ǽð����� ����� ����Ʈ�� ���� ����
//////////////////////////////////////////////////////////////////////////
class RDynamicLightManager			
{
protected:
	static	RDynamicLightManager	msInstance;
	static	sMapLight 				msMapLightList[MAP_LIGHT_NUM];

	bool										mbGunLight;							
	sMapLightObj						mGunLight;			// ���� ����Ʈ�� �� ĳ������ �͸� ������ ����
	vector<sMapLightObj>			mExplosionLightList;	
    
	float								mTime;
	rvector							mvPosition;

	// �ɼ�
	int									miNumEnableLight;	//	������ ����Ʈ�� ����
	
public:
	bool	AddLight( MAP_LIGHT_TYPE light_type_, rvector& pos_ ); 
	void	Update();									// ������ ���� �� ��� ������
	void	Initialize();									// �ʱ�ȭ
	int		SetLight( rvector pos_ );					// pos_ : ĳ������ ���� ��ġ, return ���õ� ����Ʈ ����
	void	ReleaseLight();
	void	SetPosition( rvector& pos_ );				// ĳ���� ��ġ /// �ʿ����� ���

	bool	SetLight( rvector& pos_, int lightIndex_, float maxDistance_ );	//
	
	static	RDynamicLightManager* GetInstance()
	{
		return &msInstance;
	}
	static	sMapLight*	GetLightMapList()
	{
		return msMapLightList;
	}
	bool	IsThereLight( )
	{
		if( mbGunLight || mExplosionLightList.size() )
		{
			return true;
		}
		return false;
	}

public:
	RDynamicLightManager();
	~RDynamicLightManager();
};

// singleTon ������
RDynamicLightManager*	RGetDynamicLightManager();
sMapLight*				RGetMapLightList();