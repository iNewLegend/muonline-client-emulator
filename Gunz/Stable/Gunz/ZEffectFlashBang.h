#pragma once

#include "RMeshUtil.h"

//	sort�� �ʿ� ���� 
//	effectmanager���� ���� �׷��� ��� ���� loop�� �߻��� �� �ֱ� ������
//	������ update / draw ���ش�

class RealSoundEffectPlay;

class ZEffectFlashBang
{
protected:
	bool	mbActivated;			// �÷��ù��� ������ �޴����� ����
	bool	mbInitialSound;			// ���� �ʱ�ȭ�� �ʿ����� ����
	static bool	mbDrawCopyScreen;
	
	float	mfStartTime;
	float	mfPower;				// myCharacter�� ���� ��ġ���� �Ÿ� & �ü��� ���� ������ (0~1)
	float	mfDuration;				// ȿ�� ���� �ð�..
	float	mfVolumn;				// Def Sound �� ����fElapsedTime

	RTLVertex	mBuffer[4];
	//RealSoundEffectPlay* mpRSEffectPlay;
	int m_iChannel;
	LPDIRECT3DTEXTURE9	mpBlurTexture;
	LPDIRECT3DSURFACE9	mpBlurSurface;
	LPDIRECT3DSURFACE9	mpDepthBuffer;
	LPDIRECT3DSURFACE9	mpHoldDepthBuffer;
	LPDIRECT3DSURFACE9	mpHoldBackBuffer;

	static ZEffectFlashBang		msInstance;

public:
	void	SetBuffer();			// ���� ������ ȣ��
	void	ReleaseBuffer();		// ���� �ı��� ȣ��
	void	OnInvalidate();			
	void	OnRestore();			

	void	Init( rvector& ExplosionPos_, rvector playerPos_, rvector playerDir_, float Duration_ );
	void	End();	// ����Ʈ ���� �����Ѵ�

	void	Render();
	void	PlaySound();

	bool	IsActivated() const 
	{
		return mbActivated;	
	}

	static	ZEffectFlashBang* GetInstance() { return &msInstance; }
	static	void SetDrawCopyScreen( bool b_ ) { mbDrawCopyScreen = b_; }

public:
	ZEffectFlashBang();
	~ZEffectFlashBang();
};

//////////////////////////////////////////////////////////////////////////
//	Interface
//////////////////////////////////////////////////////////////////////////
ZEffectFlashBang* ZGetFlashBangEffect();
//	Duration_ : ȿ�� ���� �ð�(������ �Ӽ�), ExplosionPos_ : ���� ���
//	playerPos_ : myCharacter�� ��ġ, playerDir_ : ĳ���Ͱ� �ٶ󺸰� �ִ� ����
void	CreateFlashBangEffect( rvector& ExplosionPos_, rvector playerPos_, rvector playerDir_, float Duration_ );
bool	IsActivatedFlashBangEffect();				// ���ӿ��� ���� flashbangȿ���� �޴��� ���θ� üũ
void	ShowFlashBangEffect();
void	ReleaseFlashBangEffect();