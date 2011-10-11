//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* UIIcon.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "UIButton.h"
#include "Camera.h"
#include "RenderNode.h"
#include "SceneEffect.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct ItemData 
{
	char cType;
	char cIndex;
	int level;
};

class CUIIcon : public CUIButton
{
public:
	// ----
	CUIIcon();
	~CUIIcon();
	// ----
	void			OnFrameMove		(double fTime, float fElapsedTime);
	void			OnFrameRender	(const Matrix& mTransform, double fTime, float fElapsedTime);
	// ----
	virtual void	OnMouseMove		(POINT point);
	virtual void	OnMouseWheel	(POINT point, short wheelDelta);
	virtual void	OnLButtonDown	(POINT point);
	virtual void	OnLButtonUp		(POINT point);
	virtual void	OnRButtonDown	(POINT point);
	virtual void	OnRButtonUp		(POINT point);
	virtual void	OnMButtonDown	(POINT point);
	virtual void	OnMButtonUp		(POINT point);
	// ----
	virtual void	OnSize			(const CRect<int> & rc);
	// ----
	const ItemData& getItemData		();
	void			setItemData		(const ItemData& itemData);
protected:
	int				m_nIconID;
	ItemData		m_ItemData;
	void*			s_pTexture;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------