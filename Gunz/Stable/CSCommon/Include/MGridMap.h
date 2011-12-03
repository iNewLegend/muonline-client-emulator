#ifndef MGRIDHASH_H
#define MGRIDHASH_H

#include <crtdbg.h>
#include <list>
#include <math.h>
using namespace std;


/// x-y �׸���ʰ� z�� ����Ʈ�� ������ �ִ� Ŭ����
/// - ����
///  - MGridMap<UserType>
/// - ������Ʈ �߰�/�̵�/����
///  - MGridMap<UserType>::HREF h = MGridMap<UserType>::Add(x, y, z, UserTypeObj);
///  - MGridMap<UserType>::Move(x, y, z, UserTypeObj, h);
///  - MGridMap<UserType>::Del(h);
template<class _T>
class MGridMap{
public:
	/// ���ο��� �����ϱ����� ��ġ������ UserData
	struct MITEM{
		float	x, y, z;	///< Position
		_T		Obj;		///< User Data Object
	};
	/// x-y��鿡 �������� �ϳ��� ���� ��
	class MRefCell : public list<MITEM>{};

	/// User Data Object���� ������ ���� ���۷����� �ڵ�
	struct HREF{
		MRefCell*			pRefCell;
		MRefCell::iterator	RefIterator;
	};

protected:
	MRefCell*	m_pGridMap;		///< �׸����
	float		m_fSX;			///< ���� X�� ������
	float		m_fSY;			///< ���� Y�� ������
	float		m_fEX;			///< ���� X�� ����
	float		m_fEY;			///< ���� Y�� ����
	//float		GetXSize();		///< ���� X�� ũ��
	//float		GetYSize();		///< ���� Y�� ũ��
	int			m_nXDivision;	///< ���� X�� �� ����
	int			m_nYDivision;	///< ���� Y�� �� ����

protected:
	MRefCell* GetCell(float x, float y){
		int nXPos = int((x-m_fSX)/(GetXSize()/(float)m_nXDivision));
		int nYPos = int((y-m_fSY)/(GetYSize()/(float)m_nYDivision));

		// ���� �˻�
		_ASSERT(nXPos>=0 && nYPos>=0);
		if(nXPos>=m_nXDivision) return NULL;
		if(nYPos>=m_nYDivision) return NULL;

		return &(m_pGridMap[nXPos+nYPos*m_nXDivision]);
	}
public:
	MGridMap(void){
		m_pGridMap = NULL;
	}
	virtual ~MGridMap(void){
		Destroy();
	}

	/// �� ����
	/// @param fSX			���� X�� ������
	/// @param fSY			���� Y�� ������
	/// @param fXSize		���� X�� ũ��
	/// @param fYSize		���� Y�� ũ��
	/// @param nXDivision	���� X�� �� ����
	/// @param nYDivision	���� Y�� �� ����
	//void Create(float fSX, float fSY, float fXSize, float fYSize, int nXDivision, int nYDivision){
	void Create(float fSX, float fSY, float fEX, float fEY, int nXDivision, int nYDivision){
		m_pGridMap = new MRefCell[nXDivision*nYDivision];
		m_fSX = fSX;
		m_fSY = fSY;
		m_fEX = fEX;
		m_fEY = fEY;
		//GetXSize() = fXSize;
		//GetYSize() = fYSize;
		m_nXDivision = nXDivision;
		m_nYDivision = nYDivision;
	}
	/// ����
	void Destroy(void){
		if(m_pGridMap!=NULL){
			delete[] m_pGridMap;
			m_pGridMap = NULL;
		}
	}

	/// ���� ���� �߰�
	/// @return �߰��� ������Ʈ�� HREF�� �����ؼ� �̵�/������ ����ؾ� �Ѵ�.
	HREF Add(float x, float y, float z, _T Obj){
		_ASSERT(x>=m_fSX && y>=m_fSY);
		_ASSERT(x<=m_fEX && y<=m_fEY);
		HREF hPos;
		MRefCell* pCell = GetCell(x, y);
		if(pCell==NULL){
			hPos.pRefCell = NULL;
			return hPos;
		}
		MITEM item;
		item.x = x;
		item.y = y;
		item.z = z;
		item.Obj = Obj;
		hPos.RefIterator = pCell->insert(pCell->end(), item);
		hPos.pRefCell = pCell;
		return hPos;
	}
	/// ���� �� ����
	void Del(HREF hRef){
		_ASSERT(hRef.pRefCell!=NULL);
		hRef.pRefCell->erase(hRef.RefIterator);
	}
	/// �ش� ������ Obj����Ʈ ���, Obj����Ʈ�� ���������� ��Ʈ�Ǿ� ���� �ʴ�.
	void Get(list<_T>* pObjs, float x, float y, float z, float fRadius){
		float fXCellSize = GetXSize()/(float)m_nXDivision;
		float fYCellSize = GetYSize()/(float)m_nYDivision;
		int nXPos = int((x-m_fSX)/fXCellSize);
		int nYPos = int((y-m_fSY)/fYCellSize);
#define MORE_SEARCH	2
		int nXRadius = int(fRadius/fXCellSize) + MORE_SEARCH;
		int nYRadius = int(fRadius/fYCellSize) + MORE_SEARCH;
		float fRadiusPow = fRadius*fRadius;
		for(int yp=-nYRadius; yp<=nYRadius; yp++){
			for(int xp=-nXRadius; xp<=nXRadius; xp++){
				float fCellX = (nXPos+xp+(xp>=0?1:0))*fXCellSize + m_fSX;
				float fCellY = (nYPos+yp+(yp>=0?1:0))*fYCellSize + m_fSY;
				float f2DLenPow = float(pow(fCellX-x, 2) + pow(fCellY-y, 2));
				if(f2DLenPow>fRadiusPow) continue;	// �� ��ü�� ���� ������ ����� ���
				int nX = nXPos+xp;
				int nY = nYPos+yp;
				if(nX<0 || nX>=m_nXDivision) continue;
				if(nY<0 || nY>=m_nYDivision) continue;
				MRefCell* pRefCell = &(m_pGridMap[nX+nY*m_nXDivision]);
				for(MRefCell::iterator it=pRefCell->begin(); it!=pRefCell->end(); it++){
					MITEM* pItem = &(*it);
					float f3DLenPow = float(pow(pItem->x-x, 2)+pow(pItem->y-y, 2)+pow(pItem->z-z, 2));
					if(f3DLenPow<=fRadiusPow){
						pObjs->insert(pObjs->end(), pItem->Obj);
					}
				}
			}
		}
	}

	/// Obj �̵��� ���� ���� ���� �̵�
	HREF Move(float x, float y, float z, _T Obj, HREF& hRef){
		_ASSERT(hRef.pRefCell!=NULL);
		_ASSERT((&(*hRef.RefIterator))->Obj==Obj);

		MRefCell* pRefCell = GetCell(x, y);
		// ���� RefCell�� ������ �ƹ��͵� ���� �ʴ´�.
		if(pRefCell==hRef.pRefCell) return hRef;
		// ���� ���� �����
		hRef.pRefCell->erase(hRef.RefIterator);
		// ���ο� ���� �����
		return Add(x, y, z, Obj);
	}

	/// ���� ���� X
	float GetSX(void) const { return m_fSX; }
	/// ���� ���� Y
	float GetSY(void) const { return m_fSY; }
	/// ���� �� X
	float GetEX(void) const { return m_fEX; }
	/// ���� �� Y
	float GetEY(void) const { return m_fEY; }

	/// ���� X�� ũ��
	float GetXSize(void) const { return m_fEX-m_fSX; }
	/// ���� Y�� ũ��
	float GetYSize(void) const { return m_fEY-m_fSY; }
	/// ���� X�� �� ����
	int GetXDivision(void) const { return m_nXDivision; }
	/// ���� Y�� �� ����
	int GetYDivision(void) const { return m_nYDivision; }

	/// ���� X�� �� ũ��
	float GetXDivisionSize(void) const { return GetXSize() / (float)m_nXDivision; }
	/// ���� Y�� �� ũ��
	float GetYDivisionSize(void) const { return GetYSize() / (float)m_nYDivision; }

	/// x, y ��ġ�� �� ���� ���
	MRefCell* GetCell(int x, int y){
		if(x<0 || x>=m_nXDivision) return NULL;
		if(y<0 || y>=m_nYDivision) return NULL;

		return &(m_pGridMap[x+y*m_nXDivision]);
	}
	/// �ε����� �� ���� ���
	MRefCell* GetCell(int i){
		if(i<0 || i>=m_nXDivision*m_nYDivision) return NULL;
		return &(m_pGridMap[i]);
	}
	/// �� ���� ���
	int GetCellCount(void){
		return m_nXDivision*m_nYDivision;
	}
	/// ��� �� �ʱ�ȭ�ϱ�
	void ClearAllCell(void){
		int nCellCount = GetCellCount();
		for(int i=0; i<nCellCount; i++){
			MRefCell* pRefCell = GetCell(i);
			for(MMap::MRefCell::iterator it=pRefCell->begin(); it!=pRefCell->end(); it++){
				MMap::MITEM* pItem = &(*it);
			}
			pRefCell->clear();
		}
	}
};

#endif