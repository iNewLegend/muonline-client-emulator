/*++ HackShield SDK header file   

Copyright (c) 2003-2005  AhnLab, Inc.

Module Name:

    HShield.h

Abstract:

    This header file defines the data types and constants, and exposes APIs 
	that are defined by HackShield Library.

Tag Information:
	
	This header file is auto-generated at 2006-03-29 12:17:03.

--*/
           
#ifndef _HSHIELD_H_INC
#define _HSHIELD_H_INC

// HShield Library Version Info
#define STATE_DEV		0x00
#define STATE_ALPHA		0x01
#define STATE_BETA		0x02
#define STATE_RC_RTM	0x03

#define	HSVERSION(M,m,p,s1,s2,b) (M&0x0F)<<28|(m&0x0F)<<24|(p&0x1F)<<19|(s1&0x03)<<17|(s2&0x07)<<14|(b&0x3FFF)

#define HSHIELD_PACKAGE_VER		HSVERSION (4,0,1,STATE_RC_RTM,1,48)
#define HSHIELD_PACKAGE_CRC		_T("6H7YJK4dCUulw5XHQsTQeQ==")


// Initialize Option
#define AHNHS_CHKOPT_SPEEDHACK					0x2
#define AHNHS_CHKOPT_READWRITEPROCESSMEMORY 	0x4
#define AHNHS_CHKOPT_KDTRACER					0x8
#define AHNHS_CHKOPT_OPENPROCESS				0x10
#define AHNHS_CHKOPT_AUTOMOUSE					0x20
#define AHNHS_CHKOPT_MESSAGEHOOK                0x40
#define AHNHS_CHKOPT_PROCESSSCAN                0x80


#define AHNHS_CHKOPT_ALL ( AHNHS_CHKOPT_SPEEDHACK \
						| AHNHS_CHKOPT_READWRITEPROCESSMEMORY \
						| AHNHS_CHKOPT_KDTRACER \
						| AHNHS_CHKOPT_OPENPROCESS \
						| AHNHS_CHKOPT_AUTOMOUSE \
						| AHNHS_CHKOPT_MESSAGEHOOK \
						| AHNHS_CHKOPT_PROCESSSCAN \
						| AHNHS_CHKOPT_HSMODULE_CHANGE \
						| AHNHS_USE_LOG_FILE )

#define AHNHS_USE_LOG_FILE                      0x100
#define AHNHS_ALLOW_SVCHOST_OPENPROCESS         0x400
#define AHNHS_ALLOW_LSASS_OPENPROCESS           0x800
#define AHNHS_ALLOW_CSRSS_OPENPROCESS           0x1000
#define AHNHS_DONOT_TERMINATE_PROCESS           0x2000
#define AHNHS_DISPLAY_HACKSHIELD_LOGO           0x4000
#define AHNHS_CHKOPT_HSMODULE_CHANGE			0x8000
#define AHNHS_CHKOPT_LOADLIBRARY				0x10000


// SpeedHack Sensing Ratio
#define	AHNHS_SPEEDHACK_SENSING_RATIO_HIGHEST	0x1
#define	AHNHS_SPEEDHACK_SENSING_RATIO_HIGH		0x2
#define	AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL	0x4
#define	AHNHS_SPEEDHACK_SENSING_RATIO_LOW		0x8
#define	AHNHS_SPEEDHACK_SENSING_RATIO_LOWEST	0x10
#define	AHNHS_SPEEDHACK_SENSING_RATIO_GAME		0x20



//ERROR CODE
#define HS_ERR_OK                               0x00000000		// �Լ� ȣ�� ����
#define HS_ERR_UNKNOWN                          0x00000001		// �� �� ���� ������ �߻��߽��ϴ�.
#define HS_ERR_INVALID_PARAM                    0x00000002		// �ùٸ��� ���� �����Դϴ�.
#define HS_ERR_NOT_INITIALIZED                  0x00000003		// �ٽ��� ����� �ʱ�ȭ���� ���� �����Դϴ�.
#define HS_ERR_COMPATIBILITY_MODE_RUNNING       0x00000004		// ���� ���μ����� ȣȯ�� ���� ����Ǿ����ϴ�.

#define HS_ERR_INVALID_LICENSE                  0x00000100		// �ùٸ��� ���� ���̼��� Ű�Դϴ�.
#define HS_ERR_INVALID_FILES                    0x00000101		// �߸��� ���� ��ġ�Ǿ����ϴ�. ���α׷��� �缳ġ�Ͻñ� �ٶ��ϴ�.
#define HS_ERR_INIT_DRV_FAILED                  0x00000102
#define HS_ERR_ANOTHER_SERVICE_RUNNING          0x00000103		// �ٸ� �����̳� ���μ������� �ٽ��带 �̹� ����ϰ� �ֽ��ϴ�.
#define HS_ERR_ALREADY_INITIALIZED              0x00000104		// �̹� �ٽ��� ����� �ʱ�ȭ�Ǿ� �ֽ��ϴ�.
#define HS_ERR_DEBUGGER_DETECT                  0x00000105		// ��ǻ�Ϳ��� ����� ������ �����Ǿ����ϴ�. ������� ������ ������Ų �ڿ� �ٽ� ��������ֽñ�ٶ��ϴ�.
#define HS_ERR_EXECUTABLE_FILE_CRACKED			0x00000106		// ���� ������ �ڵ尡 ũ�� �Ǿ����ϴ�.
#define HS_ERR_NEED_ADMIN_RIGHTS				0x00000107		// ADMIN ������ �ʿ��մϴ�.

#define HS_ERR_START_ENGINE_FAILED              0x00000200		// ��ŷ ���α׷� ���� ������ ������ �� �����ϴ�.
#define HS_ERR_ALREADY_SERVICE_RUNNING          0x00000201		// �̹� �ٽ��� ���񽺰� ���� ���Դϴ�.
#define HS_ERR_DRV_FILE_CREATE_FAILED           0x00000202		// �ٽ��� ����̹� ������ ������ �� �����ϴ�.
#define HS_ERR_REG_DRV_FILE_FAILED              0x00000203		// �ٽ��� ����̹��� ����� �� �����ϴ�.
#define HS_ERR_START_DRV_FAILED                 0x00000204		// �ٽ��� ����̹��� ������ �� �����ϴ�.

#define HS_ERR_SERVICE_NOT_RUNNING              0x00000301		// �ٽ��� ���񽺰� ����ǰ� ���� ���� �����Դϴ�.
#define HS_ERR_SERVICE_STILL_RUNNING            0x00000302		// �ٽ��� ���񽺰� ���� �������� �����Դϴ�.

#define HS_ERR_NEED_UPDATE                      0x00000401		// �ٽ��� ����� ������Ʈ�� �ʿ��մϴ�.

#define HS_ERR_API_IS_HOOKED					0x00000501		// �ش� API�� ��ŷ�Ǿ� �ִ� �����Դϴ�.


// Server-Client ���� ����  ERROR CODE
#define HS_ERR_ANTICPCNT_MAKEACKMSG_INVALIDPARAM				0x00010000
#define HS_ERR_ANTICPCNT_MAKEACKMSG_MAKESESSIONKEY_FAIL			0x00010001
#define HS_ERR_ANTICPCNT_MAKEACKMSG_INITCRYPT_FAIL				0x00010002
#define HS_ERR_ANTICPCNT_MAKEACKMSG_DECRYPTMESSAGE_FAIL			0x00010003
#define HS_ERR_ANTICPCNT_MAKEACKMSG_GETMEMHASH_FAIL				0x00010004
#define HS_ERR_ANTICPCNT_MAKEACKMSG_GETMODNAME_FAIL				0x00010005
#define HS_ERR_ANTICPCNT_MAKEACKMSG_GETFILEHASH_FAIL			0x00010006
#define HS_ERR_ANTICPCNT_MAKEACKMSG_GETHSHIELDHASH_FAIL			0x00010007
#define HS_ERR_ANTICPCNT_MAKEACKMSG_MAKEHSHIELDFLAG_FAIL		0x00010008
#define HS_ERR_ANTICPCNT_MAKEACKMSG_MAKEACKSEQUENCENUM_FAIL		0x00010009
#define HS_ERR_ANTICPCNT_MAKEACKMSG_ENCRYPTMESSAGE_FAIL			0x0001000A

#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_INVALIDPARAM			0x00010010
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_MAKESESSIONKEY_FAIL		0x00010011
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_INITCRYPT_FAIL			0x00010012
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_DECRYPTMESSAGE_FAIL		0x00010013
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_GETGUIDFROMFILE_FAIL	0x00010014
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_MAKEACKSEQUENCENUM_FAIL	0x00010015
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_ENCRYPTMESSAGE_FAIL		0x00010016	

#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_INVALIDPARAM			0x00010020
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_OPENFILEMAPPING_FAIL	0x00010021
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_MAPVIEWOFFILE_FAIL		0x00010022
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_INITCRYPT_FAIL			0x00010023
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_DECRYPTMESSAGE_FAIL	0x00010024
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_INVALIDMMF				0x00010025
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_OPENEVENT_FAIL			0x00010026
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_GETMODFILENAME_FAIL	0x00010027
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_GETINSTRUCTION_FAIL	0x00010028
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_ENCRYPTMESSAGE_FAIL	0x00010029

#define HS_ERR_ANTICPCNT_DEBUGGER_DETECTED						0x00010030


// CallBack Code
#define AHNHS_ACTAPC_DETECT_ALREADYHOOKED		0x010101			// �Ϻ� API�� �̹� ��ŷ�Ǿ� �ִ� �����Դϴ�. (�׷��� �����δ� �̸� �����ϰ� �ֱ� ������ ��ŷ���α׷��� �������� �ʽ��ϴ�.)
#define AHNHS_ACTAPC_DETECT_AUTOMOUSE			0x010102			// ���丶�콺 ������ �����Ǿ����ϴ�.	
#define AHNHS_ACTAPC_DETECT_HOOKFUNCTION		0x010301			// ��ȣ API�� ���� ��ŷ ������ �����Ǿ����ϴ�.
#define AHNHS_ACTAPC_DETECT_DRIVERFAILED		0x010302			// ��ŷ ���� ����̹��� �ε���� �ʾҽ��ϴ�.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK			0x010303			// ���ǵ��ٷ��� ���α׷��� ���� �ý��� �ð��� ����Ǿ����ϴ�.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK_APP		0x010304			// ���ǵ��ٷ��� ���α׷��� ���� �ý��� �ð��� ����Ǿ����ϴ�.
#define AHNHS_ACTAPC_DETECT_MESSAGEHOOK			0x010306			// �޽��� ��ŷ�� �õ��Ǿ����� �̸� �������� ���߽��ϴ�.
#define AHNHS_ACTAPC_DETECT_KDTRACE				0x010307			// ����� Ʈ���̽��� �߻��ߴ�.(Ŀ�� ����� Ȱ��ȭ, ���� �극��ũ ������ ó��)
#define AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED		0x010308			// ��ġ�� ����� Ʈ���̽��� ����Ǿ���.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK_RATIO		0x01030B			// ���ǵ��� ���� �ɼ��� 'GAME'�� ��� �� �ݹ����� �ֱ� 5�ʵ����� �ð������� ���޵˴ϴ�.
#define AHNHS_ENGINE_DETECT_GAME_HACK           0x010501			// ���� ��ŷ���� ������ �߰ߵǾ����ϴ�.
#define AHNHS_ENGINE_DETECT_GENERAL_HACK        0x010502			// �Ϲ� ��ŷ��(Ʈ���̸� ����)�� �߰ߵǾ����ϴ�.
#define AHNHS_ACTAPC_DETECT_MODULE_CHANGE       0x010701			// �ٽ��� ���ø���� ����Ǿ����ϴ�.


// CallBack�� ���޵Ǵ� ������
#pragma pack(1)

typedef struct _ACTAPCPARAM_DETECT_HOOKFUNCTION
{	
	char szFunctionName[128];		// file path
	char szModuleName[128];
	
} ACTAPCPARAM_DETECT_HOOKFUNCTION, *PACTAPCPARAM_DETECT_HOOKFUNCTION;


// ���丶�콺 ���� APC Structure
typedef struct
{
	BYTE	byDetectType;			// AutoMouse ���� API ȣ�� 1, AutoMouse ���� API ���� 2
	DWORD	dwPID;					// AutoMouse ���μ��� �Ǵ� API �� ������ ���μ���
	CHAR	szProcessName[16+1];	// ���μ�����
	CHAR	szAPIName[128];			// ȣ��� API �� �Ǵ� ������ API ��
}ACTAPCPARAM_DETECT_AUTOMOUSE, *PACTAPCPARAM_DETECT_AUTOMOUSE;


#define	EAGLE_AUTOMOUSE_APCTYPE_API_CALLED			1
#define	EAGLE_AUTOMOUSE_APCTYPE_API_ALTERATION		2
#define	EAGLE_AUTOMOUSE_APCTYPE_SHAREDMEMORY_ALTERATION	3

#pragma pack()


// Server-Client ���� ������ ���Ǵ� ������ ũ��
#define SIZEOF_REQMSG		160
#define SIZEOF_ACKMSG		56
#define SIZEOF_GUIDREQMSG	20
#define SIZEOF_GUIDACKMSG	20


// Callback Function Prototype
typedef int (__stdcall* PFN_AhnHS_Callback)(
	long lCode,
	long lParamSize,
	void* pParam 
);


// Export API
#if defined(__cplusplus)
extern "C"
{
#endif

/*!
 * 
 * @remarks	�ٽ��� �ʱ�ȭ �Լ�
 *
 * @param	szFileName : [IN] Ehsvc.dll�� Full Path
 * @param	pfn_Callback : [OUT] Callback Function Pointer
 * @param	nGameCode : [IN] Game Code
 * @param	szLicenseKey : [IN] License Code
 * @param	unOption : [IN] Initialize Options
 * @param	unSHackSensingRatio : [IN] SpeedHack Sensing Ratio
 *
 */
int
__stdcall
_AhnHS_Initialize (
	const char* szFileName,
	PFN_AhnHS_Callback pfn_Callback,
	int nGameCode,
	const char* szLicenseKey,
	unsigned int unOption,
	unsigned int unSHackSensingRatio
);


/*!
 * 
 * @remarks	�ٽ��� ���� �Լ�
 *
 */
int
__stdcall
_AhnHS_StartService ();


/*!
 * 
 * @remarks	�ٽ��� ���� �Լ�
 *
 */
int
__stdcall
_AhnHS_StopService ();


/*!
 * 
 * @remarks	�ٽ��� ��� ���� �Լ��� ����� Ű���� ���� �޽��� �Ű��� ��ɸ� ���� ��Ų��.
 *
 * @param	unPauseOption : [IN] ���� �Ϸ��� ����� �ɼ� ( ����� AHNHS_CHKOPT_MESSAGEHOOK�� ����)
 *
 */
int
__stdcall
_AhnHS_PauseService (
	unsigned int unPauseOption
	);


/*!
 * 
 * @remarks	�ٽ��� ��� �簳 �Լ��� ����� Ű���� ���� �޽��� �Ű��� ��ɸ� �簳 ��Ų��.
 *
 * @param	unResumeOption : [IN] �簳 �Ϸ��� ����� �ɼ� ( ����� AHNHS_CHKOPT_MESSAGEHOOK�� ����)
 *
 */
int
__stdcall
_AhnHS_ResumeService (
	unsigned int unResumeOption
	);


/*!
 * 
 * @remarks	�ٽ��� ���� ���� �Լ�
 *
 */
int
__stdcall
_AhnHS_Uninitialize ();


/*!
 * 
 * @remarks	�����κ��� CRC�� ��û���� ��� �ش� �޽����� ���� ����޽����� �����.
 *
 * @param	pbyReqMsg : [IN] ��ȣȭ�� Request Message, �� ������ ũ��� ���� ���ǵ� SIZEOF_REQMSG�̴�.
 * @param	pbyAckMsg : [OUT] ��ȣȭ�� Ack Message, �� ������ ũ��� ���� ���ǵ� SIZEOF_ACKMSG�̴�.
 *
 */
int
__stdcall
_AhnHS_MakeAckMsg (
	unsigned char *pbyReqMsg,
	unsigned char *pbyAckMsg
	);


/*!
 * 
 * @remarks	�����κ��� GUID�� ��û���� ��� �ش� �޽����� ���� ����޽����� �����.
 *
 * @param	pbyGuidReqMsg : [IN] ��ȣȭ�� GUID Request Message, �� ������ ũ��� ���� ���ǵ� SIZEOF_GUIDREQMSG�̴�.
 * @param	pbyGuidAckMsg : [OUT] ��ȣȭ�� GUID Ack Message, �� ������ ũ��� ���� ���ǵ� SIZEOF_GUIDACKMSG�̴�.
 *
 */
int
__stdcall
_AhnHS_MakeGuidAckMsg (
	unsigned char *pbyGuidReqMsg,
	unsigned char *pbyGuidAckMsg
	);
							

/*!
 * 
 * @remarks	�Լ� ������ ����Ʈ�� �޾Ƶ鿩 ���� ���ӻ翡 ������ ���� �������̶��
 *			�޸𸮿� ���� CRC ������ ����� ����.
 *
 * @author	Heean Park
 *
 * @date	2005/11/21
 *
 * @param	unNumberOfFunc : [IN] �Լ� ������ ����
 * @param	... : [IN] �Լ� ������ ����Ʈ
 *
 */
int
__stdcall
_AhnHS_SaveFuncAddress (
	unsigned int unNumberOfFunc,
	...
	);


int 
__stdcall
_AhnHS_CheckAPIHooked (
	const char*	szModuleName,
	const char*	szFunctionName,
	const char*	szSpecificPath
	);


#if defined(__cplusplus)
}
#endif

#endif _HSHIELD_H_INC
