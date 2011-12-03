//// Memory Hack�� ���, �߿��������� Checksum�� �����Ѵ�. ////
#ifndef _MDATACHECKER_H
#define _MDATACHECKER_H

//#pragma once

// �Ʒ� üũ�� Ŭ������ ������� �ʽ��ϴ�.... crc32�˻�� ��ü�Ͽ����ϴ�.

#include <map>
using namespace std;


#ifndef BYTE
typedef unsigned char BYTE;
#endif

enum MEMORYFUGITIVE_TYPE {
	MT_MEMORYFUGITIVE_NONE,		// MMemoryFugitive �� �ƴ� ��� (native type)
	MT_MEMORYFUGITIVE_INT,
	MT_MEMORYFUGITIVE_FLOAT,
	//.. �ʿ��ϸ� Ÿ���� �� �߰�
};

class MDataChecker;
class MDataCheckNode {
protected:
	unsigned int	m_nID;	// ������ �ĺ���
	BYTE*			m_pData;
	unsigned int	m_nLen;
	unsigned int	m_nChecksum;
	unsigned int	m_nLastChecksum;
	MEMORYFUGITIVE_TYPE	m_memFugitiveType;

public:
	MDataCheckNode(BYTE* pData, unsigned int nLen, MEMORYFUGITIVE_TYPE memFugitiveType);
	virtual ~MDataCheckNode();

	unsigned int GetID()			{ return m_nID; }
	unsigned int GetChecksum()		{ return m_nChecksum; }
	unsigned int GetLastChecksum()	{ return m_nLastChecksum; }
	bool UpdateChecksum();	// ������Ʈ�� ���� ������ true, �ٸ��� false
	void Validate()	{ m_nLastChecksum = m_nChecksum; }

friend MDataChecker;
};
class MDataCheckMap : public map<BYTE*, MDataCheckNode*>{};


class MDataChecker {
protected:
	unsigned int	m_nTotalChecksum;
	unsigned int	m_nLastTotalChecksum;

	MDataCheckMap	m_DataCheckMap;

public:
	MDataChecker();
	virtual ~MDataChecker();

	void Clear();
	unsigned int GetChecksum()	{ return m_nTotalChecksum; }
	MDataCheckNode* FindCheck(BYTE* pData);
	MDataCheckNode* AddCheck(BYTE* pData, unsigned int nLen, MEMORYFUGITIVE_TYPE memFugitiveType=MT_MEMORYFUGITIVE_NONE);
	void RenewCheck(BYTE* pData, unsigned int nLen);
	bool UpdateChecksum();
	void BringError();
};

#endif