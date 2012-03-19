//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* DlgCharList.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIDialog.h"
#include "TSingleton.h"
#include "SceneEffect.h""
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CDlgCharList : public CUIDialog, public TSingleton<CDlgCharList>
{
public:
	void OnControlRegister();
	virtual void SetVisible(bool bVisible);
	void OnFrameMove(double fTime, float fElapsedTime);
	void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	void OnSize(const CRect<int>& rc);
	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	void OnBtnOk();
	void OnBtnBack();
	void OnBtnCharCreate();
	void OnBtnCharDelete();
protected:
	CSceneEffect		m_SceneEffect;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------