#pragma once
#include "Manager.h"
#include "UIDialog.h"

class CDlgFile : public CUIDialog
{
public:
	CDlgFile();
	~CDlgFile();
public:
	enum EVENT_TYPE
	{
		EVENT_NEW,
		EVENT_OPEN,
		EVENT_SAVE,
		EVENT_CANCEL,
	};
	enum OPERATING_TYPE
	{
		OPERATING_TYPE_NEW,
		OPERATING_TYPE_OPEN,
		OPERATING_TYPE_SAVE,
		OPERATING_TYPE_MAX,
	};
	virtual void OnControlRegister();
	virtual void SetVisible(bool bVisible);

	void OnListBoxFolderSelection();
	void OnListBoxFolderItemDblClk();

	void OnListBoxFileSelection();
	void OnListBoxFileDblClk();

	void OnBtnUp();
	void OnBtnBack();

	void OnBtnNew();
	void OnBtnOpen();
	void OnBtnSave();
	void OnBtnCancel();
	void OnCmbFileTypeSelect();

	const std::wstring& GetFilename(){return m_wstrFilename;}
	void setFileType(const std::wstring& wstrFileType);
	const std::wstring& getFileType(){return m_wstrFileType;}

	bool OpenPath(const std::wstring& wstrPath);
	void NewFile(const std::wstring& wstrPath);
	void OpenFile(const std::wstring& wstrPath);
	void SaveFile(const std::wstring& wstrPath);
	bool IsOpenFile(){return OPERATING_TYPE_OPEN==m_eOperatingType;}
	bool IsSaveFile(){return OPERATING_TYPE_SAVE==m_eOperatingType;}
protected:
private:
	CUIListBox m_ListBoxFolder;
	CUIListBox m_ListBoxFile;
	CUIEditBox m_EditBoxFile;
	CUIButton m_BtnNew;
	CUIButton m_BtnOpen;
	CUIButton m_BtnSave;
	CUIComboBox m_ComboBoxFileType;
	std::wstring	m_wstrDir;
	std::vector<std::wstring> m_setRecentPath;
	std::vector<std::wstring> m_setFileType;
	std::wstring	m_wstrFilename;
	std::wstring	m_wstrFileType;
	OPERATING_TYPE	m_eOperatingType;
};