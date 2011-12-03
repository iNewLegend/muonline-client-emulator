#ifndef ZActionKey_H
#define ZActionKey_H

#include "MEdit.h"
#include <set>

using namespace std;

typedef set<int> ActionKeyForbidSet;

class ZActionKeyLook;

/// ActionKey widget
// ���̴� ��� key ���� ZVIRTUALKEY ���̳�, header include �ϱ� �Ⱦ �� int �� ��
class ZActionKey : public MEdit{
	friend ZActionKeyLook;
protected:
	DECLARE_LOOK(ZActionKeyLook)
//	DECLARE_LOOK_CLIENT()

protected:
	bool				m_bReadyInput;
	int					m_nKey;
	int					m_nAltKey;	// �� �׼ǿ� 2���� Ű�� �����Ҽ� �ִ�

public:

	static ActionKeyForbidSet m_ForbidKey;

protected:
//	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	virtual void OnSetFocus(void);
	virtual void OnReleaseFocus(void);

	static MGLOBALEVENTCALLBACK*	m_oldEventListener;
	static ZActionKey*				m_exclusiveActionKey;

public:
	ZActionKey(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL );

#define MINT_ACTIONKEY	"ActionKey"
	virtual const char* GetClassName(void){ return MINT_ACTIONKEY; }

	static bool		OnExclusiveEvent(MEvent* pEvent);

//	void GetActionKeyName(char* szActionKeyName);
	void GetActionKey(int* pKey);
	void GetActionAltKey(int* pKey);

	void ClearActionKey();
	void SetActionKey(int nKey);
	bool DeleteActionKey(int nKey);

	void UpdateText();	// text ������ �����Ѵ�

	//bool RegisterActionKey(int nActionID);
	//bool UnregisterActionKey(void);
	
	static void RegisterForbidKey(int nkey);
	bool IsForbidKey(int nkey);

};


#endif