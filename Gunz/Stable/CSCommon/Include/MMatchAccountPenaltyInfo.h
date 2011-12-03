#pragma once

// ==============================================================================
// Admin�� ���� �������� ����(?)�� ���� Ŭ�����Դϴ�.
// ���� ���� ��ŷ ����� ���Ͽ� �������� �������� �����Դϴ�.
// ���Ŀ��� ���ľߵ��ٵ�-.,- ��� ���ľߵ���..������
//																2010-08-09 ȫ����
// ==============================================================================

// DB ���� AccountPenlatyCode�� ��ġ�Ͽ��� �Ѵ�.
enum MPenaltyCode
{
	MPC_NONE = 0,
	MPC_CONNECT_BLOCK,
	MPC_CHAT_BLOCK,
	MPC_SHOP_BLOCK,
	MPC_MAX,
};

struct MPenaltyInfo
{
	MPenaltyCode	nPenaltyCode;
	SYSTEMTIME		sysPenaltyEndDate;
};

class MMatchAccountPenaltyInfo
{
protected:
	MPenaltyInfo m_PenaltyTable[MPC_MAX];

	SYSTEMTIME GetPenaltyEndDate(int nPenaltyHour);
public:
	MMatchAccountPenaltyInfo();
	~MMatchAccountPenaltyInfo();

	void ClearPenaltyInfo(MPenaltyCode nCode);

	void SetPenaltyInfo(MPenaltyCode nCode, int nPenaltyHour);
	void SetPenaltyInfo(MPenaltyCode nCode, SYSTEMTIME sysPenaltyEndDate);

	bool IsBlock(MPenaltyCode nCode);

	const MPenaltyInfo* GetPenaltyInfo(MPenaltyCode nCode) { return &m_PenaltyTable[nCode]; }
};