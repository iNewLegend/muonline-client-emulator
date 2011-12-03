#include "stdafx.h"
#include "MAniBmButton.h"

void MAniBmButton::OnDownDraw(MDrawContext* pDC)
{
	// ���߿� Down Event���� ���� ����, �׶� ó���ؾ� �Ѵ�.
	m_pUpAnimation->Show(false);
	m_pOverAnimation->Show(true);
}

void MAniBmButton::OnUpDraw(MDrawContext* pDC)
{
	m_pUpAnimation->Show(true);
	m_pOverAnimation->Show(false);
}

void MAniBmButton::OnOverDraw(MDrawContext* pDC)
{
	m_pUpAnimation->Show(false);
	m_pOverAnimation->Show(true);
}

void MAniBmButton::OnSize(int w, int h)
{
	MRECT r = GetClientRect();
	m_pUpAnimation->SetSize(r.w, r.h);
	m_pOverAnimation->SetSize(r.w, r.h);
}

MAniBmButton::MAniBmButton(const char* szName, MWidget* pParent, MListener* pListener)
: MButton(szName, pParent, pListener)
{
	m_pUpAnimation = new MAnimation(NULL, NULL, this);
	m_pOverAnimation = new MAnimation(NULL, NULL, this);
	m_pUpAnimation->m_nPlayMode = MAPM_REPETITION;		// �켱 ���� �ݺ� �ִϸ��̼����� ó��
	m_pOverAnimation->m_nPlayMode = MAPM_REPETITION;
}

MAniBmButton::~MAniBmButton(void)
{
	delete m_pUpAnimation;
	delete m_pOverAnimation;
}

void MAniBmButton::SetUpAniBitmap(MAniBitmap* pAniBitmap)
{
	m_pUpAnimation->SetAniBitmap(pAniBitmap);
}

void MAniBmButton::SetOverAniBitmap(MAniBitmap* pAniBitmap)
{
	m_pOverAnimation->SetAniBitmap(pAniBitmap);
}
