#include "DlgModelPlayer.h"
#include "ModelDisplay.h"
#include "FileSystem.h"
#include "IniFile.h"
#include "..\MainRoot.h"

void CDlgModelPlayer::OnControlRegister()
{
	CUIDialog::OnControlRegister();

	RegisterControl("IDC_CMB_SKELETON",		m_ComboBoxSkeleton);
	RegisterControl("IDC_CMB_HEAD",			m_ComboBoxHead);
	RegisterControl("IDC_CMB_SET",			m_ComboBoxSet);
	RegisterControl("IDC_CMB_HELM",			m_ComboBoxEquips[ET_HELM]);
	RegisterControl("IDC_CMB_ARMOR",		m_ComboBoxEquips[ET_ARMOR]);
	RegisterControl("IDC_CMB_GLOVE",		m_ComboBoxEquips[ET_GLOVE]);
	RegisterControl("IDC_CMB_PANTS",		m_ComboBoxEquips[ET_PANTS]);
	RegisterControl("IDC_CMB_BOOT",			m_ComboBoxEquips[ET_BOOT]);
	RegisterControl("IDC_CMB_LEFTHAND",		m_ComboBoxLeftHand);
	RegisterControl("IDC_CMB_RIGHTHAND",	m_ComboBoxRightHand);
	RegisterControl("IDC_CMB_BACK",			m_ComboBoxBack);


	RegisterControlEvent("IDC_CMB_SKELETON",	(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_HEAD",		(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_HELM",		(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_ARMOR",		(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_GLOVE",		(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_PANTS",		(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_BOOT",		(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_LEFTHAND",	(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_RIGHTHAND",	(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);
	RegisterControlEvent("IDC_CMB_BACK",		(PEVENT)&CDlgModelPlayer::OnUpdatePlayer);

	RegisterControlEvent("IDC_CMB_SET",			(PEVENT)&CDlgModelPlayer::OnSelectSet);
}
#include <algorithm>
#include <cctype>
bool CDlgModelPlayer::OnInitDialog()
{
	m_ComboBoxSkeleton.AddItem(L"");
	m_ComboBoxHead.AddItem(L"");
	m_ComboBoxSet.AddItem(L"");
	for (size_t i=0;i<ET_MAX;++i)
	{
		m_ComboBoxEquips[i].AddItem(L"");
	}
	m_ComboBoxLeftHand.AddItem(L"");
	m_ComboBoxRightHand.AddItem(L"");
	m_ComboBoxBack.AddItem(L"");
	std::set<std::wstring>	setString;

	std::string strDirMU = IniGetStr("WorldEditor.cfg","ResDir","mu");
	std::wstring wstrFindPath = s2ws(strDirMU)+L"Player\\";//|{"+wstrFileType+L"}";
	CDir dir;
	dir.ReadDir(wstrFindPath);
	for (size_t i=0; i<dir.m_FileInfo.size(); i++)
	{
		if (!dir.m_FileInfo[i].IsDirectory())
		{
			std::wstring wstrFilename = dir.m_FileInfo[i].wstrFilename;
			std::transform(wstrFilename.begin(),wstrFilename.end(),wstrFilename.begin(),std::toupper);
			//if (wstrFileType.find(GetExtension(dir.m_FileInfo[i].wstrFilename))!=std::wstring::npos)
			if (GetExtension(wstrFilename)==L".bmd")
			{
				if (wstrFilename.find(L"HELMCLASS")!=std::wstring::npos)
				{
					m_ComboBoxHead.AddItem(wstrFilename.c_str());
				}
				else if (wstrFilename.find(L"PLAYER")!=std::wstring::npos)
				{
					m_ComboBoxSkeleton.AddItem(wstrFilename.c_str());
				}
				else
				{
					for (size_t i=0;i<ET_MAX;++i)
					{
						if (wstrFilename.find(s2ws(g_EquipNames[i]))!=std::wstring::npos)
						{
							m_ComboBoxEquips[i].AddItem(wstrFilename.c_str());
							int index=wstrFilename.find(s2ws(g_EquipNames[i]));
							wstrFilename.replace(index, strlen(g_EquipNames[i]), L"*");
							setString.insert(wstrFilename);
							break;
						}
					}
				}
			}
		}
	}
	for (std::set<std::wstring>::iterator it=setString.begin();it!=setString.end();it++)
	{
		m_ComboBoxSet.AddItem(it->c_str());
	}
	return true;
}

void CDlgModelPlayer::OnUpdatePlayer()
{
/*	if (wcslen(m_ComboBoxSkeleton.GetText())==0)
	{
		return;
	}
	std::string strDirMU = IniGetStr("WorldEditor.cfg","ResDir","mu");
	std::string strPlayerPath = strDirMU+"Player\\";
	CMainRoot::getInstance().getMainDialog().getModelDisplay().loadComplexModel(strPlayerPath+ws2s(m_ComboBoxSkeleton.GetText()));
	CModelObject* pModelObject = CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelObject();
	if (pModelObject)
	{
		std::string strModelFilename = strPlayerPath+ ws2s(m_ComboBoxHead.GetText());
	///	pModelObject->loadSkinModel("head",strModelFilename.c_str());
		for (size_t i=0;i<ET_MAX;++i)
		{
			strModelFilename = strPlayerPath+ ws2s(m_ComboBoxEquips[i].GetText());
	//		pModelObject->loadSkinModel(g_EquipNames[i],strModelFilename.c_str());
		}
	}
	CMainRoot::getInstance().getMainDialog().getDlgModelController().OnUpdate();*/
}

void CDlgModelPlayer::OnSelectSet()
{
	std::wstring wstrSet = m_ComboBoxSet.GetText();
	int index=wstrSet.find(L"*");
	if (index==std::wstring::npos)
	{
		return;
	}
	m_ComboBoxHead.getListBox().SelectItem(0);
	for (size_t i=0;i<ET_MAX;++i)
	{
		std::wstring wstrPart=wstrSet;
		wstrPart.replace(index, wcslen(L"*"),s2ws(g_EquipNames[i]));
		m_ComboBoxEquips[i].getListBox().selectByText(wstrPart.c_str());
	}
}