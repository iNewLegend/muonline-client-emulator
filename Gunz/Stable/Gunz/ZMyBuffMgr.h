#ifndef _ZMYBUFFMGR_H
#define _ZMYBUFFMGR_H

#include "ZMyBuff.h"

class ZMyBuffMgr
{
protected:
	ZMyShortBuffMap m_ShortBuffMap;

	ZMyBuffSummary	m_BuffSummary;

	bool InsertShortBuffInfo(MShortBuffInfo *pShortInfo);

public:
	ZMyBuffMgr(void);
	~ZMyBuffMgr(void);

//���������ӽ��ּ� 	void Set(MTD_CharBuffInfo* pCharBuffInfo);
	void Clear();		
};

#endif