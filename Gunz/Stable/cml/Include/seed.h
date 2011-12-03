/***************************************************************************
* Copyright (c) 2000-2004, Future Systems, Inc. / Seoul, Republic of Korea *
* All Rights Reserved.                                                     *
*                                                                          *
* This document contains proprietary and confidential information.  No     *
* parts of this document or the computer program it embodies may be in     *
* any way copied, duplicated, reproduced, translated into a different      *
* programming language, or distributed to any person, company, or          *
* corporation without the prior written consent of Future Systems, Inc.    *
*                              Hyo Sun Hwang                               *
*                372-2 YangJae B/D 6th Floor, Seoul, Korea                 *
*                           +82-2-578-0581 (552)                           *
***************************************************************************/

/*--------------------- [ Version/Command in detais] ---------------------*\
Description : seed.h
			(head file) head file for seed.c : Block Cipher SEED

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

#ifndef _SEED_H
#define _SEED_H

/*************** Header files *********************************************/
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <windows.h>

/*************** Assertions ***********************************************/
////////	Define the Endianness	////////


#undef BIG_ENDIAN
#undef LITTLE_ENDIAN

#if defined(USER_BIG_ENDIAN)
	#define BIG_ENDIAN
#elif defined(USER_LITTLE_ENDIAN)
	#define LITTLE_ENDIAN
#else
	#if 0
		#define BIG_ENDIAN		//	Big-Endian machine with pointer casting
	#elif defined(_MSC_VER)
		#define LITTLE_ENDIAN	//	Little-Endian machine with pointer casting
	#else
		#error
	#endif
#endif


/*************** Macros ***************************************************/
////////	rotate by using shift operations	////////
#if defined(_MSC_VER)
	#define ROTL_DWORD(x, n) _lrotl((x), (n))
	#define ROTR_DWORD(x, n) _lrotr((x), (n))
#else
	#define ROTL_DWORD(x, n) ( (DWORD)((x) << (n)) | (DWORD)((x) >> (32-(n))) )
	#define ROTR_DWORD(x, n) ( (DWORD)((x) >> (n)) | (DWORD)((x) << (32-(n))) )
#endif

////////	reverse the byte order of DWORD(DWORD:4-bytes integer) and WORD.
#define ENDIAN_REVERSE_DWORD(dwS)	( (ROTL_DWORD((dwS),  8) & 0x00ff00ff)	\
									 | (ROTL_DWORD((dwS), 24) & 0xff00ff00) )

////////	move DWORD type to BYTE type and BYTE type to DWORD type
#if defined(BIG_ENDIAN)		////	Big-Endian machine
	#define BIG_B2D(B, D)		D = *(DWORD *)(B)
	#define BIG_D2B(D, B)		*(DWORD *)(B) = (DWORD)(D)
	#define LITTLE_B2D(B, D)	D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
#elif defined(LITTLE_ENDIAN)	////	Little-Endian machine
	#define BIG_B2D(B, D)		D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define BIG_D2B(D, B)		*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
	#define LITTLE_B2D(B, D)	D = *(DWORD *)(B)
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = (DWORD)(D)
#else
//	#error ERROR : Invalid DataChangeType

	#define BIG_B2D(B, D)		D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define BIG_D2B(D, B)		*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
	#define LITTLE_B2D(B, D)	D = *(DWORD *)(B)
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = (DWORD)(D)

#endif

/*************** Definitions / Macros *************************************/
////	���� �Ʒ��� 4�� ����� �����Ѵ�.
#define AI_ECB					1
#define AI_CBC					2
#define AI_OFB					3
#define AI_CFB					4
////	���� �Ʒ��� �� padding�� �����Ѵ�.
#define AI_NO_PADDING			1	//	Padding ����(�Է��� 16����Ʈ�� ���)
#define AI_PKCS_PADDING			2	//	padding�Ǵ� ����Ʈ ���� padding

////	SEED�� ���õ� �����
#define SEED_BLOCK_LEN			16		//	in BYTEs
#define SEED_USER_KEY_LEN		16		//	in BYTEs
#define SEED_NO_ROUNDS			16
#define SEED_NO_ROUNDKEY		(2*SEED_NO_ROUNDS)	//	in DWORDs

/*************** New Data Types *******************************************/
////////	Determine data types depand on the processor and compiler.

/*
#define BOOL	int					//	1-bit data type
#define BYTE	unsigned char		//	unsigned 1-byte data type
#define WORD	unsigned short int	//	unsigned 2-bytes data type
#define DWORD	unsigned int		//	unsigned 4-bytes data type
*/
#define RET_VAL		DWORD			//	return values

////	SEED..
typedef struct{
	DWORD		ModeID;						//	ECB or CBC
	DWORD		PadType;					//	��Ͼ�ȣ�� Padding type
	BYTE		IV[SEED_BLOCK_LEN];			//	Initial Vector
	BYTE		ChainVar[SEED_BLOCK_LEN];	//	Chaining Variable
	BYTE		Buffer[SEED_BLOCK_LEN];		//	Buffer for unfilled block
	DWORD		BufLen; 					//	Buffer�� ��ȿ ����Ʈ ��
	DWORD		RoundKey[SEED_NO_ROUNDKEY];	//	���� Ű�� DWORD ��
} SEED_ALG_INFO;

/*************** Constant (Error Code) ************************************/
////	Error Code - �����ϰ�, ������ ����ؾ� ��.
#define CTR_SUCCESS					0
#define CTR_FATAL_ERROR				0x1001
#define CTR_INVALID_USERKEYLEN		0x1002	//	���Ű�� ���̰� ��������.
#define CTR_PAD_CHECK_ERROR			0x1003	//	
#define CTR_DATA_LEN_ERROR			0x1004	//	���� ���̰� ��������.
#define CTR_CIPHER_LEN_ERROR		0x1005	//	��ȣ���� ����� ����� �ƴ�.

/*************** Prototypes ***********************************************/
////	����Ÿ Ÿ�� SEED_ALG_INFO�� mode, padding ���� �� IV ���� �ʱ�ȭ�Ѵ�.
void	SEED_SetAlgInfo(
		DWORD			ModeID,
		DWORD			PadType,
		BYTE			*IV,
		SEED_ALG_INFO	*AlgInfo);

////	�Էµ� SEED_USER_KEY_LEN����Ʈ�� ���Ű�� ���� Ű ����
RET_VAL SEED_KeySchedule(
		BYTE			*UserKey,		//	����� ���Ű�� �Է���.
		DWORD			UserKeyLen,
		SEED_ALG_INFO	*AlgInfo);		//	�Ϻ�ȣ�� Round Key�� �����.

////	Init/Update/Final ������ ��ȣȭ.
RET_VAL	SEED_EncInit(
		SEED_ALG_INFO	*AlgInfo);
RET_VAL	SEED_EncUpdate(
		SEED_ALG_INFO	*AlgInfo,
		BYTE			*PlainTxt,		//	���� �Էµ�.
		DWORD			PlainTxtLen,
		BYTE			*CipherTxt, 	//	��ȣ���� ��µ�.
		DWORD			*CipherTxtLen);
RET_VAL	SEED_EncFinal(
		SEED_ALG_INFO	*AlgInfo,
		BYTE			*CipherTxt, 	//	��ȣ���� ��µ�.
		DWORD			*CipherTxtLen);

////	Init/Update/Final ������ ��ȣȭ.
RET_VAL	SEED_DecInit(
		SEED_ALG_INFO	*AlgInfo);
RET_VAL	SEED_DecUpdate(
		SEED_ALG_INFO	*AlgInfo,
		BYTE			*CipherTxt,		//	��ȣ���� �Էµ�.
		DWORD			CipherTxtLen,
		BYTE			*PlainTxt,		//	��ȣ���� ��µ�.
		DWORD			*PlainTxtLen);
RET_VAL	SEED_DecFinal(
		SEED_ALG_INFO	*AlgInfo,
		BYTE			*PlainTxt,		//	��ȣ���� ��µ�.
		DWORD			*PlainTxtLen);

/*************** END OF FILE **********************************************/
#endif	//	_SEED_H
