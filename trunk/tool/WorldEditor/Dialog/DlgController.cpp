#include "DlgController.h"
/*#include "mpq/mpq_libmpq.h"*/
/*#include "database.h"*/
#include "CsvFile.h"
#include "IORead.h"
#include "DlgMainEditor.h"
#include "FileSystem.h"
#include "..\MainRoot.h"
#include "RegData.h"
#include "RenderNodeMgr.h"

CDlgController::CDlgController()
{
}

CDlgController::~CDlgController()
{	
}

void CDlgController::OnControlRegister()
{
	RegisterControl("IDC_LISTBOX_FILE",		m_ListBoxFolder);

	RegisterControl("IDC_TAB_OBJECT_FILE",		m_RadioButtonFile);
	RegisterControl("IDC_TAB_OBJECT_AVATAR",	m_RadioButtonAvatar);

	m_DlgModelAvatar.Create("IDD_AVATAR", this);

	RegisterControlEvent("IDC_LISTBOX_FILE",	(PEVENT)&CDlgController::OnListBoxFolderItemDblClk, EVENT_LISTBOX_ITEM_DBLCLK);


	RegisterControlEvent("IDC_TAB_OBJECT_FILE",		(PEVENT)&CDlgController::OnTabObjectChanged);
	RegisterControlEvent("IDC_TAB_OBJECT_AVATAR",	(PEVENT)&CDlgController::OnTabObjectChanged);

	RegisterControlEvent("IDC_BTN_CLOSE",		(PEVENT)&CDlgController::OnClose);
}

bool CDlgController::OnInitDialog()
{
	//SetButtonGroup();

	OnTabObjectChanged();
	initRecentPath();
	//MPQModel();
	//
	int   argc=0;
	LPWSTR   *argv=::CommandLineToArgvW(::GetCommandLineW(),&argc);
	if (argc>1&&argv)
	{
		OpenFile(argv[1]);
	}
	LocalFree(argv); 

	return true;
}

void CDlgController::C3Model()
{
	m_wstrPath=L"";
	m_wstrFileType=L"";
	m_ListBoxFolder.RemoveAllItems();
	// miku
	m_ListBoxFolder.AddItem( L"D:/Miku/miku.sm");
	//
	CCsvFile csvFile;
	if (csvFile.open("csv/3DObj.csv"))
	{
		while (csvFile.seekNextLine())
		{
			m_ListBoxFolder.AddItem(s2ws(csvFile.getStr("Name","")).c_str());
		}
		csvFile.close();
	}
}

void CDlgController::MPQModel()
{
/*	m_wstrPath=L"";
	m_wstrFileType=L"";
	m_ListBoxFolder.RemoveAllItems();
	// 载入游戏MPQ文件到内存
	SetGamePath(GetModelConfig().m_strWOWPath);
	InitMPQArchives();
	std::set<FileTreeItem> filelist;
	getFileLists(filelist, filterModels);

	// 把所有可视文件放到树里面
	for (std::set<FileTreeItem>::iterator it = filelist.begin(); it != filelist.end(); ++it)
	{
		WCHAR wszBuf[256]={0};
		const std::string& str = (*it).fn;
		MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,wszBuf,256);
		m_ListBoxFolder.AddItem(wszBuf);
	}

	// clear filelist
	filelist.clear();

	//if (!
	GetAnimDB().open();//)
	//wxLogMessage(_T("Error: Could not open the Animation DB."));
	//if (!
	GetCreatureModelDB().open();//)
	//wxLogMessage(_T("Error: Could not open the Creatures DB."));
	//if (!
	GetCreatureSkinDB().open();//)
	//wxLogMessage(_T("Error: Could not open the CreatureDisplayInfo DB."));*/
}

void CDlgController::initRecentPath()
{
	// read the recent path from reg.
	std::wstring wstrRecentPath = GetRegStr(L"software\\rpgsky\\modelview\\",L"recentpath");
	OpenPath(wstrRecentPath,s2ws(CRenderNodeMgr::getInstance().getDataPlugsMgr().getAllExtensions()));
}

void CDlgController::OpenFile(const std::wstring& wstrFilename)
{
	if (IOReadBase::Exists(ws2s(wstrFilename)))
	{
		OpenPath(GetParentPath(wstrFilename),s2ws(CRenderNodeMgr::getInstance().getDataPlugsMgr().getAllExtensions()));
		//m_ListBoxFolder.SetSelec()
		//GetFilename(wstrFilename)
		CMainRoot::getInstance().getMainDialog().getModelDisplay().LoadModel(ws2s(wstrFilename));
		CMainRoot::getInstance().getMainDialog().getDlgModelController().OnUpdate();
	}
}

void CDlgController::OpenPath(const std::wstring& wstrPath, const std::wstring& wstrFileType)
{
	m_wstrPath=wstrPath;
	m_wstrFileType=wstrFileType;
	m_ListBoxFolder.RemoveAllItems();
	//|{"+wstrFileType+L"}";
	CDir dir;
	if(dir.ReadDir(m_wstrPath)==false)
	{
		m_wstrPath=getCurrentDirectory();
		dir.ReadDir(m_wstrPath);
	}
	for (int i=0; i<dir.m_FileInfo.size(); i++)
	{
		if (dir.m_FileInfo[i].IsDirectory())
		{
			m_ListBoxFolder.AddItem((L'\\'+dir.m_FileInfo[i].wstrFilename).c_str());
		}
	}
	for (int i=0; i<dir.m_FileInfo.size(); i++)
	{
		if (!dir.m_FileInfo[i].IsDirectory())
		{
			if (wstrFileType.find(GetExtension(dir.m_FileInfo[i].wstrFilename))!=std::wstring::npos)
			{
				m_ListBoxFolder.AddItem(dir.m_FileInfo[i].wstrFilename.c_str());
			}
		}

	}
}

void CDlgController::OnListBoxFolderItemDblClk()
{
	std::wstring wstrFolder = m_ListBoxFolder.GetSelectedItem()->wstrText;
	if (wstrFolder.length()>0&&wstrFolder[0]==L'\\')
	{
		if (L"\\.."==wstrFolder)
		{
			OpenPath(GetParentPath(m_wstrPath),m_wstrFileType);
		}
		else
		{
			OpenPath(m_wstrPath+wstrFolder.substr(1)+L'\\',m_wstrFileType);
		}
	}
	else
	{
		// write the recent path to reg.
		SetRegStr(L"software\\rpgsky\\modelview\\",L"recentpath",m_wstrPath.c_str());
		std::string strFilename = ws2s(m_wstrPath+m_ListBoxFolder.GetSelectedItem()->wstrText);
		CMainRoot::getInstance().getMainDialog().getModelDisplay().LoadModel( strFilename );
		CMainRoot::getInstance().getMainDialog().getDlgModelController().OnUpdate();
	}
}

void CDlgController::OnTabObjectChanged()
{
	m_ListBoxFolder.SetVisible(m_RadioButtonFile.IsChecked());
	m_DlgModelAvatar.SetVisible(m_RadioButtonAvatar.IsChecked());
}

void CDlgController::OnClose()
{
	CMainRoot::getInstance().getMainDialog().OnBtnShowController();
}