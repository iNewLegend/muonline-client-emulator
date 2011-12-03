//	CMPtrList.h
//		Pointer Linked List
//		����ȣ ( 99-01-24  )
//////////////////////////////////////////////////////////////////////

#ifndef CMPTRLIST_H
#define CMPTRLIST_H

#include <crtdbg.h>			// _ASSERT

// ����Ʈ�� ������ ���ڵ�
template<class NEW_TYPE>
class CMPtrRecord{
	CMPtrRecord<NEW_TYPE>*	m_lpPrevious;
	CMPtrRecord<NEW_TYPE>*	m_lpNext;
public:
	NEW_TYPE*			m_lpContent;

	// ��� �ʱ�ȭ
	CMPtrRecord(void){
		m_lpNext		=NULL;
		m_lpPrevious	=NULL;
		m_lpContent		=NULL;
	}
	// ���� ��� ����
	void SetPreviousPointer(CMPtrRecord<NEW_TYPE>* pprevious){
		m_lpPrevious=pprevious;
	}
	// ���� ��� ���
	CMPtrRecord<NEW_TYPE>* GetPreviousPointer(void){
		return m_lpPrevious;
	}
	// ���� ��� ����
	void SetNextPointer(CMPtrRecord<NEW_TYPE>* pnext){
		m_lpNext=pnext;
	}
	// ���� ��� ���
	CMPtrRecord<NEW_TYPE>* GetNextPointer(void){
		return m_lpNext;
	}
	// ���� ���
	NEW_TYPE *Get(void){
		return m_lpContent;
	}
	/*
	���۷����� ��ȣ���� ����������, ������� �ʴ´�.
	NEW_TYPE &Get(void){
		return *m_lpContent;
	}
	*/
};

// ����Ʈ Ŭ����
template<class NEW_TYPE>
class CMPtrList{
private:
	CMPtrRecord<NEW_TYPE>*		m_lpFirstRecord;	// ù��° ���ڵ�
	CMPtrRecord<NEW_TYPE>*		m_lpLastRecord;		// ù��° ���ڵ�
	CMPtrRecord<NEW_TYPE>*		m_lpCurrentRecord;	// ���� ���ڵ�
	int						m_nListCount;		// ���ڵ��� ����
	int						m_nCurrentPosition;	// ���� ��ġ

	//MString					m_lpszErrorString;	// ���� ��Ʈ��
public:
	// ���� �ʱ�ȭ
	CMPtrList(void);
	// ����Ʈ ����(�˾Ƽ� ���ڵ带 ��� �����Ѵ�)
	virtual ~CMPtrList(void);

	// ���� �޼��� ���
	//MString GetErrorMessage(void){return m_lpszErrorString;}

	// ���� ��ġ ������ ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	bool InsertBefore(NEW_TYPE *lpRecord);
	// ���� ��ġ ������ ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	bool AddAfter(NEW_TYPE *lpRecord);
	// ���� ��ġ ������ ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	bool Insert(NEW_TYPE *lpRecord);
	// �� �ڿ� ���ڵ� �߰�(new�� ������ ��ü�� �״�� ������.)
	bool Add(NEW_TYPE *lpRecord);
	// ��Ʈ�� ���鼭 �߰�(��, Compare��ƾ�� �־�� �Ѵ�.)
	bool AddSorted(NEW_TYPE *lpRecord);
	
	// ���� ��ġ�� ���ڵ带 ����
	void Delete(void);
	// ������ ��ġ�� ���ڵ带 ����
	void Delete(int iindex);
	// ��� ���ڵ� ����
	void DeleteAll(void);

	// Added by Kim young ho
	// ���� ��ġ�� ���ڵ带 ����, but Record�� content�� �������� ���� 
	void DeleteRecord(void);
	// ������ ��ġ�� ���ڵ带 ����, but Record�� content�� �������� ���� 
	void DeleteRecord(int iindex);
	// ��� ���ڵ� ����, but Record�� content�� �������� ���� 
	void DeleteRecordAll(void);

	// ���ڵ��� ���� ����
	int GetCount(void){return m_nListCount;}
	// ���ڵ��� ���� ��ġ ����
	int GetIndex(void){return m_nCurrentPosition;}

	////////////////////////////////////////////
	// ����Ʈ���� �����͸� �̿��� �̵��� �� ���
	// ���� ���ڵ��
	bool PreviousRecord(void);
	// ���� ���ڵ��
	bool NextRecord(void);
	// ������ ��ġ��
	void MoveRecord(int iindex);
	// �� ó������
	void MoveFirst(void);
	// �� ����������
	void MoveLast(void);

	// ���� ���ڵ��� ������ ����.
	NEW_TYPE *Get(void);

	// ������ �ε����� ���� ����.
	NEW_TYPE *Get(int iindex);

	// ������ �ε����� ���� ����.
	// ���۷����� ���� ��ȣ������ ������� �ʴ´�.
	//NEW_TYPE &operator[](int iindex);

	// �ΰ��� ���ڵ��� ���� Swap�Ѵ�.
	void Swap(int nIndex1,int nIndex2);

	// Record Compare Function. Sort�� ���� virtual�� ��¹޾ƾ� �Ѵ�.
	virtual int Compare(NEW_TYPE *lpRecord1,NEW_TYPE *lpRecord2){return -10;/*false*/}

	// Quick Sort�� �����Ѵ�.
	void Sort(void);
private:
	// Quick Sort�� ���� �Լ�
	void QuickSort(int first,int last);
};

// ���� �ʱ�ȭ
template<class NEW_TYPE>
CMPtrList<NEW_TYPE>::CMPtrList(void)
{
	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_nListCount=0;
	m_nCurrentPosition=0;
}

// ����Ʈ ����(�˾Ƽ� ���ڵ带 ��� �����Ѵ�)
template<class NEW_TYPE>
CMPtrList<NEW_TYPE>::~CMPtrList(void)
{
	DeleteAll();

	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_nListCount=0;
	m_nCurrentPosition=0;
}

template<class NEW_TYPE>
bool CMPtrList<NEW_TYPE>::InsertBefore(NEW_TYPE *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CMPtrRecord<NEW_TYPE> *ptemp;
		ptemp=new CMPtrRecord<NEW_TYPE>;
		if(ptemp==NULL){
			//m_lpszErrorString="CMPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CMPtrRecord<NEW_TYPE> *ptemp;
		ptemp=new CMPtrRecord<NEW_TYPE>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		// ���� ���ڵ�� ���� ���ڵ� ���̿� ����
		// �� ���ڵ�
		ptemp->SetPreviousPointer(m_lpCurrentRecord->GetPreviousPointer());
		ptemp->SetNextPointer(m_lpCurrentRecord);
		// ���� ���ڵ�

		// ����(��) ���ڵ�
		m_lpCurrentRecord->SetPreviousPointer(ptemp);

		if(ptemp->GetPreviousPointer()==NULL)
			m_lpFirstRecord = ptemp;
		else
			ptemp->GetPreviousPointer()->SetNextPointer(ptemp);

		// ���� �����͸� ���� ������ ���ڵ����� ��ġ
		m_lpCurrentRecord=ptemp;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount++;
	return true;
}

// ���ڵ� ����
template<class NEW_TYPE>
bool CMPtrList<NEW_TYPE>::AddAfter(NEW_TYPE *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CMPtrRecord<NEW_TYPE> *ptemp;
		ptemp=new CMPtrRecord<NEW_TYPE>;
		if(ptemp==NULL){
			//m_lpszErrorString="CMPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CMPtrRecord<NEW_TYPE> *ptemp;
		ptemp=new CMPtrRecord<NEW_TYPE>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		// ���� ���ڵ�� ���� ���ڵ� ���̿� ����
		// �� ���ڵ�
		ptemp->SetPreviousPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		// ���� ���ڵ�
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else{
			// �������� �߰��ϴ� ���̹Ƿ�
			m_lpLastRecord=ptemp;
		}

		// ����(��) ���ڵ�
		m_lpCurrentRecord->SetNextPointer(ptemp);

		// ���� �����͸� ���� ������ ���ڵ����� ��ġ
		m_lpCurrentRecord=ptemp;

		m_nCurrentPosition++;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount++;
	return true;
}

// ���ڵ� ����
template<class NEW_TYPE>
bool CMPtrList<NEW_TYPE>::Insert(NEW_TYPE *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CMPtrRecord<NEW_TYPE> *ptemp;
		ptemp=new CMPtrRecord<NEW_TYPE>;
		if(ptemp==NULL){
			//m_lpszErrorString="CMPtrList::Insert() - Memory Allocation Error";
			return false;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CMPtrRecord<NEW_TYPE> *ptemp;
		ptemp=new CMPtrRecord<NEW_TYPE>;
		if(ptemp==NULL)return false;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		// ���� ���ڵ�� ���� ���ڵ� ���̿� ����
		// �� ���ڵ�
		ptemp->SetPreviousPointer(m_lpCurrentRecord);
		ptemp->SetNextPointer(m_lpCurrentRecord->GetNextPointer());
		// ���� ���ڵ�
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else{
			// �������� �߰��ϴ� ���̹Ƿ�
			m_lpLastRecord=ptemp;
		}

		// ����(��) ���ڵ�
		m_lpCurrentRecord->SetNextPointer(ptemp);

		// ���� �����͸� ���� ������ ���ڵ����� ��ġ
		m_lpCurrentRecord=ptemp;

		m_nCurrentPosition++;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount++;
	return true;
}

// ���ڵ� �߰�
template<class NEW_TYPE>
bool CMPtrList<NEW_TYPE>::Add(NEW_TYPE *lpRecord)
{
	MoveLast();
	return Insert(lpRecord);
}

template<class NEW_TYPE>
bool CMPtrList<NEW_TYPE>::AddSorted(NEW_TYPE *lpRecord)
{
	int nTotalCount = GetCount();
	if(nTotalCount==0) return Add(lpRecord);

	int nStart = 0;
	int nEnd = nTotalCount - 1;
	while(1){
		int nCount = nEnd - nStart;
		int nMiddlePos = nCount / 2;
		if(nMiddlePos<0) nMiddlePos = 0;
		int nCurrPos = nStart+nMiddlePos;
		int nCompare = Compare(Get(nCurrPos), lpRecord);
		_ASSERT(nCompare!=-10);	// Invalide Comparision
		if(nStart==nEnd){
			if(nCompare<0){
				if(nCurrPos==nTotalCount-1) return Add(lpRecord);
				else{
					MoveRecord(nCurrPos+1);
					return InsertBefore(lpRecord);
				}
			}
			else{
				MoveRecord(nCurrPos);
				return InsertBefore(lpRecord);
			}
		}

		if( nCompare > 0 ){
			nEnd = nCurrPos - 1;
			if(nEnd<nStart){
				MoveRecord(nCurrPos);
				return InsertBefore(lpRecord);
			}
		}
		else if( nCompare == 0 ){
			MoveRecord(nCurrPos);
			return InsertBefore(lpRecord);
		}
		else{
			nStart = nCurrPos + 1;
			if(nStart>nEnd){
				if(nCurrPos==nTotalCount-1) return Add(lpRecord);
				else{
					MoveRecord(nCurrPos+1);
					return InsertBefore(lpRecord);
				}
			}
		}
	}
}

// ���� ��ġ�� ���ڵ带 ����
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::Delete(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	_ASSERT(m_lpCurrentRecord!=NULL);

	CMPtrRecord<NEW_TYPE>* pprevious;
	CMPtrRecord<NEW_TYPE>* pnext;

	pprevious	=m_lpCurrentRecord->GetPreviousPointer();
	pnext		=m_lpCurrentRecord->GetNextPointer();

	// �� ���ڵ�
	if(pprevious!=NULL){
	// �� ���ڵ尡 ó���� �ƴϸ�...
		pprevious->SetNextPointer(pnext);
	}
	else{
	// �� ���ڵ尡 ù ���ڵ��� ���
		m_lpFirstRecord=pnext;
	}
	
	// �� ���ڵ�
	if(pnext!=NULL){
	// �� ���ڵ尡 �������� �ƴϸ�...
		pnext->SetPreviousPointer(pprevious);
	}
	else{
	// �� ���ڵ尡 �������̸�
		m_lpLastRecord=pprevious;
	}

	// ���� ���ڵ��� ���� �޸𸮿��� ����
	//_ASSERT(m_lpCurrentRecord->m_lpContent!=NULL);
	//delete m_lpCurrentRecord->m_lpContent;
	// ���� ���ڵ� �޸𸮿��� ����
	delete m_lpCurrentRecord;

	// ���� ������ ����
	if(pnext!=NULL)
		m_lpCurrentRecord=pnext;
	else{
		if(pprevious!=NULL){
			m_lpCurrentRecord=pprevious;
			m_nCurrentPosition--;
			_ASSERT(m_nCurrentPosition>=0);
		}
		else
			m_lpCurrentRecord=NULL;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount--;
}

// ���ϴ� �ε����� ����
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::Delete(int iindex)
{
	MoveRecord(iindex);
	Delete();
}

// ��� ���ڵ� ����
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::DeleteAll(void)
{
	while(m_nListCount!=0)
		Delete();
	m_nCurrentPosition=0;
}

// ���� ��ġ�� ���ڵ带 ����, but Record�� content�� �������� ����  
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::DeleteRecord(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	_ASSERT(m_lpCurrentRecord!=NULL);

	CMPtrRecord<NEW_TYPE>* pprevious;
	CMPtrRecord<NEW_TYPE>* pnext;

	pprevious	=m_lpCurrentRecord->GetPreviousPointer();
	pnext		=m_lpCurrentRecord->GetNextPointer();

	// �� ���ڵ�
	if(pprevious!=NULL){
	// �� ���ڵ尡 ó���� �ƴϸ�...
		pprevious->SetNextPointer(pnext);
	}
	else{
	// �� ���ڵ尡 ù ���ڵ��� ���
		m_lpFirstRecord=pnext;
	}
	
	// �� ���ڵ�
	if(pnext!=NULL){
	// �� ���ڵ尡 �������� �ƴϸ�...
		pnext->SetPreviousPointer(pprevious);
	}
	else{
	// �� ���ڵ尡 �������̸�
		m_lpLastRecord=pprevious;
	}

	// ���� ���ڵ� �޸𸮿��� ����
	delete m_lpCurrentRecord;

	// ���� ������ ����
	if(pnext!=NULL)
		m_lpCurrentRecord=pnext;
	else{
		if(pprevious!=NULL){
			m_lpCurrentRecord=pprevious;
			m_nCurrentPosition--;
			_ASSERT(m_nCurrentPosition>=0);
		}
		else
			m_lpCurrentRecord=NULL;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount--;
}

// ���ϴ� �ε����� ����, but Record�� content� �������� ���� 
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::DeleteRecord(int iindex)
{
	MoveRecord(iindex);
	DeleteRecord();
}

// ��� ���ڵ� ����, but Record�� content�� �������� ���� 
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::DeleteRecordAll(void)
{
	while(m_nListCount!=0)
		DeleteRecord();
	m_nCurrentPosition=0;
}


// ���� ���ڵ��
template<class NEW_TYPE>
bool CMPtrList<NEW_TYPE>::PreviousRecord(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetPreviousPointer())!=NULL){
		m_nCurrentPosition--;
		_ASSERT(m_nCurrentPosition>=0);

		m_lpCurrentRecord=m_lpCurrentRecord->GetPreviousPointer();
		return true;
	}	
	else{
		//m_lpszErrorString="CMPtrList::PreviousRecord() - Out of Range";
		return false;
	}
}

// ���� ���ڵ��
template<class NEW_TYPE>
bool CMPtrList<NEW_TYPE>::NextRecord(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetNextPointer())!=NULL){
		m_nCurrentPosition++;
		_ASSERT(m_nCurrentPosition<m_nListCount);

		m_lpCurrentRecord=m_lpCurrentRecord->GetNextPointer();
		return true;
	}	
	else{
		//m_lpszErrorString="CMPtrList::NextRecord() - Out of Range";
		return false;
	}
}

// ������ ��ġ��
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::MoveRecord(int iindex)
{
	// �ε����� ������ �����ȿ� �־�� �Ѵ�.
	_ASSERT(iindex<m_nListCount);
	_ASSERT(iindex>=0);
	
	if(iindex==0)
	{
		MoveFirst();
		return;
	}
	if(iindex>m_nCurrentPosition){
		while(iindex!=m_nCurrentPosition)
			NextRecord();
	}
	else if(iindex<m_nCurrentPosition){
		while(iindex!=m_nCurrentPosition)
			PreviousRecord();
	}
}

// �� ó������
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::MoveFirst(void)
{
	m_nCurrentPosition=0;
	m_lpCurrentRecord=m_lpFirstRecord;
}

// �� ����������
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::MoveLast(void)
{
	if(m_nListCount>0){
		m_nCurrentPosition=m_nListCount-1;
		m_lpCurrentRecord=m_lpLastRecord;
	}
}

// ���� ���ڵ��� ������ ����.
template<class NEW_TYPE>
NEW_TYPE *CMPtrList<NEW_TYPE>::Get(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	return(m_lpCurrentRecord->Get());
}

// ������ �ε����� ���� ����.
template<class NEW_TYPE>
NEW_TYPE *CMPtrList<NEW_TYPE>::Get(int iindex)
{
	MoveRecord(iindex);
	return Get();
}

// ������ �ε����� ���� ����.
/*
template<class NEW_TYPE>
NEW_TYPE &CMPtrList<NEW_TYPE>::operator[](int iindex)
{
	MoveRecord(iindex);
	return Get();
}
*/

// Quick Sort�� �����Ѵ�.
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::Sort(void)
{
	if(GetCount()<=1)return;
	QuickSort(0,GetCount()-1);
}

// Quick Sort�� ���� �Լ�
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::QuickSort(int first,int last)
{
	int i,j;

	i=first,j=last;
	NEW_TYPE *pMiddle=Get((first+last)/2);

	// Compare()�� virtual�� ��¹��� ������ Assetion�� �߻���Ŵ
	_ASSERT(Compare(Get(i),pMiddle)!=-10);

	for(;;){
		while(Compare(Get(i),pMiddle)<0)i++;
		while(Compare(Get(j),pMiddle)>0)j--;
		if(i>=j)break;
		Swap(i,j);
		i++;j--;
	}
	if(first<i-1)QuickSort(first,i-1);
	if(j+1<last)QuickSort(j+1,last);
}

// �ΰ��� ���ڵ��� ���� Swap�Ѵ�.
template<class NEW_TYPE>
void CMPtrList<NEW_TYPE>::Swap(int nIndex1,int nIndex2)
{
	MoveRecord(nIndex1);
	CMPtrRecord<NEW_TYPE>* pRecord1=m_lpCurrentRecord;
	MoveRecord(nIndex2);
	CMPtrRecord<NEW_TYPE>* pRecord2=m_lpCurrentRecord;

	NEW_TYPE *pData=pRecord1->m_lpContent;
	pRecord1->m_lpContent=pRecord2->m_lpContent;
	pRecord2->m_lpContent=pData;
}

#endif
