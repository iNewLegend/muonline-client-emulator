#include "stdafx.h"
#include "ZDuelTournamentRankingListBox.h"

ZDuelTournamentRankingListBox::ZDuelTournamentRankingListBox(const char* szName, MWidget* pParent, MListener* pListener) 
: MWidget(szName, pParent, pListener)
{
	m_pBmpRankingItemBg = NULL;
	m_pBmpArrowUp = NULL;
	m_pBmpArrowDown = NULL;
	m_pBmpArrowBar = NULL;

	m_nMyRankIndex = -1;
}

ZDuelTournamentRankingListBox::~ZDuelTournamentRankingListBox()
{

}

void ZDuelTournamentRankingListBox::ClearAll()
{
	for (int i=0; i<NUM_DISPLAY_DUELTOURNAMENT_RANKING; ++i)
	{
		m_rankingList[i].bEmptyItem = true;
	}
}

void ZDuelTournamentRankingListBox::LoadInterfaceImgs()
{
	if (m_pBmpRankingItemBg == NULL) {
		m_pBmpRankingItemBg = new MBitmapR2;
		((MBitmapR2*)m_pBmpRankingItemBg)->Create( "DuelTournamentRankingItemBg.png", RGetDevice(), "Interface/loadable/DuelTournamentRankingItemBg.png");
	}
	if (m_pBmpArrowUp == NULL) {
		m_pBmpArrowUp = new MBitmapR2;
		((MBitmapR2*)m_pBmpArrowUp)->Create( "arrow_up.tga", RGetDevice(), "Interface/loadable/arrow_up.tga");
	}
	if (m_pBmpArrowDown == NULL) {
		m_pBmpArrowDown = new MBitmapR2;
		((MBitmapR2*)m_pBmpArrowDown)->Create( "arrow_down.tga", RGetDevice(), "Interface/loadable/arrow_down.tga");
	}
	if (m_pBmpArrowBar == NULL) {
		m_pBmpArrowBar = new MBitmapR2;
		((MBitmapR2*)m_pBmpArrowBar)->Create( "arrow_bar.tga", RGetDevice(), "Interface/loadable/arrow_bar.tga");
	}
}

void ZDuelTournamentRankingListBox::UnloadInterfaceImgs()
{
	SAFE_DELETE(m_pBmpRankingItemBg);
	SAFE_DELETE(m_pBmpArrowUp);
	SAFE_DELETE(m_pBmpArrowDown);
	SAFE_DELETE(m_pBmpArrowBar);
}

void ZDuelTournamentRankingListBox::SetRankInfo( unsigned int nIndex, const ZDUELTOURNAMENTRANKINGITEM& rankingItem )
{
	if (nIndex >= NUM_DISPLAY_DUELTOURNAMENT_RANKING) { _ASSERT(0); return; }
	
	m_rankingList[nIndex] = rankingItem;
}

void ZDuelTournamentRankingListBox::OnDraw( MDrawContext* pDC )
{
	if (!m_pBmpRankingItemBg || !m_pBmpArrowUp || !m_pBmpArrowDown || !m_pBmpArrowBar) { _ASSERT(0); return; }

	const int nWidth = this->GetRect().w;
	const int nHeight = this->GetRect().h;
	const int nY_firstItem = nHeight * 0.17f;	// ����Ʈ ù° �׸��� y��ġ
	const int nItemHeight = (int)(nHeight / 7.f);

	char szTemp[128];
	MRECT rc;

	for (int i=0; i<NUM_DISPLAY_DUELTOURNAMENT_RANKING; ++i)
	{
		int y = (nY_firstItem + nItemHeight * i) - (nItemHeight * 0.04f);

		// ����Ʈ������ ����̹���
		pDC->SetBitmap(m_pBmpRankingItemBg);
		pDC->Draw(0, y, nWidth, nItemHeight);

		// �׸� ���� �׸���
		ZDUELTOURNAMENTRANKINGITEM* pRankItem = &m_rankingList[i];

		if (pRankItem->bEmptyItem) continue;

		y = nY_firstItem + nItemHeight * i;

		pDC->SetColor(MCOLOR(0xFFFFFFFF));

		// ����
		if (pRankItem->nRank == -1)	// �ʱ�ȭ ���� �������ĵ��� �ʾ����� -1�� �Ǿ� ����
			strcpy(szTemp, "--");
		else
			sprintf(szTemp, "%d", pRankItem->nRank);
		rc.Set((int)(0.01f*nWidth), y, (int)(fabs(0.01f - 0.11f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, MAM_RIGHT|MAM_VCENTER);

		// ������� ȭ��ǥ
		if (pRankItem->nFluctuation == 0)
		{
			pDC->SetBitmap(m_pBmpArrowBar);
			pDC->Draw((int)(0.13f*nWidth), y + (nItemHeight - m_pBmpArrowBar->GetHeight()) * 0.5f);
		}
		else
		{
			if (pRankItem->nFluctuation > 0)
				pDC->SetBitmap(m_pBmpArrowUp);
			else if (pRankItem->nFluctuation < 0)
				pDC->SetBitmap(m_pBmpArrowDown);

			pDC->Draw((int)(0.13f*nWidth), y + (nItemHeight - m_pBmpArrowBar->GetHeight()) * 0.5f);
			
			// ���������
			pDC->SetColor(MCOLOR(0xFFAAFF00));
			sprintf(szTemp, "%d", abs(pRankItem->nFluctuation));
			//pDC->Text((int)(0.04f*nWidth), y, szTemp);
			rc.Set((int)(0.16f*nWidth), y, (int)(fabs(0.16f - 0.26f)*nWidth), nItemHeight);
			pDC->Text(rc, szTemp, MAM_LEFT|MAM_VCENTER);
			pDC->SetColor(MCOLOR(0xFFFFFFFF));
		}

		// ��ʸ�Ʈ ��� ����
		GetDuelTournamentGradeIconFileName(szTemp, pRankItem->nGrade);
		MBitmap* pGradeIcon = MBitmapManager::Get(szTemp);
		if (pGradeIcon) {
			//int height1px = int(600.f/MGetWorkspaceHeight() + 0.5f);
			
			int margin = (nItemHeight * 0.05f + 0.5f);
			int size = nItemHeight-(margin*4);
			pDC->SetBitmap(pGradeIcon);
			rc.Set((int)(0.279f*nWidth), y+margin, size, size);//(int)(0.055f*nWidth), nItemHeight-3);
			pDC->Draw(rc);
		}


		// �̸�
		rc.Set((int)(0.34f*nWidth), y, (int)(fabs(0.34f - 0.55f)*nWidth), nItemHeight);
		pDC->Text(rc, pRankItem->szCharName, MAM_HCENTER|MAM_VCENTER);

		// ����
		sprintf(szTemp, "%d", pRankItem->nWins);
		rc.Set((int)(0.57f*nWidth), y, (int)(fabs(0.57f - 0.67f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, MAM_RIGHT|MAM_VCENTER);
		rc.Set((int)(0.67f*nWidth), y, (int)(fabs(0.67f - 0.68f)*nWidth), nItemHeight);
		pDC->Text(rc, "/", MAM_HCENTER|MAM_VCENTER);
		sprintf(szTemp, "%d", pRankItem->nLosses);
		rc.Set((int)(0.68f*nWidth), y, (int)(fabs(0.68f - 0.78f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, MAM_LEFT|MAM_VCENTER);

		// ��¼�
		sprintf(szTemp, "%d", pRankItem->nWinners);
		rc.Set((int)(0.8f*nWidth), y, (int)(fabs(0.8f - 0.91f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, MAM_RIGHT|MAM_VCENTER);

		// ��ʸ�Ʈ ����Ʈ
		sprintf(szTemp, "%d", pRankItem->nPoint);
		rc.Set((int)(0.92f*nWidth), y, (int)(fabs(0.92f - 0.99f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, MAM_RIGHT|MAM_VCENTER);

		// �� ��ŷ �׸��� ��� ���̶���Ʈ �̹��� ������
		if (i == m_nMyRankIndex)
		{
			MBitmapR2 *pBitmap=(MBitmapR2*)MBitmapManager::Get("button_glow.png");
			if(pBitmap) {
				DWORD defaultcolor = 0x333333;
				DWORD opacity=(DWORD)pDC->GetOpacity();
				MRECT rt(0, y, nWidth, nItemHeight);
				MDrawEffect prevEffect = pDC->GetEffect();
				pDC->SetEffect(MDE_ADD);
				MCOLOR prevColor = pDC->GetBitmapColor();
				pDC->SetBitmapColor(MCOLOR(defaultcolor));
				unsigned char prevOpacity = pDC->GetOpacity();
				pDC->SetOpacity(opacity);
				pDC->SetBitmap(pBitmap);
				pDC->Draw(rt.x,rt.y,rt.w,rt.h,0,0,64,32);
				pDC->SetBitmapColor(prevColor);
				pDC->SetEffect(prevEffect);
				pDC->SetOpacity(prevOpacity);
			}
		}
	}
}


