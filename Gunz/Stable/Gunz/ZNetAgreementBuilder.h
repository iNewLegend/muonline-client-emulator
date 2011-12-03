#ifndef _ZNETAGREEMENTBUILDER_H
#define _ZNETAGREEMENTBUILDER_H

#include "MUID.h"
#include "MMatchGlobal.h"
#include <list>
using namespace std;

class ZNetAgreementBuilder
{
private:
	int						m_nRequestID;
	MMatchProposalMode		m_nProposalMode;
	bool					m_bProposingNow;

	struct ZReplier
	{
		char	szCharName[MATCHOBJECT_NAME_LENGTH];
		bool	bAnswered;
		bool	bAgreement;
	};
	list<ZReplier*>			m_Replies;

	void ClearReplies();
	void Clear();
public:
	ZNetAgreementBuilder();
	virtual ~ZNetAgreementBuilder();
	
	bool Proposal(MMatchProposalMode nProposalMode, int nRequestID, char** ppReplierNames, int nReplierCount);
	void CancelProposal();

	enum _BuildReplyResult
	{
		BRR_WRONG_REPLY		= 0,		// �߸��� �亯
		BRR_NOT_REPLIED_ALL	= 1,		// ���� ��� ������� �ʾҴ�.
		BRR_ALL_AGREED		= 2,		// ��� ������.
		BRR_DISAGREED		= 3			// ������ ����� �ִ�.
	};

	ZNetAgreementBuilder::_BuildReplyResult BuildReply(const char* szReplierName, const MMatchProposalMode nProposalMode, 
													   int nRequestID, bool bAgreement);	// ��� ���ǰ� �� ������� return true
	MMatchProposalMode GetProposalMode()	{ return m_nProposalMode; }
	int GetReplierNames(char** ppReplierNames, int nMaxCount);	// Return���� ���� Replier ����
	bool GetRejecter(char* out);
};



#endif