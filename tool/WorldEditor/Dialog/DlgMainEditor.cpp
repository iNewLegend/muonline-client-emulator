#include "DlgMainEditor.h"
#include "LumpFile.h"
#include "FileSystem.h"
#include "Camera.h"
#include "RegData.h"

CDlgMainEditor::CDlgMainEditor()
{
}

CDlgMainEditor::~CDlgMainEditor()
{	
}

CUIWorldEditorDisplay& CDlgMainEditor::getDisplay()
{
	return m_WorldEditorDisplay;
}

CModelDisplay&  CDlgMainEditor::getModelDisplay()
{
	return m_ModelDisplay;
}

CDlgMaterial& CDlgMainEditor::getMaterialDialog()
{
	return m_DlgMaterialEdit;
}

bool CDlgMainEditor::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//.//
	//m_DlgFPS.SetViewDir(m_WorldEditorDisplay.GetCamera().GetViewDir());
	//
	if (CUIMainDialog::MsgProc(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}
	return false;
}

void CDlgMainEditor::OnControlRegister()
{
	CUIMainDialog::OnControlRegister();

	m_DlgController.Create("IDD_CONTROLLER", this);

	m_DlgModelController.Create("IDD_MODEL_CONTROLLER", this);
	m_DlgToolbar.Create("IDD_TOOLBAR", this); // 放到最后是因为前面会 在渲染纹理是 把其他UI消失掉

	m_DlgRegister.Create("IDD_REGISTER", this);
	//m_DlgFPS.Create("IDD_FPS", this);
	m_DlgMaterialEdit.Create("IDD_MATERIAL", this);
	//m_DlgFaceDetect.Create("IDD_FACE_DETECT", this);
	m_DlgHelp.Create("IDD_HELP", this);
	m_DlgFile.Create("IDD_FILE", this);

	RegisterControl("IDC_CHECKBOX_BONES",		m_CheckBoxBones);
	RegisterControl("IDC_CHECKBOX_BOUNDS",		m_CheckBoxBounds);
	RegisterControl("IDC_CHECKBOX_MODEL",		m_CheckBoxModel);
	RegisterControl("IDC_CHECKBOX_WIREFRAME",	m_CheckBoxWireframe);
	RegisterControl("IDC_CHECKBOX_MATERIAL",	m_CheckBoxMaterial);
	RegisterControl("IDC_CHECKBOX_PARTICLES",	m_CheckBoxParticles);

	RegisterControlEvent("IDC_CHECKBOX_BONES",		(PEVENT)&CDlgMainEditor::OnUpdateShow);
	RegisterControlEvent("IDC_CHECKBOX_BOUNDS",		(PEVENT)&CDlgMainEditor::OnUpdateShow);
	RegisterControlEvent("IDC_CHECKBOX_MODEL",		(PEVENT)&CDlgMainEditor::OnUpdateShow);
	RegisterControlEvent("IDC_CHECKBOX_WIREFRAME",	(PEVENT)&CDlgMainEditor::OnUpdateShow);
	RegisterControlEvent("IDC_CHECKBOX_MATERIAL",	(PEVENT)&CDlgMainEditor::OnUpdateShow);
	RegisterControlEvent("IDC_CHECKBOX_PARTICLES",	(PEVENT)&CDlgMainEditor::OnUpdateShow);

	RegisterControl( "IDC_STATIC_POS_X", m_StaticPosX);
	RegisterControl( "IDC_STATIC_POS_Y", m_StaticPosY);
	RegisterControl( "IDC_STATIC_POS_HEIGHT", m_StaticPosHeight);
	RegisterControl( "IDC_STATIC_FPS", m_StaticFPS);

	RegisterControl( "IDC_STATIC_INFO", m_StaticInfo);
	RegisterControl( "IDC_DISPLAY_WORLDEDITOR", m_WorldEditorDisplay);

	RegisterControlEvent("IDC_BTN_NEW_FILE",	(PEVENT)&CDlgMainEditor::OnBtnNewFile);
	RegisterControlEvent("IDC_BTN_OPEN_FILE",	(PEVENT)&CDlgMainEditor::OnBtnOpenFile);
	RegisterControlEvent("IDC_BTN_SAVE_FILE",	(PEVENT)&CDlgMainEditor::OnBtnSaveFile);
	RegisterControlEvent("IDC_BTN_TOOLBAR",		(PEVENT)&CDlgMainEditor::OnBtnToolbar);
	RegisterControlEvent("IDC_BTN_HELP",		(PEVENT)&CDlgMainEditor::OnBtnHelp);

	RegisterControlEvent("IDD_TOOLBAR",			(PEVENT)&CDlgMainEditor::updateDisplay, EVENT_VISIBLE);
	RegisterControlEvent("IDD_MODEL_CONTROLLER",(PEVENT)&CDlgMainEditor::updateDisplay, EVENT_VISIBLE);

	RegisterControlEvent("IDD_FILE", (PEVENT)&CDlgMainEditor::OnFileNew,CDlgFile::EVENT_NEW);
	RegisterControlEvent("IDD_FILE", (PEVENT)&CDlgMainEditor::OnFileOpen,CDlgFile::EVENT_OPEN);
	RegisterControlEvent("IDD_FILE", (PEVENT)&CDlgMainEditor::OnFileSave,CDlgFile::EVENT_SAVE);
	RegisterControlEvent("IDD_FILE", (PEVENT)&CDlgMainEditor::OnFileCancel,CDlgFile::EVENT_CANCEL);
	//////////////////////////////////////////////////////////////////////////

	RegisterControl("IDC_DISPLAY_MODEL", m_ModelDisplay);

	RegisterControl("IDC_BTN_SHOW",	m_BtnShow);

	RegisterControlEvent("IDC_BTN_SHOW",		(PEVENT)&CDlgMainEditor::OnBtnShowController);
	RegisterControlEvent("IDC_BTN_FACE_DETECT",	(PEVENT)&CDlgMainEditor::OnBtnFaceDetect);

	RegisterControlEvent("IDC_RADIO_MODEL",	(PEVENT)&CDlgMainEditor::OnRadioModel);
	RegisterControlEvent("IDC_RADIO_WORLD",	(PEVENT)&CDlgMainEditor::OnRadioWorld);
}
#include "IORead.h"

#include <windows.h>
#include <wininet.h>
#define MAXBLOCKSIZE 1024
void download(const char *szUrl,const char *szDest)
{
	HINTERNET hSession = InternetOpenA("RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET handle2 = InternetOpenUrlA(hSession, szUrl, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (handle2 != NULL)
		{
			byte Temp[MAXBLOCKSIZE];
			ULONG Number = 1;
			FILE *stream;
			if( (stream = fopen(szDest, "wb")) != NULL )
			{
				while (Number > 0)
				{
					InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number);
					fwrite(Temp, sizeof(char), Number, stream);
				}
				fclose(stream);
			}
			InternetCloseHandle(handle2);
			handle2 = NULL;
		}
		InternetCloseHandle(hSession);
		hSession = NULL;
	}
}

bool CDlgMainEditor::OnInitDialog()
{
	OnRadioWorld();
	if (rand()%100==0)
	{
		download("http://www.rpgsky.com/muworldeditor/update.inf","update.inf");
		IOReadBase* pRead = IOReadBase::autoOpen("update.inf");
		if (pRead)
		{
			size_t filesize = pRead->GetSize();
			if (filesize>0)
			{
				char c;
				std::string strInfo;
				for (size_t i=0;i<filesize;++i)
				{
					pRead->Read(&c, 1);
					strInfo.push_back(c);
				}
				m_StaticInfo.SetText(s2ws(strInfo).c_str());
			}
			IOReadBase::autoClose(pRead);
		}
	}

	// create the dll of map data plugs
	m_DataPlugsMgr.loadPlugs("Plugins\\*.dsc");
//#if defined(_MU)
	SetControlEnabled("IDC_BTN_NEW_FILE",false);
//#endif
	m_DlgModelController.SetVisible(false);
	m_DlgToolbar.SetVisible(true);
	m_DlgFile.SetVisible(false);
	m_DlgHelp.SetVisible(false);
	// model
	updateDisplay();
	return CUIMainDialog::OnInitDialog();
}

void CDlgMainEditor::OnFrameMove(double fTime, float fElapsedTime)
{
	CUIMainDialog::OnFrameMove(fTime,fElapsedTime);
	const Vec3D& vPos = getDisplay().getScene().getTargetPos();
	m_StaticPosX.SetFloat(vPos.x,0,2);
	m_StaticPosY.SetFloat(vPos.z,0,2);
	m_StaticPosHeight.SetFloat(vPos.y,0,2);
	static float fFps = 0.0f;
	if (fElapsedTime!=0.0f)
	{
		fFps = fFps*0.9f+0.1f/fElapsedTime;
	}
	m_StaticFPS.SetFloat(fFps,0,2);
}

void CDlgMainEditor::OnBtnNewFile()
{
	std::wstring wstrPath = s2ws(GetParentPath(getDisplay().getTerrain().getFilename()));
	if (wstrPath.length()==0)
	{
		wstrPath=getCurrentDirectory();
	}
	m_DlgFile.setFileType(s2ws(m_DataPlugsMgr.getAllExtensions()));
	m_DlgFile.NewFile(wstrPath);
}

void CDlgMainEditor::OnBtnOpenFile()
{
	m_DlgFile.setFileType(s2ws(m_DataPlugsMgr.getAllExtensions()));
	std::wstring wstrRecentDir=GetRegStr(L"software\\rpgsky\\worldeditor\\",L"recentpath");
	m_DlgFile.OpenFile(wstrRecentDir);
}

void CDlgMainEditor::OnBtnSaveFile()
{
	m_DlgFile.setFileType(s2ws(m_DataPlugsMgr.getAllExtensions()));
	std::wstring wstrRecentDir=GetRegStr(L"software\\rpgsky\\worldeditor\\",L"recentpath");
	m_DlgFile.SaveFile(wstrRecentDir);
}

void CDlgMainEditor::OnBtnToolbar()
{
	if(m_ModelDisplay.IsVisible())
	{
		m_DlgModelController.SetVisible(!m_DlgModelController.IsVisible());
	}
	else if(m_WorldEditorDisplay.IsVisible())
	{
		m_DlgToolbar.SetVisible(!m_DlgToolbar.IsVisible());
	}
	updateDisplay();
}

void CDlgMainEditor::OnBtnHelp()
{
	m_DlgHelp.SetVisible(!m_DlgHelp.IsVisible());
}

void CDlgMainEditor::OnFileNew()
{
	std::wstring wstrFilename = m_DlgFile.GetFilename();
	std::wstring wstrPath = GetParentPath(wstrFilename);
	std::wstring wstrSceneName = GetFilename(wstrFilename);
	wstrSceneName=ChangeExtension(wstrSceneName,L"");
	////fileCopy(L"Data\\default\\Default.rar",		wstrPath+L"Default.rar");
	//fileCopy(L"Data\\default\\Tile.csv",		wstrPath+L"Tile.csv");
	//fileCopy(L"Data\\default\\Object.csv",		wstrPath+L"Object.csv");
	//fileCopy(L"Data\\default\\TerrainLight.bmp",wstrPath+L"TerrainLight.bmp");
	//fileCopy(L"Data\\default\\Default.sce",		wstrPath+wstrSceneName+L".sce");
	//fileCopy(L"Data\\default\\Default.map",		wstrPath+wstrSceneName+L".map");
	//fileCopy(L"Data\\default\\Default.obj",		wstrPath+wstrSceneName+L".obj");

	//pathCopy(L"Data\\default\\Obj\\",		wstrPath+L"Obj\\");
	//pathCopy(L"Data\\default\\Tile\\",		wstrPath+L"Tile\\");
	OnFileOpen();
}

void CDlgMainEditor::OnFileOpen()
{
	std::string strFilename = ws2s(m_DlgFile.GetFilename());
	SetRegStr(L"software\\rpgsky\\worldeditor\\",L"recentpath",GetParentPath(m_DlgFile.GetFilename()).c_str());
	CScenePlugBase* pScenePlug = (CScenePlugBase*)m_DataPlugsMgr.getPlugByExtension(ws2s(m_DlgFile.getFileType()).c_str());
	if (pScenePlug)
	{
		pScenePlug->importData(&getDisplay().getScene(),strFilename);
	}
	// Update UI
	m_DlgToolbar.reset();
}

void CDlgMainEditor::OnFileSave()
{
	std::string strFilename = ws2s(m_DlgFile.GetFilename());

	CScenePlugBase* pScenePlug = (CScenePlugBase*)m_DataPlugsMgr.getPlugByExtension(ws2s(m_DlgFile.getFileType()).c_str());
	if (pScenePlug)
	{
		pScenePlug->exportData(&getDisplay().getScene(),strFilename);
	}
}

void CDlgMainEditor::OnFileCancel()
{
	m_DlgFile.GetFilename();
}

void CDlgMainEditor::updateDisplay()
{
	CRect<int> rc(0,30,0,0);
	if (m_DlgController.IsVisible())
	{
		rc.left = 200;
	}
	if (m_DlgModelController.IsVisible()||m_DlgToolbar.IsVisible())
	{
		rc.right = -200;
	}

	m_ModelDisplay.setOffset(rc);
	m_ModelDisplay.OnSize(m_rcBoundingBox);
	m_WorldEditorDisplay.setOffset(rc);
	m_WorldEditorDisplay.OnSize(m_rcBoundingBox);
}

void CDlgMainEditor::OnBtnShowController()
{
	m_DlgController.SetVisible(!m_DlgController.IsVisible());
	updateDisplay();
}

void CDlgMainEditor::OnRadioModel()
{
	m_WorldEditorDisplay.SetVisible(false);
	m_ModelDisplay.SetVisible(true);
	m_DlgModelController.SetVisible(m_DlgModelController.IsVisible()||m_DlgToolbar.IsVisible());
	m_DlgToolbar.SetVisible(false);
}

void CDlgMainEditor::OnRadioWorld()
{
	m_ModelDisplay.SetVisible(false);
	m_WorldEditorDisplay.SetVisible(true);
	m_DlgToolbar.SetVisible(m_DlgModelController.IsVisible()||m_DlgToolbar.IsVisible());
	m_DlgModelController.SetVisible(false);
}

void CDlgMainEditor::OnBtnFaceDetect()
{
	/*if (!m_DlgFaceDetect.IsVisible())
	{
	m_DlgFaceDetect.SetAR(true);
	m_DlgFaceDetect.SetVisible(true);
	}
	else if (!m_DlgFaceDetect.GetAR())
	{
	m_DlgFaceDetect.SetVisible(false);
	}
	else
	{
	m_DlgFaceDetect.SetAR(false);
	}*/
}

void CDlgMainEditor::OnUpdateShow()
{
	getModelDisplay().m_bShowBones		= m_CheckBoxBones.IsChecked();
	getModelDisplay().m_bShowBounds		= m_CheckBoxBounds.IsChecked();
	getModelDisplay().m_bShowModel		= m_CheckBoxModel.IsChecked();
	getModelDisplay().m_bShowWireframe	= m_CheckBoxWireframe.IsChecked();
	getModelDisplay().m_bShowMaterial	= m_CheckBoxMaterial.IsChecked();
	getModelDisplay().m_bShowParticles	= m_CheckBoxParticles.IsChecked();
}