#ifndef _MemPool_h
#define _MemPool_h

//#pragma once

#include "Windows.h"
#include "Winbase.h"
#include "MDebug.h"
#include "assert.h"

//////////////////////////////////////////////////////////////////////////
//
//	MemoryPool
//	* new, delete operator overload
//	* �޸� �Ҵ����
//		- ��ü���� �ϳ��� MemoryPool ����
//		- delete : ���� delete���� �ʰ� memory pool�� ����
//		- new : ���� ��� �ִ� �޸𸮰� ������ �Ҵ� else ���� ��ü�� new�� ȣ��
//
//	* ����
//		- CMemPool�� ����ϴ� ��ü�� CMemPool�� ��ӹ޴´�
//		- ����ϱ� �� InitMemPool�� ȣ���Ѵ�
//		- ����� ���̻� ������� ������ UninitMemPool�� ȣ���Ѵ�
//		- ������ Free Slot�� Hold�Ǿ� �ִ� �޸𸮸� �����ϰ� ������ ReleaseMemPool�� ȣ���Ѵ�
//
//	* ��
//
//	class CTest:public CMemPool<CTest>
//	{
//	public:
//		CTest();
//		virtual ~CTest();
//		...
//	};		
//
//	InitMemPool(CTest);
//		...
//	CTest* p = new CTest;
//		...
//	delete(p);
//		...
//	ReleaseMemPool(CTest);
//		...
//	UninitMemPool(CTest);
//		...
//
//	* ���� 
//		- new �� �� delete���� ���� �޸𸮿� ���� å���� ���� �ʴ´�(�޸� ����)
//		- �޸� ������ ���� ó���� �������� �ʴ´�
//		- new[]�� delete[]�� ������� �ʴ´�
//		- MultiThread���� ����ǵ��� ��������� �׽�Ʈ�� �غ��� �ʾ���..
//
//////////////////////////////////////////////////////////////////////////

#define InitMemPool(T)		CMemPool<T>::_InitCS()
#define UninitMemPool(T)	CMemPool<T>::_DeleteCS();
#define ReleaseMemPool(T)	CMemPool<T>::Release();

template< typename T >
class CMemPool
{
private:
	static int	m_nCapacity;

protected:
	static T*	m_list;
	T*			m_next;

	// Multi Thread���� �޸𸮸� ��ȣ�ϱ� ����
	static CRITICAL_SECTION m_csLock;

public:
	static void	_InitCS()
	{
		m_nCapacity = 0;
		InitializeCriticalSection( &m_csLock );
	}

	static void	_DeleteCS()
	{
		DeleteCriticalSection( &m_csLock );
	}
	static void	Release();

	static int	GetCapacity();

public:
	static void* operator new( size_t size_ );
	static void  operator delete( void* deadObject_, size_t size_ );
public:
	CMemPool(void)	{};
	~CMemPool(void)	{};
};

// new
template<typename T>
void* CMemPool<T>::operator new( size_t size_ )
{
	T* instance;
	EnterCriticalSection( &m_csLock );

	if( m_list != NULL )
	{
		instance	= m_list;
		m_list	= m_list->m_next;
	}
	else
	{
		instance = (T*)::operator new(size_);
		m_nCapacity++;
	}

	LeaveCriticalSection( &m_csLock );

#ifdef _DEBUG
	if(size_ != sizeof(*instance))
		assert(0);
#endif

	return instance;
}

// delete
template<typename T>
void CMemPool<T>::operator delete( void* deadObject_, size_t size_ )
{
	EnterCriticalSection( &m_csLock );

	((T*)deadObject_)->m_next	= m_list;
	m_list	= (T*)deadObject_;

	LeaveCriticalSection( &m_csLock );
}

// Release
// ���� �Ҵ�Ǿ� ���� ���� �޸� ����~~
template<typename T>
void CMemPool<T>::Release()
{
	// KeeperManager(PatchInterface)�� �������� Ŭ���̾�Ʈ�� ������ ����.
	//  2���̻��� Ŭ���̾�Ʈ�� ����ɶ� �̺κ��� �ߺ� ȣ���� �Ǿ�, 
	//  ��ȿ���� ���� m_csLock�� ũ��Ƽ�ü����� �����Ͽ� ���ܰ� �߻���.
	//  ����Ʈ�� �ش� �ּҰ� '0'�� �ƴҰ�츸 �����ϵ��� ����. - by �߱���.
	if( NULL != m_list ) 
	{
		EnterCriticalSection( &m_csLock );

		T* pInstace		= m_list;
		while( pInstace != NULL )
		{
			pInstace	= m_list->m_next;
			::operator delete( m_list );
			m_list	= pInstace;
			m_nCapacity--;
		}

		LeaveCriticalSection( &m_csLock );
	}
}

template<typename T>
int	CMemPool<T>::GetCapacity()
{
	return m_nCapacity;
}

template<typename T> CRITICAL_SECTION CMemPool<T>::m_csLock;
template<typename T> T* CMemPool<T>::m_list;
template<typename T> int CMemPool<T>::m_nCapacity = 0;


////////////////////////////////////////////////////////////////////////////
// ������ ��� ���ϴ�~
////////////////////////////////////////////////////////////////////////////

template < typename T >
class CMemPoolSm
{
protected:
	static T*	m_list;
	T*			m_next;

public:
	static void* operator new( size_t size_ );
	static void  operator delete( void* deadObject_, size_t size_ );

	static void Release();

public:
	CMemPoolSm(void) {};
	~CMemPoolSm(void) {};
};


template<typename T>
void* CMemPoolSm<T>::operator new( size_t size_ )
{
	T* instance;

	if( m_list != NULL ) {
		instance = m_list;
		m_list = m_list->m_next;
//		mlog("�޸� ����\n");
	} else {
		instance = (T*)::operator new(size_);
//		mlog("�޸� �Ҵ�\n");
	}

#ifdef _DEBUG
	if(size_ != sizeof(*instance))
		assert(0);
#endif

	return instance;
}

template<typename T>
void CMemPoolSm<T>::operator delete( void* deadObject_, size_t size_ )
{
	((T*)deadObject_)->m_next = m_list;
	m_list	= (T*)deadObject_;
//	mlog("�޸� ���� ����\n");
}



// ���� �Ҵ�Ǿ� ���� ���� �޸� ����~~
template<typename T>
void CMemPoolSm<T>::Release()
{
	T* pInstace	= m_list;
	while( pInstace != NULL ) {
		pInstace = m_list->m_next;
		::operator delete( m_list );
		m_list = pInstace;
//		mlog("�޸� ����\n");
	}
}

/////////////////////////////////////////////////////////////////////////

template<typename T> T* CMemPoolSm<T>::m_list;

#endif//_MemPool_h