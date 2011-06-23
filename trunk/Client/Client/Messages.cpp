#include "Messages.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CMessages::CMessages(void)
{
	prepareClass();	
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CMessages::~CMessages(void)
{

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMessages::prepareClass()
{
	m_TextRender.load("Data\\Fonts\\l_10646.ttf");
	m_TextRender.setFontSize(MSG_FONT_HEIGHT-1);
	// ----
	m_xPos			= 0;
	m_yPos			= 0;
	// ----
	m_Rect.top		= 0;
	m_Rect.bottom	= 150;
	m_Rect.left		= 5;
	// ----
	m_fTimeUpdate	= 0.0f;
}
//----------------------------------------------------------------------------z-------------------------------------------------------------------------------------------------------

void CMessages::frameMove(double fTime, float fElapsedTime)
{
	if(m_Msg.empty() == false)
	{
		m_fTimeUpdate += fElapsedTime;
		// ----
		if(m_fTimeUpdate > MESSAGES_UPDATE_TIME)
		{
			m_fTimeUpdate = 0.0f;
			// ----
			m_Msg.erase(m_Msg.begin());
		}
	}
	// ----
	m_TextRender.OnFrameMove();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMessages::frameRender()
{
	m_yPos = m_Rect.top;
	// ----
	for(int i = 0 ; i != m_Msg.size() ; i++)
	{
		if(m_yPos >= m_Rect.bottom)
		{
			m_Msg.erase(m_Msg.begin());
			// ----
			break;
		}
		if(m_Msg.at(i).Text.empty() == true)
		{
			continue;
		}
		//m_TextRender.drawText(m_Msg.at(i).Text.c_str(), m_Rect.left, m_yPos, m_Msg.at(i).Color);
		// more cool
		RECT rc = {m_Rect.left, m_yPos,m_Rect.left+1000, m_yPos+MSG_FONT_HEIGHT};
		m_TextRender.DrawUBB(m_Msg.at(i).Text.c_str(),rc);
		// ----
		m_yPos += MSG_FONT_HEIGHT;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMessages::addTextBase(std::wstring Text , COLORREF Color)
{
	MSG_INFO msg;
	// ----
	msg.Text = Text;
	msg.Color = Color;
	// ----
	m_Msg.push_back(msg);
	// ----
	// # Clear Update Time
	// ----
	m_fTimeUpdate = 0.0f;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------