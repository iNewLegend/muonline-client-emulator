#ifndef _MPACKET_H
#define _MPACKET_H

#include "MCommand.h"
#include "MCommandManager.h"

#define MAX_PACKET_SIZE			16384		// �ִ� ��Ŷ ũ��� 16k

#define MSG_COMMAND	1000

#define MSGID_REPLYCONNECT	10
#define MSGID_RAWCOMMAND	100		// ��ȣȭ �ȵ� Ŀ�ǵ�
#define MSGID_COMMAND		101		// ��ȣȭ�� Ŀ�ǵ�
//#define MSGID_HSHIELDCOMMAND 102	// �ٽǵ� ��ȣȭ�� Ŀ�ǵ�




class MPacketCrypter;

#pragma pack(1)

struct MPacketHeader
{
	unsigned short nMsg;
	unsigned short nSize;
	unsigned short nCheckSum;

	MPacketHeader() { nMsg=MSG_COMMAND; nSize=0; nCheckSum=0; }
	int CalcPacketSize(MPacketCrypter* pCrypter);		// ���� ��ȣȭ�Ǿ����� ��� ��Ŷ�� ũ��� �� �Լ��θ� �� �� �ִ�.
														// nSize �������� ��ȣȭ�� ���� ������� �� ����
};


struct MReplyConnectMsg : public MPacketHeader
{
	unsigned int	nHostHigh;
	unsigned int	nHostLow;
	unsigned int	nAllocHigh;
	unsigned int	nAllocLow;
	unsigned int	nTimeStamp;
};

struct MCommandMsg : public MPacketHeader
{
	char	Buffer[1];
};


#pragma pack()

// Tiny CheckSum for MCommandMsg
inline unsigned short MBuildCheckSum(MPacketHeader* pPacket, int nPacketSize)
{
	int nStartOffset = sizeof(MPacketHeader);
	BYTE* pBulk = (BYTE*)pPacket;
	//int nPacketSize = min(65535, pPacket->nSize);
	nPacketSize = min(65535, nPacketSize);

	unsigned long nCheckSum = 0;
	for (int i=nStartOffset; i<nPacketSize; i++) {
		nCheckSum += pBulk[i];
	}
	nCheckSum -= (pBulk[0]+pBulk[1]+pBulk[2]+pBulk[3]);
	unsigned short nShortCheckSum = HIWORD(nCheckSum) + LOWORD(nCheckSum);
	return nShortCheckSum;
}


#endif
