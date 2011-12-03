#pragma once

#include "MMath.h"
#include "MCRC32.h"

class MCrc32Container
{
	std::map<void*, DWORD> m_mapCrc32;
	typedef std::map<void*, DWORD>::iterator Iterator;

public:
	void Add(void* key, DWORD crc)
	{
		m_mapCrc32[key] = crc;
	}
	void Remove(void* key)
	{
		m_mapCrc32.erase(key);
	}
	DWORD Get(void* key)
	{
		Iterator it = m_mapCrc32.find(key);
		if (it!=m_mapCrc32.end())
			return it->second;
		return 0;
	}
};

extern void (*g_fpOnCrcFail)();	// crcüũ ���н�(��ŷ��) ȣ���� �Լ� ������

MCrc32Container* GetCrcContainer();


#define PTR_OFFSET 0x66D6
// ����� �ѱ� ���������� ���� �˻��غ��� �� ������ 0x15000000 ~ 0x30000000 ����
// (��RandMaskVal.exe�� ���ؼ� ���� ���� ����Ǵ� ��. define ���� �����ϸ� �Ľ� ������ ���ϴ�.)

template <typename T>
class MProtectValue
{
	// ���� �Ҵ��� ������ �ּҰ����� ����ŷ���� ��� �ּ���ü�� �����
	typedef unsigned char* MaskedPtr;
	MaskedPtr m_pValue;

private:
	MaskedPtr	ToMaskedPtr(T* ptr) const			{ return MaskedPtr(ptr) + (PtrToUlong(this)+PTR_OFFSET); }
	T*			ToNormalPtr(MaskedPtr mptr) const	{ return (T*)(mptr - (PtrToUlong(this)+PTR_OFFSET)); }

	DWORD BuildCrc()
	{
		BYTE* pData = (BYTE*)ToNormalPtr(m_pValue);
		return MCRC32::BuildCRC32(pData, sizeof(T));
	}

public:
	MProtectValue()		 { m_pValue = ToMaskedPtr(new T); }

	~MProtectValue()	 { 
		delete ToNormalPtr(m_pValue);
		::GetCrcContainer()->Remove(this);
	}

	// ���� ���۷����� ��´�
			T& Ref()		{ return *ToNormalPtr(m_pValue); }
	const	T& Ref() const	{ return *ToNormalPtr(m_pValue); }

	// �� ����
	void Set(const T& newVal)		{ Ref() = newVal; }

	// �� �����ϸ鼭 crc�˻�
	bool Set_CheckCrc(const T& newVal) {
		CheckCrc();
		Set(newVal);
		MakeCrc();
		return true;
	}
	// �� �����ϸ鼭 crc ���� (���� �� ������)
	void Set_MakeCrc(const T& newVal) {
		Set(newVal);
		MakeCrc();
	}

	// Set_CheckCrc()�� ����. �ٸ� T�� ����ü�� ��� crc�� üũ�ϸ� ����� �����ϱ� ���� �ϴ� ��ũ�� �Լ�
#define MEMBER_SET_CHECKCRC(PROTECTVALUE, MEMBER, NEWVAL) { \
		PROTECTVALUE.CheckCrc(); \
		PROTECTVALUE.Ref().MEMBER = (NEWVAL); \
		PROTECTVALUE.MakeCrc(); \
	}

	// ���Ҵ�->���縦 ���� ����ġ�� �����Ѵ�
	void ShiftHeapPos()
	{
		T* p = ToNormalPtr(m_pValue);
		m_pValue = ToMaskedPtr(new T);
		Ref() = *p;
		delete p;
	}

	void ShiftHeapPos_CheckCrc() { CheckCrc(); ShiftHeapPos(); }

	void MakeCrc()
	{
		DWORD crc = BuildCrc();
		::GetCrcContainer()->Add(this, crc);
	}

	// crc�� �˻��Ѵ�
	void CheckCrc()
	{
		if (BuildCrc() != ::GetCrcContainer()->Get(this))
			g_fpOnCrcFail();
	}

private:
	// ������ �����ε带 ���� ���ϰ����� ������ ���� ���� �Ͼ���� ��������� ǥ���ϵ��� �����ϴ� ���� ���ٰ� ������.

	MProtectValue(T val) { m_pValue = ToMaskedPtr(new T(val)); }	// ���������..ȥ�������� ���� ������
	MProtectValue<T>& operator=(int) {}	// ������� �����ڸ� ����
};

// USAGE : ��ŷ�� ������ �ϴ� ���� �� ���ø����� ���Ѵ�. 
// �� ���ø��� �ΰ��� �������� �޸����� ������ �� �ִ�.
// �ֱ������� ����ġ�� �Űܼ� ������ Ư�������� freeze�ϴ� ���� ���ƺ� �� �ִ�.
// �׷��� �� ���ø� �ȿ� �ִ� ������ �ּҸ� ������ ���� �����Ƿ� 2�������� crc32�˻縦 �� �� �ִ�.
// crc32�� ����Ϸ��� ���� �����ϱ� ���� ������crcüũ�� �ؾ��ϰ�, ���� �����ڿ� crc�� ���� �����ؾ߸� �Ѵ�.
//
// �� ���ø�Ŭ������ ���δ��� �ʿ��� ��쿡�� �ٹ�� ����� ���۽�ų �� �ִ�. ������� ZObject�� ����� �̰ɷ� ���δ���
// ���� �ٹ�� ����� �ϴ� �Լ��� ȣ������ ������ �ٹ�� ȿ���� ����. ZObject�� ��� ZMyCharacter�� NPC�� ZActor�� ��ӹ޴µ�
// NPC�� ��ŷ����� �ƴϹǷ� ���� �ٹ� ������ ���ϸ� ������ �ʿ���� ZMyCharacter�� �ٹ��� �Լ��� ȣ���ϸ� �ȴ�.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������ ���ø����� ���ſ� �ٹ��� ����ϴ� ����
// MMemoryBlock�� �շȴٰ� ��..
// MMemoryFugitive�� ����ġ�� �� �ű����� �� ��쿡 ���� ������ ����� üũ���� �����ϱ� ����ϴ�
/*


#define WRAPING_TIME 1000	//�޸� ���� �ٲ��ִ� �ð�...1�ʿ� �ѹ���...
#define NUM_OF_MEMORY 20	//�ϳ��� �޸� ���� ������ �޸� ����..

template <typename _data>
struct MMemoryBlock
{
	_data * m_pMemory[NUM_OF_MEMORY];
};

template <typename _data>
class MMemoryProxy
{
	MMemoryBlock<_data> * MB1;
	MMemoryBlock<_data> * MB2;
	MMemoryBlock<_data> * MB3;

	MMemoryBlock<_data> * MB;

	_data m_initial;

	DWORD m_dwCurrentTime;
	int m_nCorrectDataIndex;
	int m_nIndex;
	
public:
	MMemoryProxy()
	{
		Init();
	}

	MMemoryProxy(_data init)
	{
		m_initial = init;
		m_nIndex = 0;
		m_nCorrectDataIndex = 0;
		Init();
	}

	~MMemoryProxy()
	{
		Destroy();
	}

	void Init()
	{
		MB1 = new MMemoryBlock<_data>();
		MB2 = new MMemoryBlock<_data>();
		MB3 = new MMemoryBlock<_data>();
		
		for(int i=0; i<NUM_OF_MEMORY; i++)
		{
			MB1->m_pMemory[i] = new _data;
			*(MB1->m_pMemory[i]) = m_initial;

			MB2->m_pMemory[i] = new _data;
			*(MB2->m_pMemory[i]) = m_initial;

			MB3->m_pMemory[i] = new _data;
			*(MB3->m_pMemory[i]) = m_initial;
		}
		
		m_nIndex = 1;
		MB = MB1;
		m_dwCurrentTime = GetTickCount();

		SetRandomData( m_initial);
	}
	void Destroy()
	{
		for(int i=0; i<NUM_OF_MEMORY; i++)
		{
			delete MB1->m_pMemory[i];
			delete MB2->m_pMemory[i];
			delete MB3->m_pMemory[i];
		}

		delete MB1;
		delete MB2;
		delete MB3;
	}

	void SetData(const _data & data)
	{
		SetRandomData(data);
	}

	_data & GetData()
	{
		return * MB->m_pMemory[m_nCorrectDataIndex];
	}

	void SetRandomData(const _data & data)
	{
		m_nCorrectDataIndex = RandomNumber(0, NUM_OF_MEMORY-1);
		*(MB->m_pMemory[m_nCorrectDataIndex]) = data;
	}
	void SetWarpingAdd(DWORD tick)
	{
		if( tick - m_dwCurrentTime > WRAPING_TIME)
		{
			int i = RandomNumber(0, NUM_OF_MEMORY-1);
			if(m_nIndex == 1)
			{
				*(MB2->m_pMemory[i]) = *(MB1->m_pMemory[m_nCorrectDataIndex]);
				MB = MB2;
				*(MB1->m_pMemory[m_nCorrectDataIndex]) = m_initial;	//���� �ȳ����
				m_nCorrectDataIndex = i;
			}
			else if(m_nIndex == 2)
			{
				*(MB3->m_pMemory[i]) = *(MB2->m_pMemory[m_nCorrectDataIndex]);
				MB = MB3;
				*(MB2->m_pMemory[m_nCorrectDataIndex]) = m_initial;
				m_nCorrectDataIndex = i;
			}
			else
			{
				*(MB1->m_pMemory[i]) = *(MB3->m_pMemory[m_nCorrectDataIndex]);
				MB = MB1;
				*(MB3->m_pMemory[m_nCorrectDataIndex]) = m_initial;
				m_nCorrectDataIndex = i;
			}
			m_nIndex++;
			if(m_nIndex>3)
				m_nIndex = 1;

			m_dwCurrentTime = tick;
		}	
	}
};




template <typename _data>
class MMemoryFugitive
{
	_data* m_value;

public:
	MMemoryFugitive()
	{
		m_value = NULL;
	}

	MMemoryFugitive(_data data)
	{
		m_value = new _data;
		*m_value = data;
	}

	~MMemoryFugitive()
	{
		Destroy();
	}

	void Destroy()
	{
		delete m_value;
	}

	MMemoryFugitive& operator=(const _data& other)	
	{
		SetData(other);
		return *this;
	}

	MMemoryFugitive& operator+=(const _data &other)
	{
		_data sum = *m_value + other;
		SetData(sum);
		return *this;
	}
	MMemoryFugitive& operator-=(const _data &other)
	{
		_data sum = *m_value - other;
		SetData(sum);
		return *this;
	}
	MMemoryFugitive& operator*=(const _data &other)
	{
		_data sum = *m_value * other;
		SetData(sum);
		return *this;
	}

	void SetData(const _data & data)
	{
		_data* pTemp = NULL;

		if (m_value != NULL)
		{
			pTemp = m_value;
		}

		m_value = new _data;
		*m_value = data;

		if (pTemp != NULL)
		{
			delete pTemp;
		}
	}

	_data & GetData()
	{
		return *m_value ;
	}

	const _data & GetData() const
	{
		return *m_value ;
	}


	void ShiftManually()
	{
		if (m_value != NULL)
		{
			_data val_copy = GetData();
			SetData(val_copy);
		}
	}
};

*/