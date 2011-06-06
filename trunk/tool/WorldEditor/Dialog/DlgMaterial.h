#pragma once
#include "UIDialog.h"
#include "Material.h"

class CDlgMaterial :public CUIDialog
{
public:
	CDlgMaterial();
	virtual void	OnControlRegister();
	void			setMaterial(const std::string& strMaterial, const std::string& strPath);
private:
	std::string		getEditBoxFilename(const CUIEditBox& editBox);
	// Controls' event.
	void			OnEditboxTexture(int id);
	// ----
	void			OnEditboxS0();
	void			OnEditboxS1();
	void			OnEditboxS2();
	void			OnEditboxS3();
	void			OnEditboxS4();
	void			OnEditboxS5();
	void			OnEditboxS6();
	void			OnEditboxS7();
	// ----
	void			OnComboBoxPassChanged();
	// ----
	void			OnBtnClose();
private:
	CUIEditBox		m_EditboxTexture[8];
	CUIComboBox		m_ComboBoxPass;
	// ----
	CMaterial*		m_pSelectedMaterial;
	// ----
	std::string		m_strPath;
};