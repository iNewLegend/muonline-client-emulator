/*
* (C) Copyright 2005. Ahn Lab, Inc.
*
* Any part of this source code can not be copied with
* any method without prior written permission from
* the author or authorized person.
*
*/

/*
* File Name : HsCryptLib.h
* Author: Nam Sung il(oncea@ahnlab.com)
* Revision History
* Date         Name                    Description
* 2005-07-27   Nam, Sung-Il(siNam)     Created
*
*/
#ifndef _HSCRYPTLIB_H
#define _HSCRYPTLIB_H

#if defined(__cplusplus)
extern "C"
{
#endif

#define HSCRYPTLIB_INITKEY_SIZE		16
#define HSCRYPTLIB_KEY_SIZE			550

#pragma pack(8)
// Ű����ü.
typedef struct _HSCRYPT_KEYINFO
{
	BYTE  byInitKey[HSCRYPTLIB_INITKEY_SIZE];
	BYTE  AesEncKey[HSCRYPTLIB_KEY_SIZE];
	BYTE  AesDecKey[HSCRYPTLIB_KEY_SIZE];
} HSCRYPT_KEYINFO, *PHSCRYPT_KEYINFO;
#pragma pack()

// ERROR CODE DEFINE
#define ERROR_HSCRYPTLIB_BASECODE						0x0001B000
#define ERROR_HSCRYPTLIB_UNKNOWN						ERROR_HSCRYPTLIB_BASECODE + 0x0
#define ERROR_HSCRYPTLIB_EXCEPTION						ERROR_HSCRYPTLIB_BASECODE + 0x1
#define ERROR_HSCRYPTLIB_INITCRYPT_INVALIDPARAM			ERROR_HSCRYPTLIB_BASECODE + 0x2
#define ERROR_HSCRYPTLIB_GETENCMSG_INVALIDPARAM			ERROR_HSCRYPTLIB_BASECODE + 0x3
#define ERROR_HSCRYPTLIB_GETDECMSG_INVALIDPARAM			ERROR_HSCRYPTLIB_BASECODE + 0x4

#define ERROR_HSCRYPTLIB_FREAD_INVALIDPARAM				ERROR_HSCRYPTLIB_BASECODE + 0x5
#define ERROR_HSCRYPTLIB_FREAD_DECRYPT_RANGE			ERROR_HSCRYPTLIB_BASECODE + 0x6
#define ERROR_HSCRYPTLIB_FREAD_DECRYPT_FREAD			ERROR_HSCRYPTLIB_BASECODE + 0x7
#define ERROR_HSCRYPTLIB_FREAD_DECRYPT_GETDECMSG		ERROR_HSCRYPTLIB_BASECODE + 0x8
#define ERROR_HSCRYPTLIB_FREAD_GETFILELEN				ERROR_HSCRYPTLIB_BASECODE + 0x9
#define ERROR_HSCRYPTLIB_FREAD_GETPOSITION				ERROR_HSCRYPTLIB_BASECODE + 0xA
#define ERROR_HSCRYPTLIB_FREAD_SIZEZERO					ERROR_HSCRYPTLIB_BASECODE + 0xB
#define ERROR_HSCRYPTLIB_FREAD_FSEEK					ERROR_HSCRYPTLIB_BASECODE + 0xC

#define ERROR_HSCRYPTLIB_FILEFENC_INVALIDPARAM			ERROR_HSCRYPTLIB_BASECODE + 0x10
#define ERROR_HSCRYPTLIB_FILEFENC_FOPEN					ERROR_HSCRYPTLIB_BASECODE + 0x11
#define ERROR_HSCRYPTLIB_FILEFENC_GETFILELEN			ERROR_HSCRYPTLIB_BASECODE + 0x12	
#define ERROR_HSCRYPTLIB_FILEFENC_SIZEZERO				ERROR_HSCRYPTLIB_BASECODE + 0x13
#define ERROR_HSCRYPTLIB_FILEFENC_INITCRYPT				ERROR_HSCRYPTLIB_BASECODE + 0x14
#define ERROR_HSCRYPTLIB_FILEFENC_COPYFILE				ERROR_HSCRYPTLIB_BASECODE + 0x15
#define ERROR_HSCRYPTLIB_FILEFENC_ENCFILE				ERROR_HSCRYPTLIB_BASECODE + 0x16
#define ERROR_HSCRYPTLIB_FILEFENC_FWRITE				ERROR_HSCRYPTLIB_BASECODE + 0x17
#define ERROR_HSCRYPTLIB_FILEFENC_ENCMSG				ERROR_HSCRYPTLIB_BASECODE + 0x18
	
#define ERROR_HSCRYPTLIB_ENCFFILE_INVALIDPARAM			ERROR_HSCRYPTLIB_BASECODE + 0x20
#define ERROR_HSCRYPTLIB_ENCFFILE_FREAD					ERROR_HSCRYPTLIB_BASECODE + 0x21
#define ERROR_HSCRYPTLIB_ENCFFILE_GETENCMSG				ERROR_HSCRYPTLIB_BASECODE + 0x22
	
#define ERROR_HSCRYPTLIB_FILEFDECBLK_INVALIDPARAM		ERROR_HSCRYPTLIB_BASECODE + 0x30
#define ERROR_HSCRYPTLIB_FILEFDECBLK_FOPEN				ERROR_HSCRYPTLIB_BASECODE + 0x31
#define ERROR_HSCRYPTLIB_FILEFDECBLK_GETFILELEN			ERROR_HSCRYPTLIB_BASECODE + 0x32
#define ERROR_HSCRYPTLIB_FILEFDECBLK_FILESEEK			ERROR_HSCRYPTLIB_BASECODE + 0x33
#define ERROR_HSCRYPTLIB_FILEFDECBLK_INITCRYPT			ERROR_HSCRYPTLIB_BASECODE + 0x34
#define ERROR_HSCRYPTLIB_FILEFDECBLK_FREAD				ERROR_HSCRYPTLIB_BASECODE + 0x35
#define ERROR_HSCRYPTLIB_FILEFDECBLK_FWRITE				ERROR_HSCRYPTLIB_BASECODE + 0x36
	
#define ERROR_HSCRYPTLIB_ISFILEENC_INVALIDPARAM			ERROR_HSCRYPTLIB_BASECODE + 0x40
#define ERROR_HSCRYPTLIB_ISFILEENC_FSEEK				ERROR_HSCRYPTLIB_BASECODE + 0x41
#define ERROR_HSCRYPTLIB_ISFILEENC_FREAD				ERROR_HSCRYPTLIB_BASECODE + 0x42
#define ERROR_HSCRYPTLIB_ISFILEENC_GETDECMSG			ERROR_HSCRYPTLIB_BASECODE + 0x43
	
	
// FUNCTION DEFINE
/*************************************************************************************
	�Լ��� : _HsCrypt_InitCrypt
	��  �� : ��/��ȣȭ�� ���� �ʱ�ȭ ��� ����
	���ڰ� : PHSCRYPT_KEYINFO pHsKeyInfo
	��ȯ�� : ERROR_SUCCESS, ERROR_HSCRYPTLIB_INITCRYPT_INVALIDPARAM
	��  ¥ : 2005. 7. 27
	�۾��� : ������
*************************************************************************************/
DWORD __stdcall _HsCrypt_InitCrypt ( IN OUT PHSCRYPT_KEYINFO pHsKeyInfo  );

/*************************************************************************************
	�Լ��� : _AntiCpCnt_GetEncMsg
	��  �� : �Էµ� ���۸� ��ȣȭ�Ͽ� ���۷� ����Ѵ�.
	���ڰ� : pbyInput, nInLength, pbyOutput, pAesEncKey, pbyOutput, nOutLength
	��ȯ�� : ERROR_HSCRYPTLIB_GETENCMSG_INVALIDPARAM, ERROR_SUCCESS
	��  ¥ : 2005. 7. 21
	�۾��� : ������
*************************************************************************************/
DWORD __stdcall _HsCrypt_GetEncMsg ( IN PBYTE pbyInput,				// [in] ��ȣȭ�� ����			
									 IN UINT nInLength,				// [in] ��ȣȭ�� ������
									 IN PBYTE pAesEncKey,			// [in] ��ȣȭŰ
									 OUT PBYTE pbyOutput,			// [out] ��ȣȭ�� ����
									 IN UINT nOutLength);			// [in] ��ȣȭ�� ������ 

/*************************************************************************************
	�Լ��� : _HsCrypt_GetDecMsg
	��  �� : �Էµ� ���۸� ��ȣȭ�Ͽ� ���۷� ����Ѵ�.
	���ڰ� : pbyInput, nInLength, pAesDecKey, pbyOutput, nOutLength
	��ȯ�� : ERROR_SUCCESS, ERROR_HSCRYPTLIB_GETDECMSG_INVALIDPARAM
	��  ¥ : 2005. 7. 21
	�۾��� : ������
*************************************************************************************/
DWORD __stdcall _HsCrypt_GetDecMsg ( IN PBYTE pbyInput,					// [in] ��ȣȭ�� ����			
									 IN UINT nInLength,					// [in] ��ȣȭ�� ������
									 IN PBYTE pAesDecKey,				// [in] ��ȣȭ Ű
									 OUT PBYTE pbyOutput,				// [out] ��ȣȭ�� ����
									 IN UINT nOutLength );				// [in] ��ȣȭ�� ������

/*************************************************************************************
	�Լ��� : _HsCrypt_FRead
	��  �� : ���ϱ���ü �����͸� �̿��Ͽ� ���ϴ� ���� ��ȣȭ�� ���۷� ���
	���ڰ� : lpOutBuffer, dwDecryptSize, pInputStream, pAesDecKey
	��ȯ�� : ERROR_SUCCESS, ERROR_HSCRYPTLIB_FREAD_INVALIDPARAM,
	         ERROR_HSCRYPTLIB_FREAD_DECRYPT_RANGE, ERROR_HSCRYPTLIB_FREAD_DECRYPT_FREAD, 
			 ERROR_HSCRYPTLIB_FREAD_DECRYPT_GETDECMSG, ERROR_HSCRYPTLIB_EXCEPTION
	��  ¥ : 2005. 7. 27
	�۾��� : ������
*************************************************************************************/
DWORD __stdcall _HsCrypt_FRead ( OUT LPVOID lpOutBuffer,			// [out] ��ȣȭ�� ����
								 IN DWORD dwDecryptSize,			// [in] ��ȣȭ(����) ������
								 IN FILE *pInputStream,				// [in] ���� ����������.
								 IN PBYTE pAesDecKey,				// [in] ��ȣȭ Ű 
								 OUT PDWORD pdwReadLen);			// [in] ��ȣȭ�� ������


/*************************************************************************************
	�Լ��� : _HsCrypt_FileFEnc
	��  �� : ���� ��ȣȭ
	���ڰ� : lpszInputFile, lpszOutputFile
	��ȯ�� : ERROR_SUCCESS, ERROR_HSCRYPTLIB_FILEFENC_INVALIDPARAM,
	         ERROR_HSCRYPTLIB_FILEFENC_FOPEN, ERROR_HSCRYPTLIB_FILEFENC_GETFILELEN, 
			 ERROR_HSCRYPTLIB_FILEFENC_SIZEZERO, ERROR_HSCRYPTLIB_FILEFENC_INITCRYPT, 
			 ERROR_HSCRYPTLIB_FILEFENC_COPYFILE, ERROR_HSCRYPTLIB_FILEFENC_ENCFILE,
			 ERROR_HSCRYPTLIB_FILEFENC_FWRITE, ERROR_HSCRYPTLIB_FILEFENC_ENCMSG,
	��  ¥ : 2005. 7. 21
	�۾��� : ������
*************************************************************************************/
DWORD __stdcall _HsCrypt_FileFEnc ( IN LPCTSTR lpszInputFile,				// [in] ��ȣȭ�� ���ϰ��
									IN PBYTE pbyInitKey,					// [in] �Ϻ�ȣȭ Ű 
								    OUT LPTSTR lpszOutputFile );			// [out] ������ ����� ���ϰ�� 

#if defined(__cplusplus)
}
#endif //(__cplusplus)

#endif //_HSCRYPTLIB_H