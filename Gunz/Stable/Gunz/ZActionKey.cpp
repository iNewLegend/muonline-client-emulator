#include "stdafx.h"
#include "ZActionKey.h"
#include "Mint.h"
#include "ZInput.h"
#include "ZOptionInterface.h"


class ZActionKeyLook : public MEditLook{
public:
	virtual void OnFrameDraw(MEdit* pEdit, MDrawContext* pDC);
	virtual void OnDraw(MEdit* pEdit, MDrawContext* pDC);
};

void ZActionKeyLook::OnFrameDraw(MEdit* pEdit, MDrawContext* pDC)
{
	MRECT r = pEdit->GetInitialClientRect();
	ZActionKey* pActionKey = (ZActionKey*)pEdit;	// -_-;
	MCOLOR color = pActionKey->IsFocus() ? MCOLOR(200,200,200,255) : MCOLOR(100,100,100,255);
	pDC->SetColor(color);
	pDC->Rectangle(r);
}

void ZActionKeyLook::OnDraw(MEdit* pEdit, MDrawContext* pDC) 
{
	MEditLook::OnDraw(pEdit,pDC,false);
}


ActionKeyForbidSet ZActionKey::m_ForbidKey;

ZActionKeyLook ZActionKey::m_DefaultLook;	
ZActionKeyLook* ZActionKey::m_pStaticLook = &ZActionKey::m_DefaultLook;

ZActionKey::ZActionKey(const char* szName, MWidget* pParent, MListener* pListener)
: MEdit(szName, pParent, pListener ), m_bReadyInput(false), m_nKey(-1), m_nAltKey(-1)
{
	LOOK_IN_CONSTRUCTOR();
}

void ZActionKey::OnDraw( MDrawContext* pDC )
{
	if( GetLook() != NULL )
		GetLook()->OnDraw( this, pDC );
}

/*
void ZActionKey::GetActionKeyName(char* szActionKeyName)
{
	const char* szKeyName = Mint::GetInstance()->GetActionKeyName(m_nKey);
	if(szKeyName!=NULL) strcpy(szActionKeyName, szKeyName);
	else strcpy(szActionKeyName, "N/A");
}
*/

void ZActionKey::GetActionKey(int* pKey)
{
	if(pKey!=NULL) *pKey = m_nKey;
}

void ZActionKey::GetActionAltKey(int* pKey)
{
	if(pKey!=NULL) *pKey = m_nAltKey;
}

void ZActionKey::ClearActionKey()
{
	m_nKey = -1;
	m_nAltKey = -1;

	SetText(ZInput::GetUndefinedKeyName());
}

void ZActionKey::UpdateText()
{
	char szBuffer[256] = "";
	ZGetInput()->GetInputKeyName(m_nKey, szBuffer,sizeof(szBuffer));

	if(m_nAltKey!=-1)
	{
		char szActionKeyName[128];
		ZGetInput()->GetInputKeyName(m_nAltKey, szActionKeyName,sizeof(szActionKeyName));
		strcat(szBuffer,"  or  ");
		strcat(szBuffer,szActionKeyName);
	}

	SetText(szBuffer);

	SetStartPos(0);
}

void ZActionKey::SetActionKey(int nKey)
{
	if(nKey==-1) return;
	if(IsForbidKey(nKey))//��ϱ���Ű���..
		return;

	if(m_nKey==-1)	// �ƹ��͵� ������ �ȵǾ� ������ ���
	{
		m_nKey = nKey;
	}else if(m_nAltKey==-1 && nKey!=m_nKey)	// alt.key �� ������ �ȵǾ������� ���
		m_nAltKey = nKey;
	else if(m_nAltKey==nKey)	// �̹� �ִ� Ű�̸�, �������� ����
	{
		m_nKey = m_nAltKey;
		m_nAltKey = -1;
	}else if(m_nKey==nKey)
		m_nAltKey = -1;
	else// �׷��� ������ key�� ������ ���
	{
		m_nKey = m_nAltKey;
		m_nAltKey = nKey;
	}

	UpdateText();

	ZGetOptionInterface()->OnActionKeySet(this,nKey);
}

bool ZActionKey::DeleteActionKey(int nKey)
{
	bool bDeleted = false;

	if(m_nAltKey==nKey)
	{
		m_nAltKey = -1;
		bDeleted = true;
	}

	if(m_nKey==nKey)
	{
		m_nKey = m_nAltKey;
		m_nAltKey = -1;
		bDeleted = true;
	}

	return bDeleted;
}


/*
bool ZActionKey::RegisterActionKey(int nActionID)
{
	return ZGetInput()->RegisterActionKey(nActionID, m_nKey);
//	return Mint::GetInstance()->RegisterActionKey(nActionID, m_nKey);
}

bool ZActionKey::UnregisterActionKey(void)
{
	return ZGetInput()->UnregisterActionKey(m_nKey);
//	return Mint::GetInstance()->UnregisterActionKey(m_nKey);
}
*/

void ZActionKey::RegisterForbidKey(int nkey)
{
	m_ForbidKey.insert(nkey);
}

bool ZActionKey::IsForbidKey(int nkey)
{
	if(m_ForbidKey.empty())
		return false;

	if(m_ForbidKey.find(nkey)!=m_ForbidKey.end())
		return true;

	return false;
}

MGLOBALEVENTCALLBACK* ZActionKey::m_oldEventListener = NULL;
ZActionKey*	ZActionKey::m_exclusiveActionKey = NULL;

bool ZActionKey::OnExclusiveEvent(MEvent* pEvent)
{
	_ASSERT(pEvent->nMessage == Z_DIRECTINPUTKEY_MESSAGE);

	// ��Ŀ���� ���涧�� �̺�Ʈ (lbutton �̳� tab )�� �����Ѵ�
	if(!m_exclusiveActionKey->m_bReadyInput)
	{
		m_exclusiveActionKey->m_bReadyInput = true;
	}else
	{
		// �ѹ� �ԷµǾ����� ��.
//		if (pEvent->nKey != 1) // 1 -> ESC
		{
			m_exclusiveActionKey->SetActionKey(pEvent->nKey);
		}
		m_exclusiveActionKey->ReleaseFocus();
	}

	return true;
}

void ZActionKey::OnSetFocus(void)
{
	_ASSERT(m_exclusiveActionKey==NULL);

	m_exclusiveActionKey = this;
	m_bReadyInput = false;
	ZGetInput()->SetExclusiveListener(OnExclusiveEvent);
	ZGetGameInterface()->SetSkipGlobalEvent(true);
}

void ZActionKey::OnReleaseFocus()
{
	ZGetGameInterface()->SetSkipGlobalEvent(false);
	m_exclusiveActionKey->m_bReadyInput = false;
	ZGetInput()->SetExclusiveListener(NULL);
	m_exclusiveActionKey = NULL;
}
