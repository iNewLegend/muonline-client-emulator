#pragma once
#include "UIDisplay.h"
#include "Vec3D.h"
//#include "Mesh.h"

class CUICoordDisplay: public CUIDisplay
{
public:
	CUICoordDisplay();
	void SetViewDir(Vec3D& vViewDir);
	virtual void	OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
protected:
//	CCoordinate		m_Coord;
};
