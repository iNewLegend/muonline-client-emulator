/*
 *	CMError.h
 *		Erroró���� ���� �Լ� �� ���� �ڵ� ����
 *		����ȣ ( 98-01-04 1:19:44 ���� )
 *
 *		SetError(CodeNum)
 *		SetErrors(CodeNum,SubStr)
 *			�� ����� ���� ���¸� �����Ѵ�.
 ********************************************************************/

#ifndef _CMERROR_H
#define _CMERROR_H

#include "CMErrorDef.h"

#ifdef	_WIN32
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	_KOREAN_VER		/*	Korean Version	*/

	#ifdef __BORLANDC__

		#define SetError(_nErrCode)	\
			_SetError(_nErrCode,NULL,__FILE__,__LINE__,__TIME__,KSTR_##_nErrCode)
		#define SetErrors(_nErrCode,_pErrSubStr)	\
			_SetError(_nErrCode,_pErrSubStr,__FILE__,__LINE__,__TIME__,KSTR_##_nErrCode)

		#define ERROR_MESSAGE_TITLE		"����"

	#else

		#define SetError(_nErrCode)	\
		_SetError(_nErrCode,NULL,__FILE__,__LINE__,__TIMESTAMP__,KSTR_##_nErrCode)
		#define SetErrors(_nErrCode,_pErrSubStr)	\
			_SetError(_nErrCode,_pErrSubStr,__FILE__,__LINE__,__TIMESTAMP__,KSTR_##_nErrCode)
	
		#define ERROR_MESSAGE_TITLE		"����"

	#endif	// __BORLANDC__

#else	/*	English Version	(Default)	*/

	#ifdef __BORLANDC__

		#define SetError(_nErrCode)	\
			_SetError(_nErrCode,NULL,__FILE__,__LINE__,__TIME__,ESTR_##_nErrCode)
		#define SetErrors(_nErrCode,_pErrSubStr)	\
			_SetError(_nErrCode,_pErrSubStr,__FILE__,__LINE__,__TIME__,ESTR_##_nErrCode)
		#define ERROR_MESSAGE_TITLE		"Error"

	#else

		#define SetError(_nErrCode)	\
			_SetError(_nErrCode,NULL,__FILE__,__LINE__,__TIMESTAMP__,ESTR_##_nErrCode)
		#define SetErrors(_nErrCode,_pErrSubStr)	\
			_SetError(_nErrCode,_pErrSubStr,__FILE__,__LINE__,__TIMESTAMP__,ESTR_##_nErrCode)
		#define ERROR_MESSAGE_TITLE		"Error"

	#endif // __BORLANDC__

#endif

/*
���� ����
	nErrCode			���� �ڵ�
	pErrSubStr			���� �ڵ忡 ���� �ΰ� ��Ʈ��
	pFileName			������ �Ͼ ���� ��(__FILE__)
	nLineNum			������ �Ͼ ���� ��(__LINE__)
	pLastModification	������ �Ͼ ������ �ֱ� ������(__TIMESTAMP__)
	pErrStr				���� ��Ʈ��(�����ڵ� + _KSTR or _ESTR)
*/
void _SetError(int nErrCode,const char *pErrSubStr,const char *pFileName,int nLineNum,const char *pLastModification,const char *pErrStr);

/*
�ΰ� ���� ��Ʈ�� ����
	pErrSubStr			���� �ڵ忡 ���� �ΰ� ��Ʈ��
*/
void SetErrorSubStr(const char *pErrSubStr);
/*
���� �ڵ� ���
*/
int GetErrorCode(void);
/*
���� ��Ʈ�� ���
*/
char *GetErrorString(void);
/*
�ΰ� ���� ��Ʈ�� ���
*/
char *GetErrorSubString(void);
/*
������ �� ���ϸ� ���
*/
char *GetFileName(void);
/*
������ �� ���� �� ���
*/
int GetLineNumber(void);
/*
������ �� ������ �ֱ� ���� �� ���
*/
char *GetLastModification(void);

#ifdef	_WIN32
/*
���� �޼��� ���
*/
void ErrMsgBox(HWND hWnd);
#endif

#ifdef __cplusplus
}
#endif


#endif	/*	_CMERROR_H	*/
