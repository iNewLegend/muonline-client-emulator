#ifndef _ZLOCALEDEFINE_H
#define _ZLOCALEDEFINE_H


/////////////////////////////////////////////////////////////////////////////////////////
// ������ ���� include //////////////////////////////////////////////////////////////////
#ifdef LOCALE_US			// ���ͳ׼ų�


#else
#ifdef LOCALE_JAPAN			// �Ϻ�
//	#include "extern/NetmarbleJP/NMCrypt.h"
	#include "ZGameOnJPAuth.h"
//	#pragma comment ( lib, "extern/NetmarbleJP/NMCrypt.lib"  )
#else						// �ѱ�
	#include "extern/Netmarble/NMCrypt.h"
	#include "ZNetmarble.h"

	#pragma comment ( lib, "extern/Netmarble/NMCrypt.lib"  )
#endif
#endif
/////////////////////////////////////////////////////////////////////////////////////////







#endif