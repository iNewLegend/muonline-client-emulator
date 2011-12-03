#ifndef _RLenzFlare_h
#define _RLenzFlare_h

#pragma once

#include "RBaseTexture.h"

_NAMESPACE_REALSPACE2_BEGIN

//////////////////////////////////////////////////////////////////////////
//  [11/11/2003]
//	2D Lenz Flare
//	Magicbell
//	- source 
//		1. lenz flare������ xml ���� - ������ ���ÿ�
//		2. lenz flare�� ���� �ؽ��ĵ�.. ( FLARE_ELEMENT_TYPE�� �ϳ� )
//	- ����
//		���� ���� �ܰ迡�� RCreateLenzFlare�� ȣ���Ѵ� - argument : xml ���� �̸�
//		RGetLenzFlare�� ȣ���Ͽ� Instance Pointer�� ��´�
//		Render�� ȣ���Ѵ� - argument1 : ���� ��ġ argument2: ���� ����ȵ�
//			����) �̶� �繰�� ������ ������ ������ �ʴ� ó���� Application���� �ؾ� ��
//					ȭ����� ������ �� Ŭ�������� ó���� �� �ִ�
//		���� ���� �ܰ迡�� RDestroyLenzFlare�� ȣ���Ѵ�
//
//////////////////////////////////////////////////////////////////////////

//	enum
enum FLARE_ELEMENT_TYPE
{
	FLARE_ELEMENT_SPHERE = 0,
	FLARE_ELEMENT_RING,
	FLARE_ELEMENT_SPOT,
	FLARE_ELEMENT_POLYGON,
	FLARE_ELEMENT_ETC,
	FLARE_ELEMENT_GROW,
	MAX_NUMBER_ELEMENT = 10,
};

#define MAX_LENZFLARE_NUMBER 1
#define MAX_NUMBER_TEXTURE MAX_NUMBER_ELEMENT

//	struct
struct sFlareElement
{
	int iType;
	float width, height;
	DWORD color;
	int iTextureIndex;
};

class RBspObject;

class RLenzFlare
{
protected:	
	//static int			msRef;						// ���� ������ ���� �÷����� ��ü ��
	static bool				mbIsReady;				// ����غ�
	static RLenzFlare		msInstance;			// �̱���
	static sFlareElement	msElements[MAX_NUMBER_ELEMENT];		
	int								miNumFlareElement;		// ���� �÷�� ���̴� �ٰ����� ����
	int*							miElementOrder;
	static RealSpace2::RBaseTexture		*msTextures[MAX_NUMBER_TEXTURE];
	//static LPDIRECT3DVERTEXBUFFER9		msVB;
	rvector						mLightList[MAX_LENZFLARE_NUMBER];
	int								miNumLight;

protected:
	static	bool	ReadXmlElement(MXmlElement* PNode,char* Path);
	bool		open( const char* pFileName_, MZFileSystem* pfs_ );
	bool		draw( float x_, float y_,				// ��ġ
					float width_, float height_,		// ũ��
					float alpha,							// ������	0~1
					DWORD color_,					// ��
					int	textureIndex_	);				// �ؽ��� �ε���
	

public:
	static	bool	Create( const char* filename_ );
	static	bool	Destroy();
	static	bool	IsReady();
	static	RLenzFlare* GetInstance(){ return &msInstance; 	}

	void		Initialize();									// �÷��� ������Ʈ�� ������ ������ ����
	bool		Render( rvector& light_pos,		// ����
					rvector& centre_,					// ȭ�� �߾�(ī�޶� ��ġ) Ȥ�� ĳ���� �� ��ġ(???)
					RBspObject* pbsp_ );				
	bool		Render( rvector& centre_ , RBspObject* pbsp_ );
	bool		SetLight( rvector& pos_ );
	void		Clear() { miNumLight	= 0;	}
	int			GetNumLight() const { return miNumLight; }
	rvector GetLightPos( int i ) const { return mLightList[i]; }

public:
	RLenzFlare(void);
	~RLenzFlare(void);
};

#ifndef __DEFINED_GLOBAL_LENZFLARE_METHOD__
#define __DEFINED_GLOBAL_LENZFLARE_METHOD__

//	���� �÷��� ����� ����ϱ� ���� ȣ��
bool	RCreateLenzFlare( const char* filename_ );

// ���̻� ���� �÷��� ����� ������� ������ ����
bool	RDestroyLenzFlare( );

// ���� �÷�� ��� �غ� �Ǿ����� ����
bool	RReadyLenzFlare( ) ;

// ���� �÷��� ��ü ������
//RLenzFlare*	RGetLenzFlare( bool* result = NULL);
RLenzFlare* RGetLenzFlare();

#endif

_NAMESPACE_REALSPACE2_END

#endif//_RLenzFlare_h