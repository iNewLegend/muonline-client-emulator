#include "stdafx.h"
#include "MMatchItemFunction.h"


// nBountyPrice		: ���� ���� ����
// nRentHourPeriod	: �ִ� ��� �Ⱓ(�ð�����) DB���� �����´�. (ZItem.xml�� �Ⱓ������ �ٸ���.)
// nRemainHour		: ���� ��� �Ⱓ(�ð�����) DB���� �����´�. (ZItem.xml�� �Ⱓ������ �ٸ���.)
// bIsChangedToRent : ������ ������ ���� ����
// 
// �׺��� ���ؼ� ���� �������� �ۺ����� �緮�� ���� �Ⱓ ������ ZItem.xml �� �ٸ��� �ֱ� ������
// DB�� ����� ������ ������ ������ �ؾ� ������ ���̰� ������ �ʴ´�.
const int CalculateRentItemSellBounty( const int nBountyPrice
									  , const int nRentHourPeriod
									  , const int nRemainHour
									  , bool bIsChangedToRent )	
{
	// ������ �������� ����� ��� ���� �ȵȴ�.
	if( RENT_PERIOD_UNLIMITED == nRentHourPeriod )
	{
		_ASSERT( 0 );
		return 0;
	}

	int iSellPrice = 0;
	// �������� ���������� �Ȱ� �ִ� �������̸� ���԰��ݿ� ��ȭ�� �����.
	// ���� ���� ���� = �������� �Ǹ��ϴ� �Ⱓ�� �������� ���� ���� / 90 * (�Ⱓ(��¥����)) �� ���� //
	if (bIsChangedToRent)
	{
		iSellPrice = (nBountyPrice / 90) * (nRentHourPeriod / 24);
	}
	else
	{
		iSellPrice = nBountyPrice;
	}
	// ���� �ǸŰ��� = ���� ���԰��� * (���� ���� ���ð� / �ִ� ��� �ð�) * 0.5 (���� �ڸ��� ����)
	return ((int)(iSellPrice * ((double)nRemainHour / (double)nRentHourPeriod) * 0.5) / 10) * 10;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �׺� ������ - ���� ������ 500��
// �Ҹ� ������ - ���� �ϰ� 10�ٿ�Ƽ
const int CalculateCashItemToBounty(CASHITEM_TYPE nType)
{
	if( nType == CASHITEM_GAMBLE )
	{
		return 500;
	}
	else if( nType == CASHITEM_SPENDABLE ) 
	{
		return 10;
	}

	_ASSERT(0);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��Ҹ� �Ϲ� ������ : 10 * �Ⱓ����ġ + (�������� * �Ⱓ����ġ * ��������ġ)
// 
// �Ⱓ����ġ : ���� 1�ϴ� 1��, �ִ� 30��(������ 50��)
// ��������ġ : ���� 5��, �Ź�/�尩/�Ӹ� 0.5��, �׿� 1��
const int CalculateCashItemToBounty(CASHITEM_TYPE nType, int nResLevel, int nRentMinuteRemainder, MMatchItemSlotType slotType)
{
	if( nType == CASHITEM_GAMBLE || nType == CASHITEM_SPENDABLE ) {
		return CalculateCashItemToBounty(nType);
	}

	int nRentRatio;
	int nLevelRatio;
	float fSlotTypeRatio;

	if( RENT_MINUTE_PERIOD_UNLIMITED == nRentMinuteRemainder) {
		nRentRatio = 50;
	} else {
		int nRentDayPeriod = nRentMinuteRemainder / (60 * 24);
		nRentRatio = ( nRentDayPeriod > 30 ? 30 : nRentDayPeriod );
	}

	nLevelRatio  = ( nResLevel == 0 ? 1 : nResLevel );

	switch (slotType)
	{
	case MMIST_MELEE:
	case MMIST_RANGE:
		fSlotTypeRatio = 5;
		break;
	case MMIST_HEAD:
	case MMIST_HANDS:
	case MMIST_FEET:
		fSlotTypeRatio = 0.5f;
		break;
	default:
		fSlotTypeRatio = 1;
	}

	return (10 * nRentRatio) + int(nLevelRatio * nRentRatio * fSlotTypeRatio);
}
