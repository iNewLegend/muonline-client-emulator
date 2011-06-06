#pragma once

#include "BaseFunc.h"

// ��ʱ�࣬�ԡ��롱Ϊ��λ
class CTickOver  
{
public:
	CTickOver() { m_tTickEnd = 0; m_tTickBegin = 0; }
//	virtual ~CTickOver() {}

public:
	// ��ʼ����ʱ��
	void	Startup		(int nNow, int nDur)	{ ASSERT(nDur>0); m_tTickBegin = nNow; m_tTickEnd = nNow+nDur; }
	// ����Ƿ�ʱ��ָ��ʱ��Ƭ
	bool	IsTickOver	(int nNow)	const		{ return nNow >= m_tTickEnd; }
	// ���ڶ�ʱ����������ʱ�ӡ�ÿ�δ���ʱΪtrue��
	bool	ToNextTime	(int nNow)				{ if(!IsTickOver(nNow)) return false; int nDur=m_tTickEnd-m_tTickBegin; m_tTickBegin+=nDur; m_tTickEnd+=nDur; return true; }

public: // get ���ڴ���
	int		GetBegin()				const		{ return m_tTickBegin; }
	int		GetEnd()				const		{ return m_tTickEnd; }
	int		GetDurable()			const		{ return m_tTickEnd-m_tTickBegin; }
	int		GetDurable(int nNow)	const		{ return m_tTickEnd-nNow; }

protected:
	int		m_tTickBegin;
	int		m_tTickEnd;
};