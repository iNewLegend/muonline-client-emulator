/* 
	MDebugNew.h

	�� ������ include �ϸ� MNewMemories::Init() ���� 
	MNewMemories::Shutdown() ���̿� �ִ� new / delete �� �����մϴ�

	MNewMemories::Dump() �� ȣ���ϸ� ���� ������� �Ҵ�� �޸� �����
	debug���â�� memorydump.txt ��� ���Ϸ� �����մϴ�

	���� shutdown �Ҷ��� dump�� �ϹǷ� delete���� ���� ����� ����մϴ�
*/



#ifndef _DEBUGNEW_H
#define _DEBUGNEW_H

#ifdef _MTRACEMEMORY

#include <assert.h>
#include <stdio.h>

#include <map>
#include <string>
#include <vector>
#include <list>

#include "Windows.h"

using namespace std;

struct MNEWINFO;

typedef list<MNEWINFO*>					MNEWLIST;
typedef map<void*,MNEWLIST::iterator>	MNEWPOINTERMAP;

class MNewMemories {
	static int				m_nID;		// �Ҵ�Ǵ� ��ȣ
	static MNEWLIST			m_List;
	static MNEWPOINTERMAP	m_Pointers;
	static HINSTANCE		m_hModule;
	static bool	m_bInitialized;

public:
	static bool Init();
	static void Shutdown();

	static void OnNew(MNEWINFO *allocator,void* pPointer);
	static bool OnDelete(void* pPointer);
	static void Dump();
};


//-----------------
void* operator new(size_t _size);
void* operator new[]( size_t _size );
void operator delete(void* addr);
void operator delete[](void* addr);

#else	// ��Ȱ��ȭ
class MNewMemories {
public:
	static bool Init();
	static void Shutdown();
	static void Dump();
};
#endif	// of _MTRACEMEMORY

#endif