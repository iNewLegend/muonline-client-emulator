#ifndef MCOMMANDMANAGER_H
#define MCOMMANDMANAGER_H

#include "MCommand.h"
#include "MCommandParameter.h"
#include <map>
#include <string>
using namespace std;

//typedef vector<MCommandDesc*>	MCommandDescVector;	///< Command Description Vector(ID�� �ε����� �˻��� ���� �ϹǷ�, ���ĵ� ���͸� �̿��Ѵ�.)
typedef map<int, MCommandDesc*>	MCommandDescMap;	///< Command Description Map
typedef list<MCommand*>			MCommandList;		///< Command List
typedef map<string, string>		MCommandAliasMap;		///< Alias


/// Ŀ�ǵ� �Ŵ���
/// - Ŀ�ǵ� ��ũ���� ����
/// - Ŀ�ǵ� ť ����
class MCommandManager{
protected:
	//MCommandDescVector	m_CommandDescs;			///< Command Description List
	MCommandDescMap		m_CommandDescs;
	MCommandList		m_CommandQueue;			///< Posted Command List
	MCommandAliasMap	m_CommandAlias;			///< Aliases
protected:
	void InitializeCommandDesc(void);
	void InitializeCommandMemPool();
	void FinalizeCommandMemPool();
public:
	MCommandManager(void);
	virtual ~MCommandManager(void);

	void Initialize(void);

	int GetCommandDescCount(void) const;
	int GetCommandQueueCount(void) const;
	MCommandDesc* GetCommandDesc(int i);
	MCommandDesc* GetCommandDescByID(int nID);
	void AssignDescs(MCommandManager* pTarCM);

	/// Command Description �߰�
	void AddCommandDesc(MCommandDesc* pCD);

	/// Ŀ�ǵ� Posting
	bool Post(MCommand* pNew);

	/// ���� ���� �߰��� Ŀ�ǵ带 �о����
	MCommand* GetCommand(void);
	/// ���� ���� �߰��� Ŀ�ǵ带 CommandManager���� ������ �ʰ� �о����
	MCommand* PeekCommand(void);

	/// Command Description�� ���� ���� ��Ʈ�� ����
	void GetSyntax(char* szSyntax, const MCommandDesc* pCD);
	/// �޼����� �Ľ��ؼ� Ŀ�ǵ�� �����
	/// @param pCmd				[out] ������� Ŀ�ǵ�
	/// @param szErrMsg			[out] ���� �޼���
	/// @param nErrMsgMaxLength	[out] ���� �޼��� ũ��
	/// @param szMsg			[in]  �Ľ��� ���� �޼���
	bool ParseMessage(MCommand* pCmd, char* szErrMsg, int nErrMsgMaxLength, const char* szMsg);

	/// Alias�� �߰��Ѵ�.
	/// @param szName			[in] Alias �̸�
	/// @param szText			[in] Alias ����
	void AddAlias(string szName, string szText);
};

// Post Command Macro For Convenience
#define POSTCMD0(_ID)									{ MCommand* pC=NEWCMD(_ID); ASPostCommand(pC); }
#define POSTCMD1(_ID, _P0)								{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); ASPostCommand(pC); }
#define POSTCMD2(_ID, _P0, _P1)							{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); ASPostCommand(pC); }
#define POSTCMD3(_ID, _P0, _P1, _P2)					{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); ASPostCommand(pC); }
#define POSTCMD4(_ID, _P0, _P1, _P2, _P3)				{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); ASPostCommand(pC); }
#define POSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)			{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); ASPostCommand(pC); }
#define POSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)		{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); ASPostCommand(pC); }

#define ENPOSTCMD0(_ID)									{ MCommand* pC=NEWCMD(_ID); ASEnPostCommand(pC); }
#define ENPOSTCMD1(_ID, _P0)							{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); ASEnPostCommand(pC); }
#define ENPOSTCMD2(_ID, _P0, _P1)						{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); ASEnPostCommand(pC); }
#define ENPOSTCMD3(_ID, _P0, _P1, _P2)					{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); ASEnPostCommand(pC); }
#define ENPOSTCMD4(_ID, _P0, _P1, _P2, _P3)				{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); ASPostCommand(pC); }
#define ENPOSTCMD5(_ID, _P0, _P1, _P2, _P3, _P4)		{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); ASEnPostCommand(pC); }
#define ENPOSTCMD6(_ID, _P0, _P1, _P2, _P3, _P4, _P5)	{ MCommand* pC=NEWCMD(_ID); pC->AP(_P0); pC->AP(_P1); pC->AP(_P2); pC->AP(_P3); pC->AP(_P4); pC->AP(_P5); ASEnPostCommand(pC); }



// Command Description Add Macro For Convenience
#ifdef _DEBUG

#define BEGIN_CMD_DESC(_pCommandManager)	{ MCommandDesc* pCD4m; MCommandManager* pCM4m = _pCommandManager;
#define END_CMD_DESC()						}
#define C(_a, _b, _c, _d)					{ pCD4m = new MCommandDesc(_a, _b, _c, _d); pCM4m->AddCommandDesc(pCD4m); }
#define P(_a, _b)							{ pCD4m->AddParamDesc(new MCommandParameterDesc(_a, _b)); }
#define CA(_a, _b)							{ pCM4m->AddAlias(_a, _b); }
#define P_MINMAX(_a, _b, _min, _max)		{ MCommandParameterDesc* pNewDesc = new MCommandParameterDesc(_a, _b);  \
	pNewDesc->AddCondition(new MCommandParamConditionMinMax(_min, _max)); \
	pCD4m->AddParamDesc(pNewDesc); }

#else	// ��ũ���ǿ� ���ڿ��� ��ŷ �������� �ǹǷ� ��������� �����Ѵ�

#define BEGIN_CMD_DESC(_pCommandManager)	{ MCommandDesc* pCD4m; MCommandManager* pCM4m = _pCommandManager;
#define END_CMD_DESC()						}
#define C(_a, _b, _c, _d)					{ pCD4m = new MCommandDesc(_a, "", "", _d); pCM4m->AddCommandDesc(pCD4m); }
#define P(_a, _b)							{ pCD4m->AddParamDesc(new MCommandParameterDesc(_a, "")); }
#define CA(_a, _b)							{ pCM4m->AddAlias(_a, _b); }
#define P_MINMAX(_a, _b, _min, _max)		{ MCommandParameterDesc* pNewDesc = new MCommandParameterDesc(_a, "");  \
	pNewDesc->AddCondition(new MCommandParamConditionMinMax(_min, _max)); \
	pCD4m->AddParamDesc(pNewDesc); }

#endif


#endif