#ifndef MUID_H
#define MUID_H

#include <map>
#include <vector>
#include <crtdbg.h>

using namespace std;

/// MAIET Unique ID
struct MUID{
	unsigned long int	High;	///< High 4 Byte
	unsigned long int	Low;	///< High 4 Byte

	MUID(void) { SetZero(); }
	MUID(unsigned long int h, unsigned long int l){
		High = h;
		Low = l;
	}

	/// MUID 0���� �ʱ�ȭ
	void SetZero(void){
		High = Low = 0;
	}
	void SetInvalid(void){
		SetZero();
	}

	/*
	/// MUID 1��ŭ ����
	void Increase(void){
		if(Low==UINT_MAX){
			_ASSERT(High<UINT_MAX);
			High++;
			Low = 0;
		}
		else{
			Low++;
		}
	}
	/*/

	/// MUID�� nSize��ŭ ����
	MUID Increase(unsigned long int nSize=1){
		if(Low+nSize>UINT_MAX){
			_ASSERT(High<UINT_MAX);
			Low = nSize-(UINT_MAX-Low);
			High++;
		}
		else{
			Low+=nSize;
		}
		return *this;
	}

	/// MUID�� ��ȿ���� �˻�, 0:0�̸� Invalid
	bool IsInvalid(void) const {
		// 0�̸� Invalid
		if(High==Low && Low==0) return true;
		return false;
	}
	/// MUID�� ��ȿ���� �˻�, 0:0�̸� Invalid
	bool IsValid(void) const {
		// 0�̸� Invalid
		if(High==Low && Low==0) return false;
		return true;
	}

	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator > (const MUID& a, const MUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator >= (const MUID& a, const MUID& b){
		if(a.High>b.High) return true;
		if(a.High==b.High){
			if(a.Low>=b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator < (const MUID& a, const MUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<b.Low) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator <= (const MUID& a, const MUID& b){
		if(a.High<b.High) return true;
		if(a.High==b.High){
			if(a.Low<=b.Low) return true;
		}
		return false;
	}

	/// ���� ���۷����� �����ε�
	inline MUID& operator=(int v){
		High = 0;
		Low = v;
		return *this;
	}
	/// ���� ���۷����� �����ε�
	inline MUID& operator=(const MUID& a){
		High = a.High;
		Low = a.Low;
		return *this;
	}
	/// �� ���۷����� �����ε�
	inline friend bool operator==(const MUID& a, const MUID& b){
		if(a.High==b.High){
			if(a.Low==b.Low) return true;
		}
		return false;
	}
	/// �� ���۷����� �����ε�
	inline friend bool operator!=(const MUID& a, const MUID& b){
		if(a.Low!=b.Low) return true;
		if(a.High!=b.High) return true;
		return false;
	}
	/// ���� ���۷����� �����ε�
	inline friend MUID& operator++(MUID& a){
		a.Increase();
		return a;
	}

	/// Invalid�� UID ���
	static MUID Invalid(void);
};

/// MUID ����
struct MUIDRANGE{
	MUID	Start;
	MUID	End;

	bool IsEmpty(void){
		return (Start==End);
	}
	void Empty(void){
		SetZero();
	}
	void SetZero(void){
		Start.SetZero();
		End.SetZero();
	}
};

/// int �ΰ������� MUID ����
#define MAKEMUID(_high, _low)	MUID(_high, _low)


/// MUID Reference Map
/// - ���������� 1�� �����ϴ� ID�� �� �� �ִ� MUID ����
class MUIDRefMap : protected map<MUID, void*>{
	MUID	m_CurrentMUID;		///< ���� �߱޵� MUID
public:
	MUIDRefMap(void);
	virtual ~MUIDRefMap(void);

	/// ���۷����� MUID�ϳ��� �Ҵ��Ѵ�.
	/// @param pRef	���۷��� ������
	/// @return		�Ҵ�� MUID
	MUID Generate(void* pRef);

	/// MUID�� ���� ���۷��� �����͸� ����.
	/// @param uid	MUID
	/// @return		���۷��� ������, MUID�� �������� ������ NULL�� ����
	void* GetRef(MUID& uid);

	/// ��ϵ� MUID ����.
	/// @param uid	MUID
	/// @return		��ϵǾ��� ���۷��� ������, MUID�� �������� ������ NULL�� ����
	void* Remove(MUID& uid);

	/// nSize��ŭ Ȯ��
	MUIDRANGE Reserve(int nSize);

	/// ���ݱ��� �߱޵� UID ����
	MUIDRANGE GetReservedCount(void);
};

/*
/// MUID Reference Array
/// - ���� ���� �뷮�� MUID�� �����س��� ���� MUID ����
class MUIDRefArray : protected vector<void*>{
	MUID	m_CurrentMUID;		///< ���� MUID
public:
	MUIDRefArray(void);
	virtual ~MUIDRefArray(void);

	/// ���۷����� MUID�ϳ��� �Ҵ��Ѵ�.
	/// @param pRef	���۷��� ������
	/// @return		�Ҵ�� MUID
	MUID Generate(void* pRef);
	/// MUID�� ���� ���۷��� �����͸� ����.
	/// @param uid	MUID
	/// @return		���۷��� ������, MUID�� �������� ������ NULL�� ����
	void* GetRef(MUID& uid);
};
*/

/// MUID Reference Map Cache
/// - ��ü UID�� �˻� ���ϸ� ���̱� ���� ĳ�� Ŭ����
class MUIDRefCache : public map<MUID, void*>{
public:
	MUIDRefCache(void);
	virtual ~MUIDRefCache(void);

	/// ���۷����� MUID�ϳ��� �Ҵ��Ѵ�.
	/// @param pRef	���۷��� ������
	/// @return		�Ҵ�� MUID
	void Insert(const MUID& uid, void* pRef);
	/// MUID�� ���� ���۷��� �����͸� ����.
	/// @param uid	MUID
	/// @return		���۷��� ������, MUID�� �������� ������ NULL�� ����
	void* GetRef(const MUID& uid);
	/// ��ϵ� MUID ����.
	/// @param uid	MUID
	/// @return		��ϵǾ��� ���۷��� ������, MUID�� �������� ������ NULL�� ����
	void* Remove(const MUID& uid);
};

#endif