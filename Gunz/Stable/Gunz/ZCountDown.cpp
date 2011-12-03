#include "stdafx.h"
#include "ZCountDown.h"
#include "ZGameInterface.h"
#include "ZApplication.h"

void OnTimer_CountDown(void* pParam)
{
	ZCOUNTDOWN *pCountDown = (ZCOUNTDOWN*)pParam;

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pTargetWidget = pResource->FindWidget(pCountDown->szTargetWidget);
	if(!pTargetWidget || !pTargetWidget->IsVisible()) return;	// ���ų� ���������� ī��Ʈ�� �����Ѵ�.

	if(pCountDown->nSeconds>0) {

		// ���� ������ �����Ѵ�
		if(pCountDown->szLabelWidget!=NULL)
		{
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MWidget* pWidget = pResource->FindWidget(pCountDown->szLabelWidget);
			if(pWidget)
			{
				char buffer[256];
				sprintf(buffer,"%d",pCountDown->nSeconds);
				pWidget->SetText(buffer);		
			}
		}

		pCountDown->nSeconds--;
		ZApplication::GetTimer()->SetTimerEvent(1000, &OnTimer_CountDown, pCountDown, true);
		return;
	}

	// 0�� �Ǿ����Ƿ� ��� ������ �����, 
	pTargetWidget->Show(false);	

	// �ݹ��� ������ �����Ѵ�
	if(pCountDown->pCallBack)
		pCountDown->pCallBack();
}

