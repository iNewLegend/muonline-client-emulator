#include "ClientNetWork.h"
#include "SimpleModulus.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CSimpleModulus g_SimpleModulusCS;
CSimpleModulus g_SimpleModulusSC;
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static unsigned char s_uSerial=-1;
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CClientNetWork::CClientNetWork()
{
	if(g_SimpleModulusCS.LoadEncryptionKey("Data\\Enc1.dat") == FALSE)
	{
		MessageBox(NULL, "Dec1.dat file not found", "error", MB_OK|MB_APPLMODAL);
	}
	// ----
	if(g_SimpleModulusSC.LoadDecryptionKey("Data\\Dec2.dat") == FALSE)
	{
		MessageBox(NULL, "Enc2.dat file not found", "error", MB_OK|MB_APPLMODAL);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CClientNetWork::~CClientNetWork()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CClientNetWork::SetProtocolCore(WsProtocolCore pc)
{
	this->wsProtocolCore = pc;
	// ----
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CClientNetWork::send(void* buf)
{
	static unsigned char s_XorBuf[MAX_EXSENDBUF_SIZE]	= {0};
	static unsigned char s_EncBuf[MAX_EXSENDBUF_SIZE]	= {0};
	// ----
	unsigned char* pMsg									= (unsigned char*)buf;
	// ---
	unsigned char uMsgType								= pMsg[0];
	// ----
	// # Get Size
	int len												= pMsg[1];
	// ----
	if ( uMsgType == 0xC2 || uMsgType == 0xC4 )
	{
		len = MAKE_NUMBERW(pMsg[1],pMsg[2]);
	}
	// ----
	// # Enc Xor Data
	// ----
	memcpy(s_XorBuf,buf,len);
	// ----
	pMsg = s_XorBuf;
	// ----
	EncXorData(pMsg,len,(uMsgType==0xC1||uMsgType==0xC3)?2:3);
	// ----
	// # Encrypt C3 C4
	// ---- 
	if((uMsgType == 0xC3) || (uMsgType == 0xC4))
	{
		s_uSerial++;
		// ----
		if(uMsgType == 0xC3)
		{
			pMsg[1]		= s_uSerial;
			// ----
			len			= g_SimpleModulusCS.Encrypt(& s_EncBuf[2], & pMsg[1], len-1) + 2;
			// ----
			s_EncBuf[0] = uMsgType;
			s_EncBuf[1] = len;
		}
		else
		{
			pMsg[2]		= s_uSerial;
			// ----
			len			= g_SimpleModulusCS.Encrypt(& s_EncBuf[3], & pMsg[2], len-2) + 3;
			// ----
			s_EncBuf[0] = uMsgType;
			s_EncBuf[1] = SET_NUMBERH(len);
			s_EncBuf[2] = SET_NUMBERL(len);
		}
		// ----
		pMsg = s_EncBuf;
	}
	// ----
	return CNetWork::DataSend(pMsg,len);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int CClientNetWork::ProtocolCore(const unsigned char* buf, int len)
{
	int size		= 0;
	// ----
	BYTE headcode	= 0x00;
	// ----
	if((buf[0] == 0xC1) || (buf[0] == 0xC3))		
	{
		// # Type packet handling C1 Type
		// ----
		size		= buf[1];
		headcode	= buf[2];
	}
	else if((buf[0] == 0xC2) || (buf[0] == 0xC4)) 
	{
		// # Type packet handling C2 Type
		// ----
		size		= MAKE_NUMBERW(buf[1],buf[2]);
		headcode =	 buf[3];
	}
	// ----
	if(size > 0)
	{
		if((buf[0] == 0xC1) || (buf[0] == 0xC2))
		{
			if((size > 0) && (size <= len))
			{
				this->wsProtocolCore(headcode, buf, size);
			}
		}
		else if((buf[0] == 0xC3 || buf[0] == 0xC4))
		{
			static unsigned char s_DecBuf[MAX_EXSENDBUF_SIZE]	= {0};
			// ----
			int nMsgSize										= 0;
			// ----
			if(buf[0] == 0xC3)
			{
				nMsgSize	= g_SimpleModulusSC.Decrypt(& s_DecBuf[1], & buf[2], size - 2) + 1;
				// ----
				s_DecBuf[0] = 0xC3;
				s_DecBuf[1] = nMsgSize;
				// ----
				headcode	= s_DecBuf[2];
			}
			else
			{
				nMsgSize	= g_SimpleModulusSC.Decrypt(& s_DecBuf[2], & buf[3], size - 3) + 2;
				// ----
				s_DecBuf[0] = 0xC4;
				s_DecBuf[1] = SET_NUMBERH(nMsgSize);
				s_DecBuf[2] = SET_NUMBERL(nMsgSize);
				// ----
				headcode	= s_DecBuf[3];
			}
			// ----
			if((size > 0) && (size <= len))
			{
				this->wsProtocolCore(headcode, s_DecBuf, nMsgSize);
			}
		}
	}
	// ----
	return size;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------