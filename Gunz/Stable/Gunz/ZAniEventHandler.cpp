#include "stdafx.h"

#include "ZAniEventHandler.h"

void ZAniEventHandler::ZAniEventHandlerCB(RAniEventInfo* pAniEventInfo)//�̺�Ʈ�� �߻��Ҷ� �Ҹ��� ������ �ٸ��� üũ�� �ʿ���� �׳� �÷��̸� ���ָ� �ȴ�. 
{
	char* cEventType = pAniEventInfo->GetEventType();
	
	if( strcmp(cEventType , "sound") == 0)
	{
		char* cFileName = pAniEventInfo->GetFileName();
		ZGetSoundEngine()->PlaySound(cFileName, pAniEventInfo->m_vPos, false);
	}
}