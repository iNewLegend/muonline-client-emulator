#pragma once
#include "UIMainDialog.h"
#include "DlgToolBar.h"
#include "DlgFile.h"
#include "DlgFPS.h"
#include "UIWorldEditorDisplay.h"

#include "DlgRegister.h"
#include "DlgModelController.h"
#include "DlgMaterial.h"
#include "ModelDisplay.h"

#include "DataPlugsMgr.h"
//#include "DlgFaceDetect.h"
#include "DlgController.h"
#include "DlgModelController.h"

class CDlgMainEditor : public CUIMainDialog
{
public:
	CDlgMainEditor();
	~CDlgMainEditor();
public:
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	virtual void OnFrameMove(double fTime, float fElapsedTime);

	void OnBtnNewFile();
	void OnBtnOpenFile();
	void OnBtnSaveFile();
	void OnBtnToolbar();
	void OnBtnHelp();
	void OnBtnTerrainResize();

	void OnFileNew();
	void OnFileOpen();
	void OnFileSave();
	void OnFileCancel();

	void OnBtnShowController();
	void OnBtnFaceDetect();

	void OnRadioModel();
	void OnRadioWorld();
public:
	CUIWorldEditorDisplay& getDisplay();
	CModelDisplay& getModelDisplay();
	CDlgMaterial& getMaterialDialog();
	CDlgModelController& getDlgModelController(){return m_DlgModelController;}
private:
	void OnUpdateShow();
	void updateDisplay();

	CUIButton		m_BtnShow;
	//CDlgFaceDetect	m_DlgFaceDetect;

	CUIStatic	m_StaticPosX;
	CUIStatic	m_StaticPosY;
	CUIStatic	m_StaticPosHeight;
	CUIStatic	m_StaticFPS;
	CUIStatic	m_StaticInfo;
	CModelDisplay			m_ModelDisplay;
	CUIWorldEditorDisplay	m_WorldEditorDisplay;
	CDlgController	m_DlgController;
	CDlgModelController	m_DlgModelController;
	CDlgToolbar		m_DlgToolbar;
	CUIDialog		m_DlgHelp;
	CDlgFile		m_DlgFile;
	//CDlgFPS			m_DlgFPS;
	CDlgRegister	m_DlgRegister;

	CDataPlugsMgr	m_DataPlugsMgr;


	CDlgMaterial m_DlgMaterialEdit;

	CUICheckBox m_CheckBoxBones;
	CUICheckBox	m_CheckBoxBounds;
	CUICheckBox m_CheckBoxModel;
	CUICheckBox m_CheckBoxWireframe;
	CUICheckBox m_CheckBoxMaterial;
	CUICheckBox	m_CheckBoxParticles;
};