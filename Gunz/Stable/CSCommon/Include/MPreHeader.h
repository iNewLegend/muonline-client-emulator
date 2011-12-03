// MPreHeader.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#include "ODBCRecordset.h"
#include "winsock2.h"
#include "MXml.h"

// Windows ��� �����Դϴ�.
#include <windows.h>
// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#pragma warning(disable:4786)
#include <list>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
using namespace std;

#include <crtdbg.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include "MUID.h"
#include "MDebug.h"
#include "MCRC32.h"
#include "MObjectTypes.h"
#include "MFormulaTable.h"
#include "MErrorTable.h"
#include "MSharedCommandTable.h"
#include "MMatchGlobal.h"
#include "MMatchFormula.h"

#include "MCommand.h"
#include "MCommandManager.h"
#include "MCommandParameter.h"
#include "Msg.h"
#include "MPacket.h"

/*
#include "MMatchDBMgr.h"
#include "MServer.h"
#include "MMatchObject.h"
#include "MAgentObject.h"
#include "MMatchChannel.h"
#include "MMatchStage.h"
#include "MSafeUDP.h"
#include "MMatchTransDataType.h"
#include "MMatchAdmin.h"
#include "MAsyncProxy.h"
#include "MMatchGlobal.h"
#include "MMatchShutdown.h"
#include "MMatchChatRoom.h"
#include "MTypes.h"
*/