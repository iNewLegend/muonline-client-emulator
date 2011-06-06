#include "windows.h"
#include "winbase.h"
#include "iostream"
#include "SimpleModulus.h"

DWORD CSimpleModulus::s_dwSaveLoadXOR[SIZE_ENCRYPTION_KEY]=
{
0x3F08A79B, 0xE25CC287, 0x93D27AB9, 0x20DEA7BF,
/*0x837a9bc7, 0x1ffea89b, 0xe836579c, 0xd89a0924,
0x9d8a73cf, 0x4dea98ce, 0x4dc9ef0f, 0x2bac890a,
0xf32afe54, 0xd8902a1e, 0x3fdef8f9, 0x5a827990,*/};

CSimpleModulus::CSimpleModulus()	// Completed
{ 
	this->Init(); 
}

CSimpleModulus::~CSimpleModulus()	// Completed
{
	return;
}

void CSimpleModulus::Init()	// Completed
{
	memset(this->m_dwEncryptionKey, 0, sizeof(this->m_dwEncryptionKey));
	memset(this->m_dwModulus, 0, sizeof(this->m_dwModulus));
	memset(this->m_dwDecryptionKey, 0, sizeof(this->m_dwDecryptionKey));
}

int CSimpleModulus::Encrypt(void * lpDest, const void * lpSource, int iSize)	// Emulated - Completed
{
	int iTempSize = iSize;
	int iTempSize2;
	int iOriSize ;
	unsigned char * lpTempDest = (LPBYTE)lpDest;
	unsigned char * lpTempSource = (LPBYTE)lpSource;
	
	int iDec = ((iSize+SIZE_ENCRYPTION_BLOCK-1)/SIZE_ENCRYPTION_BLOCK);
	iSize = iDec*SIZE_ENCRYPTED_BLOCK;

	if ( lpDest != NULL )
	{
		iOriSize = iTempSize;
		for ( int i=0; i<iTempSize; i+=SIZE_ENCRYPTION_BLOCK,iOriSize-=SIZE_ENCRYPTION_BLOCK,lpTempDest+=SIZE_ENCRYPTED_BLOCK)
		{
			iTempSize2 = iOriSize;
			if ( iOriSize >= SIZE_ENCRYPTION_BLOCK )
				iTempSize2 = SIZE_ENCRYPTION_BLOCK;

			this->EncryptBlock(lpTempDest, lpTempSource+i, iTempSize2);
		}
	}
	return iSize;
}

int CSimpleModulus::Decrypt(void * lpDest, const void * lpSource, int iSize)
{
	if ( lpDest == NULL)
	{
		return iSize*SIZE_ENCRYPTION_BLOCK/SIZE_ENCRYPTED_BLOCK;
	}

	LPBYTE lpTempDest = (LPBYTE)lpDest;
	LPBYTE lpTempSrc = (LPBYTE)lpSource;

	int iResult = 0;
	int iDecLen = 0;

	if ( iSize > 0 )
	{
		while ( iDecLen < iSize )
		{
			int iTempResult = this->DecryptBlock(lpTempDest, lpTempSrc);

			if ( iResult < 0 )
			{
				return iResult;
			}

			iResult += iTempResult;
			iDecLen += SIZE_ENCRYPTED_BLOCK;
			lpTempSrc += SIZE_ENCRYPTED_BLOCK;
			lpTempDest += SIZE_ENCRYPTION_BLOCK;
		}
	}
	return iResult;
}

int CSimpleModulus::EncryptBlock(void*lpDest,const void*lpSource,int iSize)
{
	DWORD dwEncBuffer[SIZE_ENCRYPTION_KEY];
	DWORD dwEncValue=0;	// TempVar1

	LPBYTE lpEncDest = (LPBYTE)lpDest;
	LPBYTE lpEncSource = (LPBYTE)lpSource;

	memset(lpEncDest, 0, SIZE_ENCRYPTED_BLOCK);

	for ( int i=0;i<SIZE_ENCRYPTION_KEY;i++)
	{
		dwEncBuffer[i]=((this->m_dwXORKey[i]^((WORD*)lpEncSource)[i]^dwEncValue)*this->m_dwEncryptionKey[i])%this->m_dwModulus[i];
		dwEncValue=dwEncBuffer[i]&0xFFFF;
	}

	for ( int i=0;i<SIZE_ENCRYPTION_KEY-1;i++)
	{
		dwEncBuffer[i] = dwEncBuffer[i] ^ this->m_dwXORKey[i] ^ ( dwEncBuffer[i+1] & 0xFFFF );
	}

	int iBitPos = 0;

	for ( int i=0;i<SIZE_ENCRYPTION_KEY;i++)
	{
		iBitPos = this->AddBits(lpDest, iBitPos, &dwEncBuffer[i], 0, 16);
		iBitPos = this->AddBits(lpDest, iBitPos, &dwEncBuffer[i], 22, 2);
	}

	BYTE btCheckSum = 0xF8;
	
	for (int i=0;i<SIZE_ENCRYPTION_BLOCK;i++)
		btCheckSum ^= lpEncSource[i];

	((LPBYTE)&dwEncValue)[1] = btCheckSum ; 
	((LPBYTE)&dwEncValue)[0] = btCheckSum ^ iSize ^ 0x3D; 

	return this->AddBits(lpDest, iBitPos, &dwEncValue, 0, 16);
}

int CSimpleModulus::DecryptBlock(void*lpDest,const void*lpSource)
{
	memset(lpDest, 0, SIZE_ENCRYPTION_BLOCK);
	DWORD dwDecBuffer[SIZE_ENCRYPTION_KEY]= {0};
	int iBitPosition = 0;

	LPBYTE lpDecDest = (LPBYTE)lpDest;
	LPBYTE lpDecSource = (LPBYTE)lpSource;

	for(int i=0;i<SIZE_ENCRYPTION_KEY;i++)
	{
		this->AddBits(&dwDecBuffer[i], 0, lpDecSource, iBitPosition, 16);
		iBitPosition += 16;
		this->AddBits(&dwDecBuffer[i], 22, lpDecSource, iBitPosition, 2);
		iBitPosition += 2;
	}

	for(int i=SIZE_ENCRYPTION_KEY-2;i>=0;i--)
	{
		dwDecBuffer[i] = dwDecBuffer[i] ^ this->m_dwXORKey[i] ^ (dwDecBuffer[i+1]&0xFFFF);
	}

	DWORD Temp=0,Temp1;

	for(int i=0;i<SIZE_ENCRYPTION_KEY;i++)
	{
		Temp1 = (( this->m_dwDecryptionKey[i] * (dwDecBuffer[i])) % ( this->m_dwModulus[i])) ^ this->m_dwXORKey[i]^Temp;
		Temp = dwDecBuffer[i]&0xFFFF;
		((WORD*)lpDecDest)[i] = (Temp1);
	}

	dwDecBuffer[0]=0;
	this->AddBits(&dwDecBuffer[0], 0, lpDecSource, iBitPosition, 16);
	((LPBYTE)dwDecBuffer)[0]=((LPBYTE)dwDecBuffer)[1]^ ((LPBYTE)dwDecBuffer)[0]^0x3D;

	BYTE btCheckSum=0xF8;

	for(int i=0;i<SIZE_ENCRYPTION_BLOCK;i++)
		btCheckSum ^= lpDecDest[i];

	if( btCheckSum != ((LPBYTE)dwDecBuffer)[1] )
		return -1;
	
	return ((LPBYTE)dwDecBuffer)[0];
}

int CSimpleModulus::AddBits(void*lpDest,int iDestBitPos,const void*lpSource,int iBitSourcePos,int iBitLen)	// Completed Emulated
{
	// Get Buffer Lens
	int iSourceBufferBitLen = iBitLen + iBitSourcePos ;
	int iTempBufferLen = this->GetByteOfBit(iSourceBufferBitLen-1);
	iTempBufferLen += 1 - this->GetByteOfBit(iBitSourcePos);

	// Copy the Source Buffer
	unsigned char * pTempBuffer = new unsigned char[iTempBufferLen+1];
	memset(pTempBuffer, 0, iTempBufferLen+1);
	memcpy(pTempBuffer, (unsigned char *)lpSource+ this->GetByteOfBit(iBitSourcePos), iTempBufferLen);

	// Save the Last ibt if exist
	if ( (iSourceBufferBitLen%8 ) != 0 )
	{
		pTempBuffer[iTempBufferLen - 1] &= 255 << (8 - (iSourceBufferBitLen%8));
	}

	// Get the Values to Shift
	int iShiftLeft = (iBitSourcePos%8);
	int iShiftRight = (iDestBitPos%8);

	// Shift the Values to Add the right space of the desired bits
	this->Shift(pTempBuffer, iTempBufferLen, -iShiftLeft);
	this->Shift(pTempBuffer, iTempBufferLen+1, iShiftRight);

	// Copy the the bits of Source to the Dest
	int iNewTempBufferLen = (( iShiftRight <= iShiftLeft )?0:1) + iTempBufferLen;
	unsigned char * TempDist = (unsigned char *)lpDest + this->GetByteOfBit(iDestBitPos);

	for ( int i=0;i<iNewTempBufferLen;i++)
	{
		TempDist[i] |= pTempBuffer[i];
	}

	// Delete the temp Buffer
	delete [] pTempBuffer;

	// Return the number of bits of the new Dest Buffer
	return iDestBitPos + iBitLen;
}

void CSimpleModulus::Shift(void*lpBuff,int iSize,int ShiftLen) // Need To be Checked
{
	unsigned char * TempBuff = (unsigned char *)lpBuff;

	// Case no Shift Len
	if ( ShiftLen != 0  )	
	{
		// Shift Right
		if ( ShiftLen > 0 )
		{
			if ( (iSize -1 ) > 0 )
			{
				for ( int i=(iSize-1); i>0 ; i--)
				{
					TempBuff[i] = (TempBuff[i-1]<<((8 - ShiftLen))) | (TempBuff[i]>>ShiftLen );
				}
			}

			TempBuff[0] >>=  ShiftLen;
		}
		else	// Shift Left
		{
			ShiftLen = - ShiftLen;

			if ( (iSize-1) > 0 )
			{
				for ( int i=0; i<(iSize-1) ;i++)
				{
					TempBuff[i] = (TempBuff[i+1]>>((8 - ShiftLen))) | (TempBuff[i]<<ShiftLen );
				}
			}

			TempBuff[iSize-1] <<= ShiftLen;
		}
	}
}

int CSimpleModulus::GetByteOfBit(int btByte)
{
	return btByte >> 3;
}

BOOL CSimpleModulus::SaveAllKey(LPSTR lpszFileName )
{
	return this->SaveKey(lpszFileName, 4370, TRUE, TRUE, TRUE, TRUE);
}

BOOL CSimpleModulus::LoadAllKey(LPSTR lpszFileName)
{
	return this->LoadKey(lpszFileName, 4370, TRUE, TRUE, TRUE, TRUE);
}

BOOL CSimpleModulus::SaveEncryptionKey(LPSTR lpszFileName)
{
	return this->SaveKey(lpszFileName, 4370, TRUE, TRUE, FALSE, TRUE);
}


BOOL CSimpleModulus::LoadEncryptionKey(LPSTR lpszFileName)
{
	return this->LoadKey(lpszFileName, 4370, TRUE, TRUE, FALSE ,TRUE);
}

BOOL CSimpleModulus::SaveDecryptionKey(LPSTR lpszFileName)
{
	return this->SaveKey(lpszFileName, 4370, TRUE, FALSE, TRUE, TRUE);
}


BOOL CSimpleModulus::LoadDecryptionKey(LPSTR lpszFileName)
{
	return this->LoadKey(lpszFileName, 4370, TRUE, FALSE ,TRUE ,TRUE);
}


BOOL CSimpleModulus::SaveKey(LPSTR lpszFileName, WORD wFileHeader, BOOL bSaveModulus, BOOL bSaveEncKey, BOOL bSaveDecKey, BOOL bSaveXORKey)
{
	// MAde by Deathway
	ENCDEC_FILEHEADER HeaderBuffer;
	int iSize;
	DWORD XORTable[SIZE_ENCRYPTION_KEY];
	HANDLE hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0x80, NULL);

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	HeaderBuffer.sFileHeader = wFileHeader;
	HeaderBuffer.dwSize = ( bSaveModulus + bSaveEncKey + bSaveDecKey + bSaveXORKey ) * sizeof(XORTable) + sizeof(ENCDEC_FILEHEADER);

	WriteFile( hFile, &HeaderBuffer, sizeof(ENCDEC_FILEHEADER), (ULONG*)&iSize, NULL);

	if ( bSaveModulus != FALSE )
	{
		for ( int n = 0 ; n < SIZE_ENCRYPTION_KEY ; n++ )
		{
			XORTable[n] = this->m_dwModulus[n] ^ s_dwSaveLoadXOR[n];
		}
		WriteFile(hFile, &XORTable, sizeof(XORTable), (ULONG*)&iSize, NULL);
	}

	if ( bSaveEncKey != FALSE )
	{
		for ( int n = 0 ; n < SIZE_ENCRYPTION_KEY ; n++ )
		{
			XORTable[n] = this->m_dwEncryptionKey[n] ^ s_dwSaveLoadXOR[n];
		}
		WriteFile(hFile, &XORTable, sizeof(XORTable), (ULONG*)&iSize, NULL);
	}

	if ( bSaveDecKey != FALSE )
	{
		for ( int n = 0 ; n < SIZE_ENCRYPTION_KEY ; n++ )
		{
			XORTable[n] = this->m_dwDecryptionKey[n] ^ s_dwSaveLoadXOR[n];
		}
		WriteFile(hFile, &XORTable, sizeof(XORTable), (ULONG*)&iSize, NULL);
	}

	if ( bSaveXORKey != FALSE )
	{
		for ( int n = 0 ; n < SIZE_ENCRYPTION_KEY ; n++ )
		{
			XORTable[n] = this->m_dwXORKey[n] ^ s_dwSaveLoadXOR[n];
		}
		WriteFile(hFile, &XORTable, sizeof(XORTable), (ULONG*)&iSize, NULL);
	}

	CloseHandle(hFile);

	return TRUE;
}

BOOL CSimpleModulus::LoadKey(LPSTR lpszFileName, WORD wFileHeader, BOOL bLoadModulus, BOOL bLoadEncKey, BOOL bLoadDecKey, BOOL bLoadXORKey)
{
	ENCDEC_FILEHEADER HeaderBuffer;
	int iSize;
	DWORD XORTable[SIZE_ENCRYPTION_KEY];
	HANDLE hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0x80, NULL);

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		ReadFile( hFile, &HeaderBuffer, sizeof(ENCDEC_FILEHEADER), (ULONG*)&iSize, NULL);
	}

	if ( HeaderBuffer.sFileHeader == wFileHeader  )
	{
		if ( (HeaderBuffer.dwSize) == (int)((( bLoadModulus + bLoadEncKey + bLoadDecKey + bLoadXORKey )*sizeof(XORTable))+sizeof(ENCDEC_FILEHEADER)) )
		{
			if ( bLoadModulus != FALSE )
			{
				ReadFile(hFile, XORTable, sizeof(XORTable) , (ULONG*)&iSize, NULL);
				for ( int n = 0 ; n<SIZE_ENCRYPTION_KEY;n++)
				{
					this->m_dwModulus[n] = s_dwSaveLoadXOR[n] ^ XORTable[n];
				}
			}
			if ( bLoadEncKey != FALSE )
			{
				ReadFile(hFile, XORTable, sizeof(XORTable) , (ULONG*)&iSize, NULL);
				for ( int n = 0 ; n<SIZE_ENCRYPTION_KEY;n++)
				{
					this->m_dwEncryptionKey[n] = s_dwSaveLoadXOR[n] ^ XORTable[n];
				}
			}
			if ( bLoadDecKey != FALSE )
			{
				ReadFile(hFile, XORTable, sizeof(XORTable) , (ULONG*)&iSize, NULL);
				for ( int n = 0 ; n<SIZE_ENCRYPTION_KEY;n++)
				{
					this->m_dwDecryptionKey[n] = s_dwSaveLoadXOR[n] ^ XORTable[n];
				}
			}
			if ( bLoadXORKey != FALSE )
			{
				ReadFile(hFile, XORTable,sizeof(XORTable) , (ULONG*)&iSize, NULL);
				for ( int n = 0 ; n<SIZE_ENCRYPTION_KEY;n++)
				{
					this->m_dwXORKey[n] = s_dwSaveLoadXOR[n] ^ XORTable[n];
				}
			}

			CloseHandle(hFile);
			return TRUE;
		}
	}
	else
	{
		CloseHandle(hFile);
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

static unsigned char s_byXorFilter[32]={
	0xE7,0x6D,0x3A,0x89,0xBC,0xB2,0x9F,0x73,
	0x23,0xA8,0xFE,0xB6,0x49,0x5D,0x39,0x5D,
	0x8A,0xCB,0x63,0x8D,0xEA,0x7D,0x2B,0x5F,
	0xC3,0xB1,0xE9,0x83,0x29,0x51,0xE8,0x56};


void EncXorData(unsigned char* pBuffer, int size, int begin)
{
	for (int i=begin+1;i<size;++i)
	{
		pBuffer[i]^=pBuffer[i-1]^s_byXorFilter[i%32];
	}
}

void DecXorData(unsigned char* pBuffer, int size, int begin)
{
	for (int i=size-1;i>begin;--i)
	{
		pBuffer[i]^=pBuffer[i-1]^s_byXorFilter[i%32];
	}
}


static unsigned char bBuxCode[3]={0xFC, 0xCF, 0xAB};	// Xox Key for some interesthing things :)

void BuxConvert(char* buf, int size)
{
	int n;

	for (n=0;n<size;n++)
	{
		buf[n]^=bBuxCode[n%3] ;		// Nice trick from WebZen
	}
}
