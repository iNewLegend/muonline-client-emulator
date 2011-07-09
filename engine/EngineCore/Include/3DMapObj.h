#pragma once
#include "MapObj.h"

class C3DMapObj : public CMapObj
{
public:
	C3DMapObj();
	~C3DMapObj();
protected:
	Pos2D	m_posCell;
public:
	virtual void	GetCellPos		( Pos2D& posCell ){posCell = m_posCell;}
	virtual void	SetCellPos		( Pos2D& posCell ){m_posCell = posCell;}

	virtual int		getObjType		(){return MAP_3DOBJ;}
	virtual bool	IsFocus			(){return false;}

	virtual Matrix	getShadowMatrix	(const Vec3D& vLight,float fHeight)const;
	virtual void	renderShadow	(const Matrix& mWorld, const Vec3D& vLight,float fHeight)const;
	virtual void	renderFocus		()const;
	virtual void	renderDebug		()const;

	void			renderFocus		(Color32 color)const;
protected:
};