#include "stdafx.h"

#include "ZApplication.h"
#include "ZMapListBox.h"
#include "MListBox.h"
#include "MZFileSystem.h"
#include "Mint4R2.h"
#include "MTextArea.h"
#include "ZChannelRule.h"

#define PREVIEW_W	200

void ZMapListBox::OnDraw(MDrawContext* pDC)
{
	if(m_pThumbnail!=NULL){
		MRECT r = GetClientRect();
		pDC->SetBitmap(m_pThumbnail);
		pDC->Draw(r.x+r.w-PREVIEW_W+10, r.y);
	}
}

bool ZMapListBox::OnShow(void)
{
	Refresh(ZApplication::GetFileSystem());

	if(m_pThumbnail!=NULL){
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}

	// ó�� �������� ����Ų��.
	SetSelIndex(0);

	m_pListBox->SetFocus();

	return true;
}

bool ZMapListBox::OnCommand(MWidget* pWidget, const char* szMessage)
{
	if(pWidget==m_pListBox && strcmp(szMessage, MLB_ITEM_SEL)==0){
		SetSelIndex(-1);
		return true;
	}
	else if(pWidget==m_pListBox && strcmp(szMessage, MLB_ITEM_DBLCLK)==0){
		if(GetListener()!=NULL)return GetListener()->OnCommand(pWidget, szMessage);
	}
	return false;
}

ZMapListBox::ZMapListBox(const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	SetBounds(0, 0, 300, 200);

	MRECT r = GetClientRect();

	m_pListBox = new MListBox("MapList", this, this);
	m_pListBox->SetBounds(r.x, r.y, r.w-PREVIEW_W-10, r.h-r.y);
	m_pListBox->m_Anchors.m_bLeft = true;
	m_pListBox->m_Anchors.m_bTop = true;
	m_pListBox->m_Anchors.m_bRight = true;
	m_pListBox->m_Anchors.m_bBottom = true;

	m_pThumbnail = NULL;
}

ZMapListBox::~ZMapListBox(void)
{
	if(m_pThumbnail!=NULL){
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}
	delete m_pListBox;
}

void ZMapListBox::Refresh(MZFileSystem* pFS)
{
	m_pListBox->RemoveAll();

#define EXT	".rs"
#define MAPDIRECTORY "Maps/"

	MChannelRule* pRule = ZGetChannelRuleMgr()->GetCurrentRule();
	if (pRule == NULL) {
		mlog("ZMapListBox::Refresh() > No Current ChannelRule \n");
		return;
	}

	int nExtLen = (int)strlen(EXT);
	for(int i=0; i<pFS->GetFileCount(); i++){
		const char* szFileName = pFS->GetFileName(i);
		const MZFILEDESC* desc = pFS->GetFileDesc(i);
		int nLen = (int)strlen(szFileName);

		if( strnicmp(desc->m_szFileName,MAPDIRECTORY,strlen(MAPDIRECTORY))==0 &&
			nLen>nExtLen && stricmp(szFileName+nLen-nExtLen, EXT)==0 )
		{
			char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
			_splitpath(szFileName,drive,dir,fname,ext);

			if (ZIsLaunchDevelop()) 
			{
				m_pListBox->Add(fname);
//			} else {
//				if (pRule->CheckMap(fname))
//					m_pListBox->Add(fname);
			}
		}
	}
}

const char* ZMapListBox::GetSelItemString(void)
{
	return m_pListBox->GetSelItemString();
}

void ZMapListBox::SetSelIndex(int i)
{
	if(i>=0 && i<m_pListBox->GetCount()) m_pListBox->SetSelIndex(i);

	if(m_pThumbnail!=NULL){
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}

	const char* szName = m_pListBox->GetSelItemString();
	if(szName==NULL) return;
	char szThumbnail[256];
	sprintf(szThumbnail, "%s.rs.bmp", szName);

	m_pThumbnail = Mint::GetInstance()->OpenBitmap(szThumbnail);
}
