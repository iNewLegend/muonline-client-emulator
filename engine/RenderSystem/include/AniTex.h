#pragma once
#include "Color.h"
#include "windows.h"

class CAniTex
{
public:
	CAniTex();
	~CAniTex();
	void Draw(int x, int y, double fTime=0.0f, Color32 color=0xFFFFFFFF);
	void Draw(const RECT& rcDest, double fTime=0.0f, Color32 color=0xFFFFFFFF);
public:
	int		m_nTexID;
	int		m_nXCount;
	int		m_nYCount;
	int		m_nStart;
	int		m_nCount;
	int		m_nSpeed;
};