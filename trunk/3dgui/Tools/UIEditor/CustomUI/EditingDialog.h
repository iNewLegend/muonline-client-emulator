#pragma once
#include "Manager.h"
#include "UIDialog.h"
#include "UIControl.h"

#include "UIStatic.h"
#include "UIButton.h"
#include "UICheckBox.h"
#include "UIComboBox.h"
#include "UIRadioButton.h"
#include "UIImage.h"
#include "UIListBox.h"
#include "UIScrollBar.h"
#include "UISlider.h"
#include "UIEditBox.h"
#include "UIIMEEditBox.h"
class CEditingDialog : public CUIDialog
{
public:
	CEditingDialog();
	~CEditingDialog();
public:
	bool OnInitDialog();
	void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
protected:
private:
};