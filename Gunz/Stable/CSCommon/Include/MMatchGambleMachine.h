#ifndef _MMATCHGAMBLEMACHINE_H
#define _MMATCHGAMBLEMACHINE_H



#include <map>
#include <vector>


using std::map;
using std::vector;


// #define MAX_GAMBLE_RATE (1000 - 1) // 1000%�� �ִ����� ����� �Ҷ��� 0���� ������ �ϱ⶧���� 0 ~ 999������ �ȴ�.
static const DWORD MAX_GAMBLE_RATE = (1000 - 1); // 1000%�� �ִ����� ����� �Ҷ��� 0���� ������ �ϱ⶧���� 0 ~ 999������ �ȴ�.


class MMatchGambleItem;
class MMatchGambleRewardItem;


class MMatchGambleMachine
{
private :
	map< DWORD, MMatchGambleItem* >	m_GambleItemMap;
	vector< MMatchGambleItem* >		m_GambleItemVec;
	DWORD							m_dwLastUpdateTime;

private :
	const MMatchGambleRewardItem*	GetGambleRewardItem( const DWORD dwGambleItemID, const WORD wRate ) const;
									// �׺� ������ ��å������ ����� �ؾ� �Ѵ�.
	const bool						CheckGambleItemIsSelling( const int nStartTimeMin
															, const int nEndTimeMin
															, const int nCurTimeMin
															, const bool bIsNoTimeLimit  ) const;
	
public :
	MMatchGambleMachine();
	~MMatchGambleMachine(); 

									// Create�Լ��� ȣ��Ǹ� ������ ����Ʈ�� �ʱ�ȭ �ϰ� �ٽ� �����Ѵ�.
									// ����Ʈ ������ �������� ���ؼ� �ϳ��� �߰� �ϴ� �Լ��� ����.
	bool							CreateGambleItemListWithGambleRewardList( vector<MMatchGambleItem*>& vGambleItemList
																		, vector<MMatchGambleRewardItem*>& vGambleRewardItemList );
	bool							CreateGambleItemList( vector<MMatchGambleItem*>& vGambleItemList );


	void							Release();

	const DWORD						GetGambleItemSize() const { return static_cast<DWORD>(m_GambleItemVec.size()); }
	const MMatchGambleItem*			GetGambleItemByIndex( const DWORD dwIndex ) const;
	const MMatchGambleItem*			GetGambleItemByGambleItemID( const DWORD dwGambleItemID ) const;
	const MMatchGambleItem*			GetGambleItemByName( const string& strGambleItemName ) const;

	const MMatchGambleRewardItem*	Gamble( const DWORD dwGambleItemID ) const;

	// �׺� �������� ��� �ð��� �˻��ؼ� ������ �ϴ� �׺� �������� ���ڷ� �Ѿ�� ���Ϳ� ����ش�. //
	void							GetItemVectorByCheckedItemTime(vector<DWORD>& outItemIndexVec, const DWORD dwCurTime) const;
	void							GetOpenedGambleItemList( vector<DWORD>& outGItemList ) const;
	const bool						IsItTimeoverEventGambleItem( const DWORD dwGambleItemID, const DWORD dwCurTime ) const;	
	const DWORD						GetLastUpdateTime() const { return m_dwLastUpdateTime; }

	void							SetLastUpdateTime( const DWORD dwCurTime ) { m_dwLastUpdateTime = dwCurTime; }

	void							WriteGambleItemInfoToLog() const;
};



#endif