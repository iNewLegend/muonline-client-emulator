#pragma once

// NHN Ŀ��Ʈ ��ŷ �� ���� Ŭ����(2010. 07. 07 ȫ����)
// NHN Ŀ�ǵ� ��ŷ�̶�, UDP�� MatchServer�� Ŀ�ǵ带 ������ �ڿ� ������ ��ŷ ����(MC_NET_DISCONNECT ��..)
class ZCommandUDPHackShield
{
private:
	set<int> m_UDPDeniedCommand;
	void AddDeniedCommand(int nCommandID);
public:
	void Init();
	bool IsDeniedCommand(int nCommandID);
};