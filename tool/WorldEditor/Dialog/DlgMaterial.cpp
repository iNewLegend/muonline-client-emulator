#include "DlgMaterial.h"
#include "FileSystem.h"
#include "RenderSystem.h"

CDlgMaterial::CDlgMaterial():m_pSelectedMaterial(NULL)
{
}

void CDlgMaterial::OnControlRegister()
{
	RegisterControl("IDC_EDITBOX_S0",	m_EditboxTexture[0]);
	RegisterControl("IDC_EDITBOX_S1",	m_EditboxTexture[1]);
	RegisterControl("IDC_EDITBOX_S2",	m_EditboxTexture[2]);
	RegisterControl("IDC_EDITBOX_S3",	m_EditboxTexture[3]);
	RegisterControl("IDC_EDITBOX_S4",	m_EditboxTexture[4]);
	RegisterControl("IDC_EDITBOX_S5",	m_EditboxTexture[5]);
	RegisterControl("IDC_EDITBOX_S6",	m_EditboxTexture[6]);
	RegisterControl("IDC_EDITBOX_S7",	m_EditboxTexture[7]);
	// ----
	RegisterControl("IDC_COMBOBOX_PASS",	m_ComboBoxPass);
	// ----
	// # Event
	// ----
	RegisterControlEvent("IDC_EDITBOX_S0",		(PEVENT)&CDlgMaterial::OnEditboxS0);
	RegisterControlEvent("IDC_EDITBOX_S1",		(PEVENT)&CDlgMaterial::OnEditboxS1);
	RegisterControlEvent("IDC_EDITBOX_S2",		(PEVENT)&CDlgMaterial::OnEditboxS2);
	RegisterControlEvent("IDC_EDITBOX_S3",		(PEVENT)&CDlgMaterial::OnEditboxS3);
	RegisterControlEvent("IDC_EDITBOX_S4",		(PEVENT)&CDlgMaterial::OnEditboxS4);
	RegisterControlEvent("IDC_EDITBOX_S5",		(PEVENT)&CDlgMaterial::OnEditboxS5);
	RegisterControlEvent("IDC_EDITBOX_S6",		(PEVENT)&CDlgMaterial::OnEditboxS6);
	RegisterControlEvent("IDC_EDITBOX_S7",		(PEVENT)&CDlgMaterial::OnEditboxS7);
	// ----
	RegisterControlEvent("IDC_COMBOBOX_PASS",	(PEVENT)&CDlgMaterial::OnComboBoxPassChanged);
	// ----
	RegisterControlEvent("IDC_BTN_CLOSE",		(PEVENT)&CDlgMaterial::OnBtnClose);
}

void CDlgMaterial::setMaterial(const std::string& strMaterial, const std::string& strPath)
{
	m_pSelectedMaterial = &GetRenderSystem().getMaterialMgr().getItem(strMaterial.c_str());
	if (m_pSelectedMaterial)
	{
		CTextureMgr& TM=GetRenderSystem().GetTextureMgr();
		CShaderMgr& SM=GetRenderSystem().GetShaderMgr();
		m_strPath = strPath;

		for (size_t i=0; i<8; ++i)
		{
			m_EditboxTexture[i].SetText(s2ws(getSimpleFilename(strPath,TM.getItemName(m_pSelectedMaterial->uTexture[i]))).c_str());
		}
		//m_EditboxEffect.SetText(s2ws(getSimpleFilename(strPath,SM.getItemName(m_pSelectedMaterial->uShader))).c_str());
		//m_CheckboxAlphatest.SetChecked(m_pSelectedMaterial->bAlphaTest);
		//m_NumAlphatestvalue.setFloat(m_pSelectedMaterial->uAlphaTestValue,0,2);
		//m_CheckboxBlend.SetChecked(m_pSelectedMaterial->bBlend);
		//m_CheckboxCull.SetChecked(m_pSelectedMaterial->uCull);
		//m_NumTexanimX.setFloat(m_pSelectedMaterial->vTexAnim.x,0,2);
		//m_NumTexanimY.setFloat(m_pSelectedMaterial->vTexAnim.y,0,2);
		//m_NumOpacity.setFloat(m_pSelectedMaterial->m_fOpacity,0,2);
		//m_ColorEmissive.setColor(m_pSelectedMaterial->cEmissive);
		//m_NumUvscaleX.setFloat(m_pSelectedMaterial->vUVScale.x,0,2);
		//m_NumUvscaleY.setFloat(m_pSelectedMaterial->vUVScale.y,0,2);
	}
}

std::string CDlgMaterial::getEditBoxFilename(const CUIEditBox& editBox)
{
	return getRealFilename(m_strPath.c_str(),ws2s(editBox.GetText()).c_str());
}

void CDlgMaterial::OnEditboxTexture(int id)
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->uTexture[id] = GetRenderSystem().GetTextureMgr().RegisterTexture(getEditBoxFilename(m_EditboxTexture[id]));
}

void CDlgMaterial::OnEditboxS0()
{
	OnEditboxTexture(0);
}

void CDlgMaterial::OnEditboxS1()
{
	OnEditboxTexture(1);
}

void CDlgMaterial::OnEditboxS2()
{
	OnEditboxTexture(2);
}

void CDlgMaterial::OnEditboxS3()
{
	OnEditboxTexture(3);
}

void CDlgMaterial::OnEditboxS4()
{
	OnEditboxTexture(4);
}

void CDlgMaterial::OnEditboxS5()
{
	OnEditboxTexture(5);
}

void CDlgMaterial::OnEditboxS6()
{
	OnEditboxTexture(6);
}

void CDlgMaterial::OnEditboxS7()
{
	OnEditboxTexture(7);
}

void CDlgMaterial::OnComboBoxPassChanged()
{
}

void CDlgMaterial::OnBtnClose()
{
	SetVisible(false);
}