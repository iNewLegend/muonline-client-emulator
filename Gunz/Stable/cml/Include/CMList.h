//	CMList.h
//		Linked List
//		����ȣ ( 97-12-30 4:15:03 ���� )
//////////////////////////////////////////////////////////////////////

#ifndef LIST_H
#define LIST_H

#ifdef __BORLANDC__
#include <assert.h>
#define _ASSERT assert
#else
#include <crtdbg.h>			// _ASSERT
#endif

#include <windows.h>		// BOOL

// ����Ʈ�� ������ ���ڵ�
template<class NEW_TYPE>
class CMRecord{
	CMRecord<NEW_TYPE>*	m_lpPrevious;
	CMRecord<NEW_TYPE>*	m_lpNext;
public:
	NEW_TYPE*			m_lpContent;

	// ��� �ʱ�ȭ
	CMRecord(void){
		m_lpNext		=NULL;
		m_lpPrevious	=NULL;
		m_lpContent		=NULL;
	}
	// ���� ��� ����
	void SetPreviousPointer(CMRecord<NEW_TYPE>* pprevious){
		m_lpPrevious=pprevious;
	}
	// ���� ��� ���
	CMRecord<NEW_TYPE>* GetPreviousPointer(void){
		return m_lpPrevious;
	}
	// ���� ��� ����
	void SetNextPointer(CMRecord<NEW_TYPE>* pnext){
		m_lpNext=pnext;
	}
	// ���� ��� ���
	CMRecord<NEW_TYPE>* GetNextPointer(void){
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
class CMLinkedList{
private:
	CMRecord<NEW_TYPE>*		m_lpFirstRecord;	// ù��° ���ڵ�
	CMRecord<NEW_TYPE>*		m_lpLastRecord;		// ù��° ���ڵ�
	CMRecord<NEW_TYPE>*		m_lpCurrentRecord;	// ���� ���ڵ�
	int						m_nListCount;		// ���ڵ��� ����
	int						m_nCurrentPosition;	// ���� ��ġ

	//MString					m_lpszErrorString;	// ���� ��Ʈ��
public:
	// ���� �ʱ�ȭ
	CMLinkedList(void);
	// ����Ʈ ����(�˾Ƽ� ���ڵ带 ��� �����Ѵ�)
	virtual ~CMLinkedList(void);

	// ���� �޼��� ���
	//MString GetErrorMessage(void){return m_lpszErrorString;}
	
	// �� �տ� ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	BOOL InsertHead(NEW_TYPE *lpRecord);
	// ���� ��ġ ������ ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	BOOL AddAfter(NEW_TYPE *lpRecord);
	// ���� ��ġ ������ ���ڵ� ����(new�� ������ ��ü�� �״�� ������.)
	BOOL Insert(NEW_TYPE *lpRecord);
	// �� �ڿ� ���ڵ� �߰�(new�� ������ ��ü�� �״�� ������.)
	BOOL Add(NEW_TYPE *lpRecord);
	
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
	BOOL PreviousRecord(void);
	// ���� ���ڵ��
	BOOL NextRecord(void);
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
	virtual int Compare(NEW_TYPE *lpRecord1,NEW_TYPE *lpRecord2){return -10;/*FALSE*/}

	// Quick Sort�� �����Ѵ�.
	void Sort(void);
private:
	// Quick Sort�� ���� �Լ�
	void QuickSort(int first,int last);
};

// ���� �ʱ�ȭ
template<class NEW_TYPE>
CMLinkedList<NEW_TYPE>::CMLinkedList(void)
{
	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_nListCount=0;
	m_nCurrentPosition=0;
}

// ����Ʈ ����(�˾Ƽ� ���ڵ带 ��� �����Ѵ�)
template<class NEW_TYPE>
CMLinkedList<NEW_TYPE>::~CMLinkedList(void)
{
	DeleteAll();

	m_lpFirstRecord=NULL;
	m_lpLastRecord=NULL;
	m_lpCurrentRecord=NULL;
	m_nListCount=0;
	m_nCurrentPosition=0;
}

// ���ڵ� ����
template<class NEW_TYPE>
BOOL CMLinkedList<NEW_TYPE>::AddAfter(NEW_TYPE *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CMRecord<NEW_TYPE> *ptemp;
		ptemp=new CMRecord<NEW_TYPE>;
		if(ptemp==NULL){
			//m_lpszErrorString="CMLinkedList::Insert() - Memory Allocation Error";
			return FALSE;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CMRecord<NEW_TYPE> *ptemp;
		ptemp=new CMRecord<NEW_TYPE>;
		if(ptemp==NULL)return FALSE;
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
	return TRUE;
}

// ���ڵ� ����
template<class NEW_TYPE>
BOOL CMLinkedList<NEW_TYPE>::Insert(NEW_TYPE *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CMRecord<NEW_TYPE> *ptemp;
		ptemp=new CMRecord<NEW_TYPE>;
		if(ptemp==NULL){
			//m_lpszErrorString="CMLinkedList::Insert() - Memory Allocation Error";
			return FALSE;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CMRecord<NEW_TYPE> *ptemp;
		ptemp=new CMRecord<NEW_TYPE>;
		if(ptemp==NULL)return FALSE;
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
	return TRUE;
}

template<class NEW_TYPE>
BOOL CMLinkedList<NEW_TYPE>::InsertHead(NEW_TYPE *lpRecord)
{
	_ASSERT(lpRecord!=NULL);

	MoveFirst();

	// ����Ʈ�� ù ���ڵ��̸�
	if(m_nListCount==0){
		CMRecord<NEW_TYPE> *ptemp;
		ptemp=new CMRecord<NEW_TYPE>;
		if(ptemp==NULL){
			//m_lpszErrorString="CMLinkedList::Insert() - Memory Allocation Error";
			return FALSE;
		}
		// �ܼ��� ����Ÿ ������ ����. ����Ÿ�� ������ �������� �Ҵ�Ǿ� �־�� �Ѵ�.
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		
		m_lpFirstRecord=ptemp;
		m_lpLastRecord=ptemp;
		m_lpCurrentRecord=m_lpFirstRecord;
	}
	else{
		CMRecord<NEW_TYPE> *ptemp;
		ptemp=new CMRecord<NEW_TYPE>;
		if(ptemp==NULL)return FALSE;
		ptemp->m_lpContent=lpRecord;
		//memcpy(&(ptemp->m_lpContent),lpRecord,sizeof(NEW_TYPE));
		// ���� ���ڵ�� ���� ���ڵ� ���̿� ����
		// �� ���ڵ�
		ptemp->SetPreviousPointer(NULL);
		ptemp->SetNextPointer(m_lpCurrentRecord);
		// ���� ���ڵ�
		if((ptemp->GetNextPointer()!=NULL))
			(ptemp->GetNextPointer())->SetPreviousPointer(ptemp);
		else{
			// �������� �߰��ϴ� ���̹Ƿ�
			m_lpLastRecord=ptemp;
		}

		m_lpFirstRecord=ptemp;

		// ���� �����͸� ���� ������ ���ڵ����� ��ġ
		m_lpCurrentRecord=ptemp;

		//m_nCurrentPosition++;
	}

	// ����Ʈ ī��Ʈ ����
	m_nListCount++;
	return TRUE;
}

// ���ڵ� �߰�
template<class NEW_TYPE>
BOOL CMLinkedList<NEW_TYPE>::Add(NEW_TYPE *lpRecord)
{
	MoveLast();
	return Insert(lpRecord);
}

// ���� ��ġ�� ���ڵ带 ����
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::Delete(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	_ASSERT(m_lpCurrentRecord!=NULL);

	CMRecord<NEW_TYPE>* pprevious;
	CMRecord<NEW_TYPE>* pnext;

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
	_ASSERT(m_lpCurrentRecord->m_lpContent!=NULL);
	delete m_lpCurrentRecord->m_lpContent;
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
void CMLinkedList<NEW_TYPE>::Delete(int iindex)
{
	if(iindex<0 || iindex>=m_nListCount) {
		_ASSERT(FALSE);
		return ;
	}

	MoveRecord(iindex);
	Delete();
}

// ��� ���ڵ� ����
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::DeleteAll(void)
{
	while(m_nListCount!=0)
		Delete();
	m_nCurrentPosition=0;
}

// ���� ��ġ�� ���ڵ带 ����, but Record�� content�� �������� ����  
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::DeleteRecord(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	_ASSERT(m_lpCurrentRecord!=NULL);

	CMRecord<NEW_TYPE>* pprevious;
	CMRecord<NEW_TYPE>* pnext;

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

// ���ϴ� �ε����� ����, but Record�� content?�������� ���� 
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::DeleteRecord(int iindex)
{
	if(iindex<0 || iindex>=m_nListCount) {
		_ASSERT(FALSE);
		return;
	}

	MoveRecord(iindex);
	DeleteRecord();
}

// ��� ���ڵ� ����, but Record�� content�� �������� ���� 
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::DeleteRecordAll(void)
{
	while(m_nListCount!=0)
		DeleteRecord();
	m_nCurrentPosition=0;
}


// ���� ���ڵ��
template<class NEW_TYPE>
BOOL CMLinkedList<NEW_TYPE>::PreviousRecord(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetPreviousPointer())!=NULL){
		m_nCurrentPosition--;
		_ASSERT(m_nCurrentPosition>=0);

		m_lpCurrentRecord=m_lpCurrentRecord->GetPreviousPointer();
		return TRUE;
	}	
	else{
		//m_lpszErrorString="CMLinkedList::PreviousRecord() - Out of Range";
		return FALSE;
	}
}

// ���� ���ڵ��
template<class NEW_TYPE>
BOOL CMLinkedList<NEW_TYPE>::NextRecord(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	if((m_lpCurrentRecord->GetNextPointer())!=NULL){
		m_nCurrentPosition++;
		_ASSERT(m_nCurrentPosition<m_nListCount);

		m_lpCurrentRecord=m_lpCurrentRecord->GetNextPointer();
		return TRUE;
	}	
	else{
		//m_lpszErrorString="CMLinkedList::NextRecord() - Out of Range";
		return FALSE;
	}
}

// ������ ��ġ��
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::MoveRecord(int iindex)
{
	// �ε����� ������ �����ȿ� �־�� �Ѵ�.
	if(iindex<0 || iindex>=m_nListCount) {
		_ASSERT(FALSE);
		return;
	}
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
void CMLinkedList<NEW_TYPE>::MoveFirst(void)
{
	m_nCurrentPosition=0;
	m_lpCurrentRecord=m_lpFirstRecord;
}

// �� ����������
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::MoveLast(void)
{
	if(m_nListCount>0){
		m_nCurrentPosition=m_nListCount-1;
		m_lpCurrentRecord=m_lpLastRecord;
	}
}

// ���� ���ڵ��� ������ ����.
template<class NEW_TYPE>
NEW_TYPE *CMLinkedList<NEW_TYPE>::Get(void)
{
	// ���� ���ڵ��� �����ʹ� NULL�ϼ� ����.
	// ��, �ƹ��͵� ����Ǿ� ���� ���� ����
	_ASSERT(m_lpCurrentRecord!=NULL);

	return(m_lpCurrentRecord->Get());
}

// ������ �ε����� ���� ����.
template<class NEW_TYPE>
NEW_TYPE *CMLinkedList<NEW_TYPE>::Get(int iindex)
{
	if(iindex<0 || iindex>=m_nListCount) {
		_ASSERT(FALSE);
		return NULL;
	}

	MoveRecord(iindex);
	return Get();
}

// ������ �ε����� ���� ����.
/*
template<class NEW_TYPE>
NEW_TYPE &CMLinkedList<NEW_TYPE>::operator[](int iindex)
{
	MoveRecord(iindex);
	return Get();
}
*/

// Quick Sort�� �����Ѵ�.
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::Sort(void)
{
	if(GetCount()<=1)return;
	QuickSort(0,GetCount()-1);
}

// Quick Sort�� ���� �Լ�
template<class NEW_TYPE>
void CMLinkedList<NEW_TYPE>::QuickSort(int first,int last)
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
void CMLinkedList<NEW_TYPE>::Swap(int nIndex1,int nIndex2)
{
	if(nIndex1<0 || nIndex1>=m_nListCount) {
		_ASSERT(FALSE);
		return;
	}

	MoveRecord(nIndex1);
	CMRecord<NEW_TYPE>* pRecord1=m_lpCurrentRecord;


	if(nIndex2<0 || nIndex2>=m_nListCount) {
		_ASSERT(FALSE);
		return;
	}

	MoveRecord(nIndex2);
	CMRecord<NEW_TYPE>* pRecord2=m_lpCurrentRecord;

	NEW_TYPE *pData=pRecord1->m_lpContent;
	pRecord1->m_lpContent=pRecord2->m_lpContent;
	pRecord2->m_lpContent=pData;
}

#endif
