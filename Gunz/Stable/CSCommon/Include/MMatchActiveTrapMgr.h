#pragma once

// MMatchActiveTrapMgr�� ������ ������ �� �� �ߵ����̰ų� Ȥ�� ���� �ߵ��Ǳ� ���� Ʈ������ ����� �����Ѵ�.
// (���� ���� ������ �������� ���� ���忡 �����ϴ� Ʈ������ �˷��ֱ� ���ؼ� ����صδ� ���̴�)


class MMatchActiveTrap
{
public:
	unsigned long m_nTimeThrowed;
	int m_nLifeTime;

	// Ʈ���� �����ٴ� Ŀ�ǵ�� ��� ����
	MUID m_uidOwner;
	int m_nTrapItemId;

	// ���� Ʈ���� �ߵ��Ǿ��ٴ� Ŀ�ǵ�� ������ ����
	MVector3 m_vPosActivated;
	unsigned long m_nTimeActivated;

	// �� Ʈ���� ���������� �ߵ��Ǳ� ���� �ð��뿡 ������ ������ uid�� ���� ����� ��
	vector<MUID> m_vecUidForcedEntered;

public:
	MMatchActiveTrap();
	bool IsActivated() { return m_nTimeActivated!=0; }
	void AddForcedEnteredPlayer(const MUID& uid);
};

class MMatchActiveTrapMgr
{
	typedef list<MMatchActiveTrap*>		ListTrap;
	typedef ListTrap::iterator			ItorTrap;
	list<MMatchActiveTrap*> m_listTrap;

	MMatchStage* m_pStage;

public:
	MMatchActiveTrapMgr();
	~MMatchActiveTrapMgr();

	void Create(MMatchStage* pStage);
	void Destroy();
	void Clear();

	void AddThrowedTrap(const MUID& uidOwner, int nItemId);
	void OnActivated(const MUID& uidOwner, int nItemId, const MVector3& vPos);

	void Update(unsigned long nClock);

	void RouteAllTraps(MMatchObject* pObj);
	void RouteTrapActivationForForcedEnterd(MMatchActiveTrap* pTrap);
};