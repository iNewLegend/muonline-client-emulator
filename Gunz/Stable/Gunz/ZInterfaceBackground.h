#ifndef ZINTERFACEBACKGROUND_H
#define ZINTERFACEBACKGROUND_H

#include "RBspObject.h"
using namespace RealSpace2;

#define LOGIN_SCENE_FIXEDSKY	0			// �ϴÿ� ī�޶� ����
#define LOGIN_SCENE_FALLDOWN	1			// ī�޶� �������鼭 �ΰ� ����
#define LOGIN_SCENE_FIXEDCHAR	2			// ĳ�������� ī�޶� ����
#define LOGIN_SCENE_SELECTCHAR	3			// ĳ���� ���� ��

class ZInterfaceBackground{
private:
	RBspObject*	m_pLogin;					// ���
	ZMapDesc* m_pMapDesc;

	rmatrix		m_matWorld;					// ���� ��

	int			m_nSceneNumber;				// ��� ��ȣ
	
	rvector		m_vCamPosSt;				// ī�޶� ���� ��ġ
	rvector		m_vCamPosEd;				// ī�޶� ���� ��ġ
	rvector		m_vCamDirSt;				// ī�޶� ���� ����
	rvector		m_vCamDirEd;				// ī�޶� ���� ��ġ
	rvector		m_vCharPos;					// ĳ���� ��ġ
	rvector		m_vCharDir;					// ĳ���� ����

	DWORD		m_dwClock;

//	bool		m_bShowMaietLogo;			// Maiet logo ����


protected:
	void SetFogState( float fStart, float fEnd, unsigned long int color);


public:
	ZInterfaceBackground( void);
	virtual ~ZInterfaceBackground( void);

	RBspObject*	GetChurchEnd() { return m_pLogin; }
	int GetScene() { return m_nSceneNumber; }
	void SetScene(int nSceneNumber);

	void LoadMesh( void);
	void Free(void);
	void Draw(void);

	void OnUpdate(float fElapsed);
	void OnInvalidate();
	void OnRestore();

	rvector& GetCharPos() { return m_vCharPos; }
	rvector& GetCharDir() { return m_vCharDir; }
};


#endif
