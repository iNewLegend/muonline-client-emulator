#include "stdafx.h"
#include "MAnimation.h"

#define DELAY_CONSTANT	1

void MAnimation::OnDraw(MDrawContext* pDC)
{
	if ( m_pAniBitmap == NULL)
		return;

	// Get current time
	int nCurTime = timeGetTime();

	// Get current frame
	int nCurFrame = GetCurrentFrame();
	
	// Get frame count
	int nFrameCount = m_pAniBitmap->GetFrameCount();


	if ( m_nPlayMode == MAPM_FORWARDONCE)
	{
		m_nCurrFrame = nCurFrame;

		if ( nCurFrame >= nFrameCount)
            m_nCurrFrame = nFrameCount - 1;
	}

	else if ( m_nPlayMode == MAPM_REPETITION)
		m_nCurrFrame = nCurFrame % nFrameCount;

	else if ( m_nPlayMode == MAPM_FORWARDNBACKWARD)
	{
		int nIterCount = nCurFrame / nFrameCount;
		m_nCurrFrame = nCurFrame % nFrameCount;
		if ( (nIterCount % 2) == 1)			// Ȧ����° Play�̸� Backward
			m_nCurrFrame = nFrameCount - m_nCurrFrame - 1;
	}

	MBitmap* pBitmap = m_pAniBitmap->Get( m_nCurrFrame);
	pDC->SetBitmap( pBitmap);
	MRECT r = GetClientRect();
	pDC->Draw( r.x, r.y, r.w, r.h);
}

MBitmap* MAnimation::GetBitmap()
{
	if(m_pAniBitmap == NULL) return NULL;
	int nCurTime = timeGetTime();
	int nCurFrame;
	if(m_nDelay!=0) nCurFrame = (nCurTime-m_nStartTime) / (m_nDelay*DELAY_CONSTANT);
	else nCurFrame = (nCurTime-m_nStartTime) / DELAY_CONSTANT;
	int nFrameCount = m_pAniBitmap->GetFrameCount();
	if(nFrameCount==0) return NULL;

	if(m_nPlayMode==MAPM_FORWARDONCE){
		if(nCurFrame>=nFrameCount) nCurFrame = nFrameCount-1;
	}
	else if(m_nPlayMode==MAPM_REPETITION){
		nCurFrame = nCurFrame % nFrameCount;
	}
	else if(m_nPlayMode==MAPM_FORWARDNBACKWARD){
		int nIterCount = nCurFrame/nFrameCount;
		nCurFrame = (nCurFrame%nFrameCount);
		if(nIterCount%2==1){	// Ȧ����° Play�̸� Backward
			nCurFrame = nFrameCount - nCurFrame - 1;
		}
	}

	return m_pAniBitmap->Get(nCurFrame);
}

MAnimation::MAnimation(const char* szName, MAniBitmap* pAniBitmap, MWidget* pParent)
: MWidget(szName, pParent, NULL)
{
	m_pAniBitmap = pAniBitmap;
	if(pAniBitmap!=NULL) m_nDelay = pAniBitmap->GetDelay();
	else m_nDelay = 1;	// Default
	m_nStartTime = timeGetTime();

	m_nPlayMode = MAPM_FORWARDONCE;
	m_nCurrFrame = 0;
	m_bRunAnimation = true;
}

void MAnimation::SetAniBitmap(MAniBitmap* pAniBitmap)
{
	m_pAniBitmap = pAniBitmap;
	if(pAniBitmap!=NULL) m_nDelay = pAniBitmap->GetDelay();
}

void MAnimation::InitStartTime()
{
	m_nStartTime = timeGetTime();
}


void MAnimation::SetRunAnimation( bool bRun)
{
	m_bRunAnimation = bRun;

	SetCurrentFrame( m_nCurrFrame);
}

int MAnimation::GetCurrentFrame( void)
{
	if ( !m_bRunAnimation)
		return m_nCurrFrame;

	// Get current time
	int nCurrTime = timeGetTime();

	// Get current frame
	int nCurrFrame;

	if ( m_nDelay != 0)
		nCurrFrame = (nCurrTime - m_nStartTime) / (m_nDelay * DELAY_CONSTANT);
	else
		nCurrFrame = (nCurrTime - m_nStartTime) / DELAY_CONSTANT;

	return nCurrFrame;
}

void MAnimation::SetCurrentFrame( int nFrame)
{
	m_nCurrFrame = nFrame;

	if ( nFrame < m_pAniBitmap->GetFrameCount())
        m_nStartTime = timeGetTime() - ( nFrame * m_nDelay * DELAY_CONSTANT);
}
