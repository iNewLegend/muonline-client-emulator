// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once

#include "UIEditBox.h"

//-----------------------------------------------------------------------------
// IME-enabled EditBox control
//-----------------------------------------------------------------------------
class CUIIMEEditBox : public CUIEditBox
{
public:
    CUIIMEEditBox();
    virtual ~CUIIMEEditBox();

	virtual void XMLParse(const TiXmlElement* pControlElement);
    virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
    virtual bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnLButtonDown(POINT point);
    virtual void UpdateRects();
    virtual void OnFocusIn();
    virtual void OnFocusOut();

    void PumpMessage();

    virtual void RenderCandidateReadingWindow(float fElapsedTime, bool bReading);
    virtual void RenderComposition(float fElapsedTime);
    virtual void RenderIndicator(float fElapsedTime);
protected:
	void TruncateCompString(bool bUseBackSpace = true, int iNewStrLen = 0);
	void FinalizeString(bool bSend);

	void SendCompString();
protected:
    // Color of various IME elements
    Color32       m_ReadingColor;        // Reading string color
    Color32       m_ReadingWinColor;     // Reading window color
    Color32       m_ReadingSelColor;     // Selected character in reading string
    Color32       m_ReadingSelBkColor;   // Background color for selected char in reading str
    Color32       m_CandidateColor;      // Candidate string color
    Color32       m_CandidateWinColor;   // Candidate window color
    Color32       m_CandidateSelColor;   // Selected candidate string color
    Color32       m_CandidateSelBkColor; // Selected candidate background color
    Color32       m_CompColor;           // Composition string color
    Color32       m_CompWinColor;        // Composition string window color
    Color32       m_CompCaretColor;      // Composition string caret color
    Color32       m_CompTargetColor;     // Composition string target converted color
    Color32       m_CompTargetBkColor;   // Composition string target converted background
    Color32       m_CompTargetNonColor;  // Composition string target non-converted color
    Color32       m_CompTargetNonBkColor;// Composition string target non-converted background
    Color32       m_IndicatorImeColor;   // Indicator text color for IME
    Color32       m_IndicatorEngColor;   // Indicator text color for English
    Color32       m_IndicatorBkColor;    // Indicator text background color

    // Edit-control-specific data
    int            m_nIndicatorWidth;     // Width of the indicator symbol
    RECT           m_rcIndicator;         // Rectangle for drawing the indicator button
};