//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* Messages.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "RenderSystem.h"
#include "RPGSkyTextRender.h"
#include "RECT.h"
#include "process.h"
#include "TColors.h"
#include <deque>
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MSG_LINE_COUNT			5
#define MSG_FONT_HEIGHT			14
#define MESSAGES_UPDATE_TIME	5.0f
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct MSG_INFO
{
	DWORD					Color;
	std::wstring			Text;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CMessages
{
private:
	std::deque<MSG_INFO>m_Msg;
	// ----
	CRect<int>			m_Rect;
	CRPGSkyTextRender	m_TextRender;
	// ----
	int					m_xPos;
	int					m_yPos;
	// ----
	float				m_fTimeUpdate;
public:
	CMessages(void);
	~CMessages(void);
	// ----
	void			prepareClass();
	void			frameRender();
	void			frameMove(double fTime, float fElapsedTime);
	// ----
	void			addTextBase(std::wstring Text, COLORREF Color);
	// ----
	void			setRect(int top, int bottom, int left) { m_Rect.top = top; m_Rect.bottom = bottom, m_Rect.left = left; } ;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------