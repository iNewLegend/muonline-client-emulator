#include "DlgUIList.h"
#include "EditingDialog.h"
#include "Tinyxml.h"
#include <List>
#include<iostream>
#include<iomanip>
#include<fstream>
#include"IniFile.h"

CDlgUIList::CDlgUIList():
m_pDlgView(NULL)
{
}

CDlgUIList::~CDlgUIList()
{	
}

void CDlgUIList::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	RegisterControl("IDC_DIALOG_LIST", m_ListBox);
	RegisterControlEvent("IDC_DIALOG_LIST", (PEVENT)&CDlgUIList::OnDlgListBoxSelection,EVENT_LISTBOX_SELECTION);
	RegisterControlEvent("IDC_DIALOG_LIST", (PEVENT)&CDlgUIList::OnDlgListBoxDblClk,EVENT_LISTBOX_ITEM_DBLCLK);
}

bool CDlgUIList::InitDialogListFromXML(const char* szFilename)
{
	TiXmlDocument myDocument( strFilename );
	myDocument.LoadFile(TIXML_ENCODING_UTF8);
	if ( myDocument.Error() )
	{
		return false;
	}
	std::list<std::wstring> listString;
	//获得根元素，即root。
	TiXmlElement *pRootElement = myDocument.RootElement();
	//获得第一个dialog节点。
	TiXmlElement *pDialogElement = pRootElement->FirstChildElement("dialog");
	while ( pDialogElement )
	{
		if ( pDialogElement->Attribute("id") )
		{
			WCHAR strText[256]={0};
			MultiByteToWideChar(CP_UTF8,0,pDialogElement->Attribute("id"),-1,strText,256);
			listString.push_back(strText);
		}
		// 查找下一个dialog
		pDialogElement = pDialogElement->NextSiblingElement("dialog");
	}
	//
	listString.sort();
	for(std::list<std::wstring>::iterator it=listString.begin(); it!=listString.end(); it++)
	{
		m_ListBox.AddItem(*it);
	}
	return true;
}

bool CDlgUIList::createDialogStringFromXML(const char* szXMLFilename, const char* szStringFilename)
{
	TiXmlDocument myDocument( strXMLFilename );
	myDocument.LoadFile(TIXML_ENCODING_UTF8);
	if ( myDocument.Error() )
	{
		return false;
	}
	std::ofstream file;
	file.open(strStringFilename.c_str(), std::ios::out);

	//获得根元素，即root。
	TiXmlElement *pRootElement = myDocument.RootElement();
	//获得第一个dialog节点。
	TiXmlElement *pDialogElement = pRootElement->FirstChildElement("dialog");
	WCHAR wszText[256]={0};
	while ( pDialogElement )
	{
		if ( pDialogElement->Attribute("id") )
		{
			std::wstring wstrDialog;
			{
				MultiByteToWideChar(CP_UTF8,0,pDialogElement->Attribute("id"),-1,wszText,256);
				std::wstring wstrDialog = wszText;
				file<<"["<<ws2s(wstrDialog)<<"]"<<std::endl;
			}
			TiXmlElement *pCaptionElement = pDialogElement->FirstChildElement("caption");
			if ( pCaptionElement&&pCaptionElement->GetText() )
			{
				std::wstring wstrCaption;
				MultiByteToWideChar(CP_UTF8,0,pCaptionElement->GetText(),-1,wszText,256);
				wstrCaption = wszText;
				file<<"CAPTION="<<ws2s(wstrCaption)<<std::endl;
			}
			{
				TiXmlElement *pControlElement = pDialogElement->FirstChildElement("element");
				while (pControlElement)
				{
					std::wstring wstrControl,wstrText,wstrTip;
					if ( pControlElement->Attribute("id") )
					{
						MultiByteToWideChar(CP_UTF8,0,pControlElement->Attribute("id"),-1,wszText,256);
						wstrControl = wszText;

						//////////////////////////////////////////////////////////////////////////
						if ( pControlElement->GetText() )
						{
							MultiByteToWideChar(CP_UTF8,0,pControlElement->GetText(),-1,wszText,256);
							wstrText = wszText;
							file<<"TEXT_"<<ws2s(wstrControl)<<"="<<ws2s(wstrText)<<std::endl;
						}
					}
					pControlElement = pControlElement->NextSiblingElement("element");
				}
			}
			{
				TiXmlElement *pControlElement = pDialogElement->FirstChildElement("element");
				while (pControlElement)
				{
					std::wstring wstrControl,wstrText,wstrTip;
					if ( pControlElement->Attribute("id") )
					{
						MultiByteToWideChar(CP_UTF8,0,pControlElement->Attribute("id"),-1,wszText,256);
						wstrControl = wszText;

						//////////////////////////////////////////////////////////////////////////
						if ( pControlElement->Attribute("tip") )
						{
							MultiByteToWideChar(CP_UTF8,0,pControlElement->Attribute("tip"),-1,wszText,256);
							wstrTip = wszText;
							file<<"TIP_"<<ws2s(wstrControl)<<"="<<ws2s(wstrTip)<<std::endl;
						}
					}
					pControlElement = pControlElement->NextSiblingElement("element");
				}
			}
			file<<std::endl;
		}
		// 查找下一个dialog
		pDialogElement = pDialogElement->NextSiblingElement("dialog");
	}
	file.close();
	//
// 	listString.sort();
// 	for(std::list<std::wstring>::iterator it=listString.begin(); it!=listString.end(); it++)
// 	{
// 		m_ListBox.AddItem(*it);
// 	}
	return true;
}

void CDlgUIList::OnDlgListBoxSelection()
{
	static CEditingDialog* s_pDialog = NULL;
	if (s_pDialog)
	{
		S_DEL(s_pDialog);
	}
	if (m_ListBox.GetSelectedItem())
	{
		s_pDialog = new CEditingDialog;
		s_pDialog->Create(ws2s(m_ListBox.GetSelectedItem()->wstrText), m_pDlgView);
		s_pDialog->OnSize(m_pDlgView->GetBoundingBox());
	}
}

void CDlgUIList::OnDlgListBoxDblClk()
{
	if (m_ListBox.GetSelectedItem())
	{
		std::string strEditUIFilename = IniGetStr("UIEditorUI.cfg","UIConfig","edit");
		createDialogCodeFromXML(strEditUIFilename,ws2s(m_ListBox.GetSelectedItem()->wstrText));
	}
}
#include <cctype>
#include <algorithm>
inline std::string id2name(const std::string& strID)
{
	std::vector<std::string> out;
	Tokenizer(strID,out,"_");
	if (out.size()>1)
	{
		std::string	name;
		for(size_t i=1;i<out.size();++i)
		{
			std::string	words;
			words = out[i];
			std::transform(words.begin(),words.end(),words.begin(),std::tolower);// toLower
			std::transform(words.begin(),++words.begin(),words.begin(),std::toupper);
			name+=words;
		}
		return name;
	}
	return strID;
}

inline  std::string type2ControlClassName(const std::string& strType)
{
	if(strType=="checkbox")
	{
		return "CUICheckBox";
	}
	else if(strType=="radiobutton")
	{
		return "CUIRadioButton";
	}
	else if(strType=="editbox")
	{
		return "CUIEditBox";
	}
	else if(strType=="imeeditbox")
	{
		return "CUIIMEEditBox";
	}
	else if(strType=="scrollbar")
	{
		return "CUIScrollBar";
	}
	else if(strType=="listbox")
	{
		return "CUIListBox";
	}
	else if(strType=="combobox")
	{
		return "CUIComboBox";
	}
	else if(strType=="numeditbox")
	{
		return "CUIComboNumber";
	}
	else if(strType=="color")
	{
		return "CUIComboColor";
	}
	else
	{
		std::string	name=strType;
		if (name.length()>0)
		{
			std::transform(name.begin(),name.end(),name.begin(),std::tolower);
			std::transform(name.begin(),name.begin()++,name.begin(),std::toupper);
		}
		name.insert(0,"CUI");
		return name;
	}
}

bool CDlgUIList::createDialogCodeFromXML(const char* szFilename,const char* szDialogName)
{
	TiXmlDocument myDocument( strFilename );
	myDocument.LoadFile(TIXML_ENCODING_UTF8);
	if ( myDocument.Error() )
	{
		return false;
	}
	std::list<std::wstring> listString;
	//获得根元素，即root。
	TiXmlElement *pRootElement = myDocument.RootElement();
	//获得第一个dialog节点。
	TiXmlElement *pDialogElement = pRootElement->FirstChildElement("dialog");
	while ( pDialogElement )
	{
		if ( pDialogElement->Attribute("id") )
		{
			WCHAR strText[256]={0};
			MultiByteToWideChar(CP_UTF8,0,pDialogElement->Attribute("id"),-1,strText,256);

			if (strDialogName==ws2s(strText))
			{
				{
					setlocale(LC_ALL,"Chinese-simplified");
					std::ofstream ofs("dialog_code.h");
					assert(ofs);
					ofs<<"class CDlg"<<id2name(strDialogName)<<": public CUIDialog"<<std::endl;
					ofs<<"{"<<std::endl;
					ofs<<"public:"<<std::endl;
					ofs<<"	virtual void OnControlRegister();"<<std::endl;
					ofs<<"private:"<<std::endl;

					ofs<<"	// Controls' event."<<std::endl;
					TiXmlElement *pControlElement = pDialogElement->FirstChildElement("element");
					while (pControlElement)
					{
						if (pControlElement->Attribute("id"))
						{
							std::string strID = pControlElement->Attribute("id");
							ofs<<"	void On"<<id2name(strID)<<"();"<<std::endl;
						}
						pControlElement = pControlElement->NextSiblingElement("element");
					}
					ofs<<"	// Controls' variable."<<std::endl;
					pControlElement = pDialogElement->FirstChildElement("element");
					while (pControlElement)
					{
						std::string strType = pControlElement->Attribute("type");
						if (pControlElement->Attribute("id"))
						{
							std::string strID = pControlElement->Attribute("id");
							ofs<<"	"<<type2ControlClassName(strType)<<"	m_"<<id2name(strID)<<";"<<std::endl;
						}
						pControlElement = pControlElement->NextSiblingElement("element");
					}
					ofs<<"}";
					ofs.close();
				}
				//////////////////////////////////////////////////////////////////////////
				{
					setlocale(LC_ALL,"Chinese-simplified");
					std::ofstream ofs("dialog_code.cpp");
					assert(ofs);
					ofs<<"#include \"Dlg"<<id2name(strDialogName)<<".h\""<<std::endl;
					ofs<<std::endl;
					ofs<<"void CDlg"<<id2name(strDialogName)<<"::OnControlRegister()"<<std::endl;
					ofs<<"{"<<std::endl;
					TiXmlElement *pControlElement = pDialogElement->FirstChildElement("element");
					while (pControlElement)
					{
						if (pControlElement->Attribute("id"))
						{
							std::string strID = pControlElement->Attribute("id");
							ofs<<"	RegisterControl(\""<<strID<<"\",	m_"<<id2name(strID)<<");"<<std::endl;
						}
						pControlElement = pControlElement->NextSiblingElement("element");
					}
					ofs<<std::endl;
					pControlElement = pDialogElement->FirstChildElement("element");
					while (pControlElement)
					{
						if (pControlElement->Attribute("id"))
						{
							std::string strID = pControlElement->Attribute("id");
							ofs<<"	RegisterControlEvent(\""<<strID<<"\",	(PEVENT)&CDlg"<<id2name(strDialogName)<<"::On"<<id2name(strID)<<");"<<std::endl;
						}
						pControlElement = pControlElement->NextSiblingElement("element");
					}
					ofs<<"}";
					pControlElement = pDialogElement->FirstChildElement("element");
					while (pControlElement)
					{
						if (pControlElement->Attribute("id"))
						{
							std::string strID = pControlElement->Attribute("id");
							ofs<<std::endl<<std::endl;
							ofs<<"void CDlg"<<id2name(strDialogName)<<"::On"<<id2name(strID)<<"()"<<std::endl;
							ofs<<"{"<<std::endl;
							ofs<<"	// Do it."<<std::endl;
							ofs<<"}";
						}
						pControlElement = pControlElement->NextSiblingElement("element");
					}
					ofs.close();
				}
				return true;
				break;
			}
		}
		// 查找下一个dialog
		pDialogElement = pDialogElement->NextSiblingElement("dialog");
	}
	return true;
}