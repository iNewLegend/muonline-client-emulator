#pragma once

/*# define SIZE_ENCRYPTION_BLOCK	( 32)
# define SIZE_ENCRYPTION_KEY	( 16)
# define SIZE_ENCRYPTED_BLOCK	( 38)*/

# define SIZE_ENCRYPTION_BLOCK	( 8)
# define SIZE_ENCRYPTION_KEY	( 4)	// SIZE_ENCRYPTION_BLOCK 의 절반
# define SIZE_ENCRYPTED_BLOCK	( 11)

class CSimpleModulus
{
public:
	CSimpleModulus();
	virtual ~CSimpleModulus();
public:
	void Init( void);
protected:
	DWORD m_dwModulus[SIZE_ENCRYPTION_KEY];
	DWORD m_dwEncryptionKey[SIZE_ENCRYPTION_KEY];
	DWORD m_dwDecryptionKey[SIZE_ENCRYPTION_KEY];
	DWORD m_dwXORKey[SIZE_ENCRYPTION_KEY];

	static DWORD s_dwSaveLoadXOR[SIZE_ENCRYPTION_KEY];
public:
	int Encrypt( void *lpTarget, const void *lpSource, int iSize);	// 큰 사이즈 모두, lpTarget 이 NULL이면 필요량 리턴
	int Decrypt( void *lpTarget, const void *lpSource, int iSize);	// 큰 사이즈 모두, lpTarget 이 NULL이면 필요량 리턴

protected:
	int EncryptBlock(void*lpDest,const void*lpSource,int iSize);	// 32 byte 를 38 byte 로
	int DecryptBlock(void*lpDest,const void*lpSource);	// 38 byte 를 32 byte 로, Size를 리턴함
	int AddBits(void*lpDest,int iDestBitPos,const void*lpSource,int iBitSourcePos,int iBitLen);
	void Shift(void*lpBuff,int iSize,int ShiftLen);
	int GetByteOfBit(int nBit);
public:
	BOOL SaveAllKey(LPSTR lpszFileName);
	BOOL LoadAllKey(LPSTR lpszFileName);
	BOOL SaveEncryptionKey(LPSTR lpszFileName);
	BOOL LoadEncryptionKey(LPSTR lpszFileName);
	BOOL SaveDecryptionKey(LPSTR lpszFileName);
	BOOL LoadDecryptionKey(LPSTR lpszFileName);
protected:
	BOOL SaveKey(LPSTR lpszFileName, WORD wFileHeader, BOOL bSaveModulus, BOOL bSaveEncKey, BOOL bSaveDecKey, BOOL bSaveXORKey);
	BOOL LoadKey(LPSTR lpszFileName, WORD wFileHeader, BOOL bLoadModulus, BOOL bLoadEncKey, BOOL bLoadDecKey, BOOL bLoadXORKey);
};


#pragma pack ( 1)
typedef struct
{
	unsigned short m_sID;
	unsigned int m_iSize;
} ChunkHeader;
struct ENCDEC_FILEHEADER
{
	short sFileHeader;
	int dwSize;
};
#pragma pack ()

# define CHUNKID_ALLKEY		( 0x1111)
# define CHUNKID_ONEKEY		( 0x1112)

void EncXorData(unsigned char* pBuffer, int size, int begin);
void DecXorData(unsigned char* pBuffer, int size, int begin);

void BuxConvert(char* buf, int size);
