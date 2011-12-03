#ifndef _MMATCHLOCALE_H
#define _MMATCHLOCALE_H


#include "MBaseLocale.h"
#include "MNJ_DBAgentClient.h"
#include "MUID.h"

class MMatchLocale : public MBaseLocale
{
protected:
	virtual bool			OnInit();
	MNJ_DBAgentClient*		m_pDBAgentClient;		///< �Ϻ��ݸ������� ����ϴ� Ŭ���̾�Ʈ ����
	bool					m_bCheckAntiHackCrack;
public:
							MMatchLocale();
	virtual					~MMatchLocale();
	static MMatchLocale*	GetInstance();

	bool					ConnectToDBAgent();
	bool					PostLoginInfoToDBAgent(const MUID&		uidComm, 
												   const char*		szCN, 
												   const char*		szPW, 
												   bool				bFreeLoginIP, 
												   unsigned long	nChecksumPack, 
												   int				nTotalUserCount);
//	bool					SkipCheckAntiHackCrack();	///< XTrap Crack�� üũ���� ����
};

inline MMatchLocale* MGetLocale()
{
	return MMatchLocale::GetInstance();
}



#endif