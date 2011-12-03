#ifndef ZMESHVIEWLIST_H
#define ZMESHVIEWLIST_H

#include "MGroup.h"
#include "RMesh.h"

//class MButton;
class MBmButton;
class ZMeshView;

// �������� �׸��� ���� MGroup�� ��ӹ޴´�.
class ZMeshViewList : public MGroup{
protected:
	int	m_nItemStartIndex;	// ������ ���� �ε���
	int	m_nItemWidth;		// ������ ���� ũ��
	//MButton*	m_pLeft;	// ���� �̵�
	//MButton*	m_pRight;	// ������ �̵�
	MBmButton* m_pBmLeft;
	MBmButton* m_pBmRight;
	//list<ZMeshView*>	m_Items;
	float m_ScrollButtonWidth;

protected:
	virtual void OnDraw(MDrawContext* pDC);
	virtual void OnSize(int w, int h);
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage);
	int GetItemVisibleWidth(void);
	int GetItemWidth(void);
	int GetVisibleCount(void);
	void RecalcBounds(void);
public:
	ZMeshViewList(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~ZMeshViewList(void);

	int GetItemCount(void);
	ZMeshView* GetItem(int i);

	void Add(RealSpace2::RMesh* pMeshRef);
	void RemoveAll(void);
	void Remove(int i);

	void SetItemWidth(int nWidth);
};

#endif