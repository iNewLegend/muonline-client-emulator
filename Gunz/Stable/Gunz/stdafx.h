// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

// Windows ��� �����Դϴ�.
#include <afxdb.h>
#include <afxtempl.h>
#include <afxdtctl.h>

#include <winsock2.h>
#include <mswsock.h>

#include <crtdbg.h>

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#include <windows.h>

#include <mmsystem.h>
#include <shlwapi.h>
#include <shellapi.h>

// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <cstddef>
#include <comutil.h>
#include <stdio.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

#include "../CSCommon/MFeatureDefine.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!! ������ ���⿡ ���� ���ð� MFeatureDefine.h���� define�� �ɵ��� �մϴ�. Ŭ��/���� ���ÿ� ������ �� �ֽ��ϴ�
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _QUEST		// ����Ʈ ���߿� ������

#define _QUEST_ITEM	// ����Ʈ ������ ���߿�. //<- ���� �̰� undef�Ѵٰ� ����Ʈ������ �ڵ带 ������ ���� ���� �Ǿ����ϴ�. (������ ���� �κ��� �� ����)

#define _MONSTER_BIBLE 

//#define _GAMBLEITEM_TEST_LOG	// �׺� �������� ���������� Ȯ���� ����(��ҿ��� �ּ�ó��)

//#define _REPLAY_TEST_LOG // ���÷��̽� Ư�������� ��� Ȯ���� ����(��Ҵ� �ּ�����)

//#define _LOCATOR

#if defined(LOCALE_NHNUSA)// || defined(_DEBUG)
	#define _MULTILANGUAGE	// ���߾�� ���� ����
#endif

#if defined(_DEBUG) || defined(_RELEASE) || defined(LOCALE_KOREA) || defined(LOCALE_NHNUSA) || defined(LOCALE_JAPAN) || defined(LOCALE_BRAZIL)
	#define _DUELTOURNAMENT	// �����ʸ�Ʈ ���� ����
#endif


#ifdef _DEBUG
// #define _XTRAP // �׽�Ʈ �ϱ� ���ؼ�. by sunge
// #define _HSHIELD // �׽�Ʈ �ϱ� ���ؼ�. by sunge
// #define _XTRAP
#endif

// stl
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>

// �ܺ� ���̺귯��
#include "d3d9.h"
#include "d3dx9math.h"

#include "fmod.h"

// cml
#include "MXml.h"
#include "MUtil.h"
#include "MDebug.h"
#include "MRTTI.h"
#include "MUID.h"
#include "MemPool.h"

// mint
#include "Mint.h"
#include "MWidget.h"
#include "MBitmap.h"
#include "MButton.h"
#include "MListBox.h"
#include "MTextArea.h"
#include "MTabCtrl.h"
#include "MComboBox.h"
#include "MFrame.h"
#include "MPopupMenu.h"

// realspace2
#include "rtypes.h"
#include "RNameSpace.h"
#include "RTypes.h"
#include "RealSpace2.h"
#include "RBspObject.h"
#include "RMeshMgr.h"
#include "RVisualMeshMgr.h"
#include "RMaterialList.h"
#include "RAnimationMgr.h"
#include "Mint4R2.h"

// cscommon
#include "MObject.h"
#include "MMatchObject.h"
#include "MMatchStage.h"
#include "MMatchItem.h"
#include "MMatchMap.h"
#include "MSafeUDP.h"
#include "MMatchClient.h"
#include "MGameClient.h"
#include "MMatchTransDataType.h"
#include "MErrorTable.h"
#include "Config.h"
#include "MSharedCommandTable.h"
#include "MObjectTypes.h"
#include "MMatchBuff.h"

// gunz global
#include "ZApplication.h"
#include "ZGlobal.h"
#include "ZMessages.h"
#include "ZStringResManager.h"
#include "ZGameInterface.h"
#include "ZCombatInterface.h"
#include "ZGame.h"
#include "ZGameClient.h"
#include "ZObject.h"
#include "ZIDLResource.h"
#include "ZInterfaceListener.h"
#include "ZColorTable.h"
#include "ZMyInfo.h"
#include "ZMyItemList.h"
#include "ZNetRepository.h"
#include "ZItem.h"
#include "ZItemDesc.h"
#include "ZPost.h"
#include "ZSoundEngine.h"
#include "ZSoundFMod.h"
#include "ZCamera.h"
#include "ZCharacter.h"
#include "ZActor.h"