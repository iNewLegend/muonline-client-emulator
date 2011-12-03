#ifndef _MMATCHDBGAMBLEITEM_H
#define _MMATCHDBGAMBLEITEM_H


#include "MMatchDBBaseGambleItem.h"


#include <map>
#include <vector>


using std::map;
using std::vector;


class MMatchGambleRewardItem
{
private :
	DWORD	m_dwGambleRewardItemID;
	DWORD	m_dwGambleItemID;
	DWORD	m_dwItemIDMale;
	DWORD	m_dwItemIDFemale;
	DWORD	m_dwItemCnt;
	WORD	m_wRentHourPeriod;
	WORD	m_wRate;
	WORD	m_wRateRange;	// �� ���� MMatchGambleItem�� AddGambleRewardItem���� �߰��ɶ� ���ǰ�, 
							//  Gamble�� �Ҷ� ��� �ȴ�.

private :
	MMatchGambleRewardItem() {}

public :
	MMatchGambleRewardItem( const DWORD dwGambleRewardItemID
		, const DWORD dwGambleItemID
		, const DWORD dwItemIDMale
		, const DWORD dwItemIDFemale
		, const DWORD dwItemCnt
		, const WORD wRentHourPeriod
		, const WORD wRate )
	{
		m_dwGambleRewardItemID	= dwGambleRewardItemID;
		m_dwGambleItemID		= dwGambleItemID;
		m_dwItemIDMale			= dwItemIDMale;
		m_dwItemIDFemale		= dwItemIDFemale;
		m_dwItemCnt				= dwItemCnt;
		m_wRentHourPeriod		= wRentHourPeriod;
		m_wRate					= wRate;
		m_wRateRange			= 0;
	}

	~MMatchGambleRewardItem() {}

	const DWORD GetGambleRewardItemID() const			{ return m_dwGambleRewardItemID; }
	const DWORD GetGambleItemID() const					{ return m_dwGambleItemID; }
	const DWORD GetItemIDMale() const					{ return m_dwItemIDMale; }
	const DWORD GetItemIDFemale() const					{ return m_dwItemIDFemale; }
	const DWORD GetItemCnt() const						{ return m_dwItemCnt; }
	const WORD	GetRentHourPeriod() const				{ return m_wRentHourPeriod; }
	const WORD	GetRate() const							{ return m_wRate; }
	const WORD	GetRateRange() const					{ return m_wRateRange; }

	void		SetRateRange( const WORD wRateRange )	{ m_wRateRange = wRateRange; }
};


class MMatchGambleItem : public MMatchDBBaseGambleItem
{
private :
	vector< MMatchGambleRewardItem* > m_RewardItemList;

	// ���� �ð��� ���������� ����.
    int		m_nStartTimeMin;
	int		m_nLifeTimeMin;
	bool	m_bIsOpened;		// �̰��� false�̸� ����� �� ����. Ŭ���̾�Ʈ�� ������ ����Ʈ���� ���ܵ�.
	bool	m_bIsNoTimeLimit;	// �̰��� true�̸� �Ⱓ�� ����.

	WORD	m_dwTotalRate;

private :
	MMatchGambleItem() {}

public :
	MMatchGambleItem( const DWORD dwGambleItemID
		, const string& strName
		, const string& strDescription
		, const int nStartTimeMin
		, const int nLifeTimeMin
		, const DWORD dwPrice
		, const bool bIsCash
		, const bool bIsOpened );
	~MMatchGambleItem();

	bool							AddGambleRewardItem( MMatchGambleRewardItem* pGRItem );

	void							Release();

	const int						GetStartTimeMin() const		{ return m_nStartTimeMin; }
	const int						GetLifeTimeMin() const		{ return m_nLifeTimeMin; }
	const int						GetEndTimeMin() const		{ return m_nStartTimeMin + m_nLifeTimeMin; }
	const bool						IsOpened() const			{ return m_bIsOpened; }
	const bool						IsNoTimeLimitItem() const	{ return m_bIsNoTimeLimit; }
	const WORD						GetTotalRate() const		{ return m_dwTotalRate; }


	const MMatchGambleRewardItem*	GetGambleRewardItemByRate( const WORD wRate ) const;
};


#endif