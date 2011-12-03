#ifndef _MBASEITEM_H
#define _MBASEITEM_H


// #define MAX_ITEM_COUNT					100		// �ѻ���� �ִ�� ���� �� �ִ� ������ ����
#define RENT_PERIOD_UNLIMITED			(0)			// �Ⱓ�� ������ �������̸� �ִ� ��� �ð��� 0���� ������ �ȴ�. 
#define RENT_MINUTE_PERIOD_UNLIMITED	(525600)	// Ŭ���̾�Ʈ���״� �Ⱓ�� ������ �Ⱓ�� minute������ ������. 525600�̸� ������(1��)


// ������. ����, Ŭ���̾�Ʈ ������ �θ� Ŭ����
// ������ MMatchItem
// Ŭ���̾�Ʈ�� ZMyItemNode
class MBaseItem
{
protected:
	bool				m_bIsRentItem;						///< �Ⱓ�� ���������� ����
	int					m_nRentMinutePeriodRemainder;		///< �Ⱓ�� �����Ⱓ(�д���)
	int					m_nCount;							///< ����. 
	WORD				m_wRentHourPeriod;					///< �Ⱓ�� �������ϰ�� �ִ� ��� �ð�.

public:
	MBaseItem(): m_bIsRentItem(false), m_nRentMinutePeriodRemainder(RENT_MINUTE_PERIOD_UNLIMITED), m_nCount(0), m_wRentHourPeriod(RENT_PERIOD_UNLIMITED)  { }
	virtual	~MBaseItem() {}
	
	int			GetRentMinutePeriodRemainder() const	{ return ((IsRentItem()) ? m_nRentMinutePeriodRemainder : RENT_MINUTE_PERIOD_UNLIMITED); }
	const WORD	GetRentHourPeriod() const				{ return m_wRentHourPeriod; }

	void		SetRentHourPeriod( const WORD wHour )	{ m_wRentHourPeriod = wHour; m_bIsRentItem = RENT_PERIOD_UNLIMITED != wHour; }

	bool		IsRentItem() const						{ return m_bIsRentItem; }

	void SetRentItem(int nRentMinutePeriodRemainder)	{ m_nRentMinutePeriodRemainder=nRentMinutePeriodRemainder; }
	
	int			GetItemCount() const					{ return m_nCount; }
	void		SetItemCount(int nVal)					{ m_nCount = nVal; }
};


#endif