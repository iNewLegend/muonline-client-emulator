#ifndef MCOMMAND_H
#define MCOMMAND_H

#include "MUID.h"

#include <vector>
#include <list>
#include <set>
#include <deque>

using namespace std;

#include "MCommandParameter.h"
#include "mempool.h"

class MCommandManager;

// Command Description Flag
#define MCDT_NOTINITIALIZED		0	///< �ʱ�ȭ�� �ȵ� ����
#define MCDT_MACHINE2MACHINE	1	///< �ӽſ��� �ӽ����� ���޵Ǵ� Ŀ�ǵ�
#define MCDT_LOCAL				2	///< ���÷� ���޵Ǵ� Ŀ�ǵ�
#define MCDT_TICKSYNC			4	///< ƽ��ũ�� �ʿ��� Ŀ�ǵ�, ����ƽ�� ����ȴ�.
#define MCDT_TICKASYNC			8	///< ƽ��ũ�� �ʿ� ���� Ŀ�ǵ�, ��ٷ� ����ȴ�.
#define MCDT_USER				16	///< ���� ���� Ŀ�ǵ�
#define MCDT_ADMIN				32	///< ��� ���� Ŀ�ǵ�
#define MCDT_PEER2PEER			64	///< Peer���� Peer�� ���޵Ǵ� Ŀ�ǵ�, MACHINE2MACHINE�ʹ� ���� �����Ѵ�.

#define MCCT_NON_ENCRYPTED		128 ///< ��ȣȭ���� �ʴ� Ŀ�ǵ�
#define MCCT_HSHIELD_ENCRYPTED	256 ///< �ٽǵ� ��ȣȭ�ϴ� Ŀ�ǵ�(���� �̱���) MCCT_NON_ENCRYPTED�� ������ �� ����.

#define MAX_COMMAND_PARAMS		255	///< Ŀ�ǵ�� �ְ� 255������ �Ķ��Ÿ�� ���� �� �ִ�.

/// Ŀ�ǵ��� �Ӽ��� ����
/// - MCommandDesc::m_nID �� ���� ID�� ���� MCommand�� MCommandDesc::m_ParamDescs�� ���ǵ� �Ķ���͸� ������ �ȴ�.
class MCommandDesc{
protected:
	int			m_nID;					///< Command ID
	char		m_szName[256];			///< Name for Parsing
	char		m_szDescription[256];	///< Description of Command
	int			m_nFlag;				///< Command Description Flag

	vector<MCommandParameterDesc*>	m_ParamDescs;	///< Parameters
public:
	/// @param nID				Ŀ�ǵ� ID
	/// @param szName			Ŀ�ǵ� �̸�
	/// @param szDescription	Ŀ�ǵ� ����
	/// @param nFlag			Ŀ�ǵ� �÷���, MCDT_NOTINITIALIZED
	MCommandDesc(int nID, const char* szName, const char* szDescription, int nFlag);
	virtual ~MCommandDesc(void);

	/// MCommandParameterDesc �߰�
	void AddParamDesc(MCommandParameterDesc* pParamDesc);

	/// �÷��� �˻�
	bool IsFlag(int nFlag) const;
	/// ���̵� ���
	int GetID(void) const { return m_nID; }
	/// �̸� ���
	const char* GetName(void) const { return m_szName; }
	/// ���� ���
	const char* GetDescription(void) const { return m_szDescription; }
	/// Parameter Description ���
	MCommandParameterDesc* GetParameterDesc(int i) const {
		if(i<0 || i>=(int)m_ParamDescs.size()) return NULL;
		return m_ParamDescs[i];
	}
	/// Parameter Description ���� ���
	int GetParameterDescCount(void) const {
		return (int)m_ParamDescs.size();
	}
	MCommandParameterType GetParameterType(int i) const
	{
		if(i<0 || i>=(int)m_ParamDescs.size()) return MPT_END;
		return m_ParamDescs[i]->GetType();
	}
	/// �ڱ⺹�� �Լ� - ���� �׽�Ʈ������ ����Ѵ�.
	MCommandDesc* Clone();
};



/// �ӽŰ� �ӽ� Ȥ�� ���ÿ� ���޵Ǵ� Ŀ���
class MCommand : public CMemPool<MCommand> 
{
public:
	MUID						m_Sender;				///< �޼����� ������ �ӽ�(Ȥ�� ������Ʈ) UID
	MUID						m_Receiver;				///< �޼����� �޴� �ӽ�(Ȥ�� ������Ʈ) UID
	const MCommandDesc*			m_pCommandDesc;			///< �ش� Ŀ�ǵ��� Description
	vector<MCommandParameter*>	m_Params;				///< �Ķ����
	unsigned char				m_nSerialNumber;		///< Ŀ�ǵ��� ���Ἲ�� üũ�ϱ� ���� �Ϸù�ȣ
	void ClearParam(int i);

protected:
	/// �ʱ�ȭ
	void Reset(void);
	/// �Ķ���� �ʱ�ȭ
	void ClearParam(void);

public:
	MCommand(void);
	MCommand(const MCommandDesc* pCommandDesc, MUID Receiver, MUID Sender);
	MCommand::MCommand(int nID, MUID Sender, MUID Receiver, MCommandManager* pCommandManager);
	virtual ~MCommand(void);

	/// MCommandDesc���� ID ����
	void SetID(const MCommandDesc* pCommandDesc);
	/// ID ����
	void MCommand::SetID(int nID, MCommandManager* pCommandManager);
	/// ID ���
	int GetID(void) const { return m_pCommandDesc->GetID(); }
	/// ���� ���
	const char* GetDescription(void){ return m_pCommandDesc->GetDescription(); }

	/// �Ķ���� �߰�
	bool AddParameter(MCommandParameter* pParam);
	/// �Ķ���� ���� ���
	int GetParameterCount(void) const;
	/// �Ķ���� ���
	MCommandParameter* GetParameter(int i) const;

	/// �ε����� �Ķ���͸� ��´�. �Ķ���� Ÿ���� ��Ȯ�ϰ� ������� ������ false�� �����Ѵ�.
	/// @brief �Ķ���� ���
	/// @param pValue	[out] �Ķ���� ��
	/// @param i		[in] �Ķ���� �ε���
	/// @param t		[in] �Ķ���� Ÿ��, ��Ȯ�� Ÿ���� �������� �Ѵ�.
	bool GetParameter(void* pValue, int i, MCommandParameterType t, int nBufferSize=-1) const;

	MUID GetSenderUID(void){ return m_Sender; }
	void SetSenderUID(const MUID &uid) { m_Sender = uid; }
	MUID GetReceiverUID(void){ return m_Receiver; }

	bool IsLocalCommand(void){ return (m_Sender==m_Receiver); }

	/// ���� ������ ���� Ŀ�ǵ� ����
	MCommand* Clone(void) const;

	/// Description�� �°Բ� �����Ǿ��°�?
	bool CheckRule(void);	

	/// Ŀ�ǵ� �޸� �� �����ͷ� ����
	/// @param pData	[out] Ŀ�ǵ� ������ ��
	/// @param nSize	[in] Ŀ�ǵ� ������ �� �ִ� ������
	/// @return			�� ������
	int GetData(char* pData, int nSize);
	/// Ŀ�ǵ� �޸� �� �����ͷκ��� ����
	/// @param pData	[in] Ŀ�ǵ� ������ ��
	/// @param pPM		[in] Ŀ�ǵ� �Ŵ���(MCommandDesc�� enum�� �� �ִ�.)
	/// @return			���� ����
	bool SetData(char* pData, MCommandManager* pCM, unsigned short nDataLen=USHRT_MAX);

	int GetSize();
	int GetSerial() { return m_nSerialNumber; }
};


class MCommandSNChecker
{
private:
	int				m_nCapacity;
	deque<int>		m_SNQueue;
	set<int>		m_SNSet;
public:
	MCommandSNChecker();
	~MCommandSNChecker();
	void InitCapacity(int nCapacity);
	bool CheckValidate(int nSerialNumber);
};



// ���μ� ���̱� ���� ��ũ��
#define NEWCMD(_ID)		(new MCommand(_ID))
#define AP(_P)			AddParameter(new _P)
#define MKCMD(_C, _ID)									{ _C = NEWCMD(_ID); }
#define MKCMD1(_C, _ID, _P0)							{ _C = NEWCMD(_ID); _C->AP(_P0); }
#define MKCMD2(_C, _ID, _P0, _P1)						{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); }
#define MKCMD3(_C, _ID, _P0, _P1, _P2)					{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); }
#define MKCMD4(_C, _ID, _P0, _P1, _P2, _P3)				{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); }
#define MKCMD5(_C, _ID, _P0, _P1, _P2, _P3, _P4)		{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); }
#define MKCMD6(_C, _ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ _C = NEWCMD(_ID); _C->AP(_P0); _C->AP(_P1); _C->AP(_P2); _C->AP(_P3); _C->AP(_P4); _C->AP(_P5); }


// Short Name
typedef MCommand				MCmd;
typedef MCommandDesc			MCmdDesc;

#endif