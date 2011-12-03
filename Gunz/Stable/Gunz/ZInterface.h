#ifndef _ZINTERFACE_H
#define _ZINTERFACE_H

#include "ZIDLResource.h"
#include <windows.h>
#include <io.h>
#include "ZGame.h"
#include "RealSpace2.h"
#include "RBaseTexture.h"
#include "Mint.h"
#include "Mint4R2.h"

_USING_NAMESPACE_REALSPACE2

// interface ���� base Ŭ����

class ZInterface : public MWidget {
public:
	ZInterface(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~ZInterface();

	virtual bool OnCreate();										// �ʱ�ȭ
	virtual void OnDestroy();										// destroy �� �Ҹ�.

	virtual void OnDraw(MDrawContext* pDC);

	virtual bool IsDone();											// ���� �������̽��� ���� false�� �����Ѵ�.

	virtual bool OnEvent(MEvent* pEvent, MListener* pListener)=0;	// interface �� �׻� �̺�Ʈ�� ó���Ѵ�.

protected:
	bool m_bDone;
};

#endif