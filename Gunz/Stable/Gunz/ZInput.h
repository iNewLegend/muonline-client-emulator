#pragma once

#include "ZActionDef.h"
#include <map>
#include "Mint.h"

using namespace std;

class ZDirectInput;

const int Z_DIRECTINPUTKEY_MESSAGE = 0x0f01;	// �������� �ʴ´�. ���� �߿����� �ʴ�.


typedef int ZVIRTUALKEY;

/*
ZVIRTUALKEY ���� ������ �����Ѵ�

Ű���� ��ĵ�ڵ�  0 ~ 255
���콺 ��ư		 256 ~ 511
���̽�ƽ ��ư	 512 ~
*/



/// first button, second ActionID
typedef map<ZVIRTUALKEY, int>	ZACTIONKEYMAP;

const int MAX_JOY_POV_COUNT		=	4;
const int MAX_JOY_BUTTON_COUNT	=	32;
const int MAX_JOY_VIRTUAL_BUTTON = MAX_JOY_POV_COUNT*4 + MAX_JOY_BUTTON_COUNT;

class ZInput
{
	/// Action Map
	ZACTIONKEYMAP	m_ActionKeyMap;

	bool			m_ActionKeyPressedTable[ZACTION_COUNT];
	
	bool			m_JoyPressedTable[MAX_JOY_VIRTUAL_BUTTON];

	ZDirectInput*	m_pDirectInput;

	MGLOBALEVENTCALLBACK* m_pEventListener;
	MGLOBALEVENTCALLBACK* m_pExclusiveListener;

	void Event(ZVIRTUALKEY key, bool bPressed);

	float			m_fRotationDeltaX;	// ������ ȸ���� ������
	float			m_fRotationDeltaY;

public:
	ZInput(ZDirectInput* pDI);
	virtual ~ZInput(void);
	
	static const char* GetUndefinedKeyName();

	void Update();

	void GetRotation(float* pfX,float* pfY);

	bool GetInputKeyName(ZVIRTUALKEY key, char* pBuffer, int nBuffer);

	void ExclusiveEvent(ZVIRTUALKEY key);

	void OnActionKey(int nActionID,bool bPressed);
	void OffActionKeys();

	/// �׼�Ű �߰�
	bool RegisterActionKey(int nActionID, ZVIRTUALKEY nKey);
	/// �׼�Ű ����
//	bool UnregisterActionKey(int nActionID);

	/// �׼�Ű Ŭ����
	void ClearActionKey();

	// �̺�Ʈ ������ ����
	void SetEventListener(MGLOBALEVENTCALLBACK pEventCallback);

	// ��� �Է��� ������ �ִ� ���� ������ ����
	void SetExclusiveListener(MGLOBALEVENTCALLBACK pEventCallback);

	bool IsActionKeyPressed(int nActionID)
	{
		if(nActionID<0 || nActionID>=ZACTION_COUNT){
			_ASSERT(FALSE);	// 0 ~ ACTIONKEYMAP_IDCOUNT-1 ���̰��̿��� �Ѵ�.
			return false;
		}
		return m_ActionKeyPressedTable[nActionID];
	}

	// �Է��� 0~1 ����, 0 = ��������. 1 = ���� �ִ�
	bool SetDeviceForcesXY(float fXForce, float fYForce);

};