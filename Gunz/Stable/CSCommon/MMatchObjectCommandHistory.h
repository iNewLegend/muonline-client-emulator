#pragma once

//////////////////////////////////////////////////////////////////////////
// MMatchObjectCommandHistory
// �� MatchObject �� ���� Ŀ�ǵ带 �����ð�, �������� �����ϰ� �ִ�.
// �ʿ��� ������ �����غ��� ���� �ڷ��̴�

const int MAX_COMMAND_HISTORY_COUNT = 50;			///< ���� ���� 30��
const int MAX_HISTORY_HOLD_TIME		= 10*60*1000;	///< �ִ� �����ð� 10��

struct MOBJECTCOMMANDHISTORY {
	string			m_strName;	///< ĳ���� �̸�
	unsigned int	m_nCID;		///< ĳ���� ID
	list < pair < int, DWORD > >	m_commands;	///< command id �� �߰��� �ð�
	DWORD			m_dwLastTime;	///< ������ Ŀ�ǵ尡 �߰��� �ð�
};

class MMatchObjectCommandHistory {
public:
	~MMatchObjectCommandHistory();

	void SetCharacterInfo(MUID uid, const char* szName, unsigned int nCID );
	
	void PushCommand(MUID uid, int nCommandID, DWORD dwCurrentTime, bool* pbFloodingSuspect = NULL);
	void Update( DWORD dwCurrentTime );
	unsigned int GetObjectCount();

	MOBJECTCOMMANDHISTORY* GetCommandHistory(MUID uid);
	bool Dump(MUID uid);

private:
	typedef map< MUID, MOBJECTCOMMANDHISTORY* > UIDHISTORYMAP;

	UIDHISTORYMAP m_mapHistories;

	MOBJECTCOMMANDHISTORY* AddNew(MUID uid); 
};

inline unsigned int MMatchObjectCommandHistory::GetObjectCount() { return (unsigned int)m_mapHistories.size(); }
