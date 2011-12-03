#pragma once

#include "Mint4R2.h"
#include "RFrameWork.h"
#include "ZButton.h"
#include "ZMsgBox.h"
#include "ZActionKey.h"

_USING_NAMESPACE_REALSPACE2

class Mint4Gunz : public Mint{
public:
	virtual void Update(void){
		RealSpace2::RFrame_Render();
		//RenderScene(NULL);
	}
	virtual MBitmap* OpenBitmap(const char* szName){

		char aliasname[256];
		char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
		_splitpath(szName,drive,dir,fname,ext);
		sprintf(aliasname,"%s%s",fname,ext);

		MBitmapR2* pNew = new MBitmapR2;
		bool bRet = pNew->Create(aliasname, RGetDevice(), szName);
		if(bRet==false){
			delete pNew;
			return NULL;
		}
		return pNew;
	}
	virtual MFont* OpenFont(const char* szName, int nHeight){
		MFontR2* pNew = new MFontR2;
		pNew->Create(szName, szName, nHeight);
		return pNew;
	}
	virtual MWidget* NewWidget(const char* szClass, const char* szName, MWidget* pParent, MListener* pListener){
		if(strcmp(szClass, MINT_BUTTON)==0) return new ZButton(szName, pParent, pListener);
		else if( strcmp(szClass, MINT_BMBUTTON)==0) return new ZBmButton(szName, pParent, pListener);
		else if( strcmp(szClass, MINT_MSGBOX)==0) return new ZMsgBox(szName, pParent, pListener);
		else if( strcmp(szClass, MINT_ACTIONKEY)==0) return new ZActionKey(szName, pParent, pListener);
		return Mint::NewWidget(szClass, szName, pParent, pListener);
	}

	virtual const char* GetActionKeyName(unsigned long int nKey);

	virtual void Draw(void){
		Mint::Draw();

		if (m_pDC && m_pMainFrame)
			m_pMainFrame->DrawAfterWidgets(m_pDC);
		
		//		MPOINT p = MEvent::GetMousePos();
		MPOINT p = MEvent::LatestPos;

		MCursorSystem::Draw(GetDrawContext(), p.x, p.y);	// RAONHAJE Mouse Cursor SoftwareDraw
	}
};