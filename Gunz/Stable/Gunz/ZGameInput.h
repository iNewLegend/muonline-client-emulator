#ifndef _ZGAMEINPUT_H
#define _ZGAMEINPUT_H

#include <list>
using namespace std;

class MEvent;
class MListener;

struct ZACTIONKEYITEM {
	ZACTIONKEYITEM(float time,bool pressed,int key) { fTime=time;bPressed=pressed;nActionKey=key; }
	float fTime;
	bool bPressed;
	int nActionKey;
};

struct ZKEYSEQUENCEITEM {
	bool bPressed;
	int nActionKey;
};

struct ZKEYSEQUENCEACTION {
	ZKEYSEQUENCEACTION(float time,int count,ZKEYSEQUENCEITEM *key) { fTotalTime=time; nKeyCount=count; pKeys=key; };

	float fTotalTime;
	int nKeyCount;
	ZKEYSEQUENCEITEM *pKeys;
};

// ����Ŭ���̾�Ʈ�� Ű���� �� ���콺, ���̽�ƽ�� �Է��� ������ Action Key �� ��ȯ�ؼ� ó���Ѵ�.
class ZGameInput
{
private:
protected:
	bool m_bCTOff;
	static ZGameInput*		m_pInstance;
	list<ZACTIONKEYITEM>	m_ActionKeyHistory;
	vector<ZKEYSEQUENCEACTION> m_SequenceActions;

protected:
	void GameCheckSequenceKeyCommand();		// �վ� �� ����Ű Ŀ�ǵ带 üũ�Ѵ�.
	bool OnDebugEvent(MEvent* pEvent);
	static bool ShowCombatInputChat();

public:
	ZGameInput();
	virtual ~ZGameInput();

	static bool OnEvent(MEvent* pEvent);
	void Update(float fElapsed);
};






#endif