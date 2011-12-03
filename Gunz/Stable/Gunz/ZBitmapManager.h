#pragma once

#include "Mint4R2.h"

// #��Ʈ�� �Ŵ���
// MBitmapManager�� �ֱ� ������ ������ü ���·� ���ǰ� �ֽ��ϴ�.
// �� �Ŵ����� ������ ��Ʈ�� �׷��� �ٷ�� ���� �� �ν��Ͻ��ؼ� ����մϴ�.
// ������ ����� ǥ�� ������Ʈ �۾� �߿�,
// �ʿ��� ����� �������� �׶��׶� �ε��ϰ�, �ʿ��������(ex;����/���â���� ������) ���� ��ε��ϱ� ������
// ������ �������� �Ŵ����� �ʿ������� ��������ϴ�.

template <typename KEYTYPE>
class ZBitmapManager
{
//	typedef map<KEYTYPE, MBitmap*>	MapBitmap;
//	typedef MapBitmap::iterator		ItorBitmap;

	map<KEYTYPE, MBitmap*> m_mapBitmap;

public:
	ZBitmapManager() {}
	~ZBitmapManager()
	{
		Clear();
	}


	void Clear()
	{
		for (map<KEYTYPE, MBitmap*>::iterator it=m_mapBitmap.begin(); it!=m_mapBitmap.end(); ++it)
			delete it->second;
		m_mapBitmap.clear();
	}

	// ��Ʈ���� �ؽ�ó�� �ӽ÷� �޸𸮿��� ��� ������.
	// ��Ʈ�� �뷮�� ������, �׻� �޸𸮿� �ε� ������ �ʿ�� ���� �͵��� ���� �������ִ� �뵵
	void UnloadTextureTemporarily()
	{
		for (map<KEYTYPE, MBitmap*>::iterator it=m_mapBitmap.begin(); it!=m_mapBitmap.end(); ++it)
			((MBitmapR2*)(it->second))->UnloadTextureTemporarily();
	}

	bool Add(KEYTYPE key, MBitmap* pBitmap)
	{
		map<KEYTYPE, MBitmap*>::iterator it = m_mapBitmap.find(key);
		if (it == m_mapBitmap.end()) {
			m_mapBitmap[key] = pBitmap;
			return true;
		}
		return false;
	}

	MBitmap* Get(KEYTYPE key)
	{
		map<KEYTYPE, MBitmap*>::iterator it = m_mapBitmap.find(key);
		if (it != m_mapBitmap.end())
			return it->second;
		return NULL;
	}

	void Delete(KEYTYPE key)
	{
		map<KEYTYPE, MBitmap*>::iterator it = m_mapBitmap.find(key);
		if (it != m_mapBitmap.end()) {
			delete it->second;
			m_mapBitmap.erase(it);
		}
	}
};