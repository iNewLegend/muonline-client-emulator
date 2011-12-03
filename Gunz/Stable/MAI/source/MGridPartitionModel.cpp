#include "MGridPartitionModel.h"
#include <math.h>

struct MGPOVERLAPINFO{
	MGPPOINT	p[2];			// ��ġ�� ����
	int			nDivision[2];	// �� �簢���� ��ġ�� �и�
};

// �� �簢���� ���� �������� ���� ���ϱ�
void GetOverlapLine(MGPOVERLAPINFO* pOI, MGPRECT& r0, MGPRECT& r1)
{
	// �� �簢���� ������ �簢�� ���ϱ�
	MGPRECT ir;
	if(r0.x>=r1.x) ir.x = r0.x;
	else ir.x = r1.x;
	if(r0.y>=r1.y) ir.y = r0.y;
	else ir.y = r1.y;
	if(r0.x+r0.w>r1.x+r1.w) ir.w = r1.x+r1.w - ir.x;
	else ir.w = r0.x+r0.w - ir.x;
	if(r0.y+r0.h>r1.y+r1.h) ir.h = r1.y+r1.h - ir.y;
	else ir.h = r0.y+r0.h - ir.y;

	// ��ġ�� ����
	pOI->p[0].x = ir.x;
	pOI->p[0].y = ir.y;
	pOI->p[1].x = ir.x+ir.w;
	pOI->p[1].y = ir.y+ir.h;

	if(ir.w==0){
		if(ir.x==r0.x+r0.w) pOI->nDivision[0] = 1;
		else if(ir.x==r0.x) pOI->nDivision[0] = 3;
		else _ASSERT(FALSE);	// ��ġ�� �ʴ� ���
	}
	else if(ir.h==0){
		if(ir.y==r0.y) pOI->nDivision[0] = 0;
		else if(ir.y==r0.y+r0.h) pOI->nDivision[0] = 2;
		else _ASSERT(FALSE);	// ��ġ�� �ʴ� ���
	}
	else{
		_ASSERT(FALSE);		// �������� ������ ���
	}

	if(pOI->nDivision[0]==2 || pOI->nDivision[0]==3){	// �ð���� �������� ����
		int tx = pOI->p[0].x;
		int ty = pOI->p[0].y;
		pOI->p[0].x = pOI->p[1].x;
		pOI->p[0].y = pOI->p[1].y;
		pOI->p[1].x = tx;
		pOI->p[1].y = ty;
	}

	switch(pOI->nDivision[0]){
	case 0:
		pOI->nDivision[1] = 2;
		break;
	case 1:
		pOI->nDivision[1] = 3;
		break;
	case 2:
		pOI->nDivision[1] = 0;
		break;
	case 3:
		pOI->nDivision[1] = 1;
		break;
	}
}

/*
	   0
	+------+
	|      | 1
  3	|      |
	+------+
	    2
*/
int GetDivision(int tx, int ty, MGPRECT& r)
{
	if(ty==r.y && (tx>=r.x && tx<r.x+r.w)) return 0;				//	0 ��и�
	else if(tx==r.x+r.w-1 && (ty>=r.y && ty<r.y+r.h)) return 1;	//	1 ��и�
	else if(ty==r.y+r.h-1 && (tx>r.x && tx<=r.x+r.w)) return 2;	//	2 ��и�
	else if(tx==r.x && (ty>r.y && ty<=r.y+r.h)) return 3;		//	3 ��и�
	else{	// �� ��� tx, ty�� �簢�� �ܰ����� ������ �ʴ°��
		_ASSERT(FALSE);
		return -1;
	}
}

// Division�� ���� �� ���ϱ�
void GetDivisionStartPoint(MGPPOINT* pPoint, MGPRECT& r, int nDivision)
{
	switch(nDivision){
	case 0:
		pPoint->x = r.x;
		pPoint->y = r.y;
		break;
	case 1:
		pPoint->x = r.x+r.w;
		pPoint->y = r.y;
		break;
	case 2:
		pPoint->x = r.x+r.w;
		pPoint->y = r.y+r.h;
		break;
	case 3:
		pPoint->x = r.x;
		pPoint->y = r.y+r.h;
		break;
	default:
		_ASSERT(FALSE);	// Division�� 4����...
		break;
	}
}

// Division�� �� �� ���ϱ�
void GetDivisionEndPoint(MGPPOINT* pPoint, MGPRECT& r, int nDivision)
{
	GetDivisionStartPoint(pPoint, r, (nDivision+1)%4);
}

// ���� ������ ���� ���� �Ÿ� ������ �� ��ǥ ���ϱ�
int GetDistanceFromStartPoint(MGPPOINT& p, MGPRECT& r, int nDivision)
{
	MGPPOINT sp;
	GetDivisionStartPoint(&sp, r, nDivision);
	switch(nDivision){
	case 0:
		return p.x - sp.x;
	case 1:
		return p.y - sp.y;
	case 2:
		return sp.x - p.x;
	case 3:
		return sp.y - p.y;
	default:
		_ASSERT(FALSE);
		return 0;
	}
}

// ���� ������ ���� ������ ��(���� �����)�� �Ÿ� ���ϱ�
int GetDistanceFromPoint(MGPPOINT& p, MGPPOINT& s, int nDivision)
{
	switch(nDivision){
	case 0:
		return p.x - s.x;
	case 1:
		return p.y - s.y;
	case 2:
		return s.x - p.x;
	case 3:
		return s.y - p.y;
	default:
		_ASSERT(FALSE);
		return 0;
	}
}

// ������� ����� �Ÿ���ŭ ������ �� ���ϱ�
void GetPointOfDistance(MGPPOINT* p, MGPPOINT& s, int nDivision, int nDistance)
{
	switch(nDivision){
	case 0:
		p->x = s.x + nDistance;
		p->y = s.y;
		break;
	case 1:
		p->x = s.x;
		p->y = s.y + nDistance;
		break;
	case 2:
		p->x = s.x - nDistance;
		p->y = s.y;
		break;
	case 3:
		p->x = s.x;
		p->y = s.y - nDistance;
		break;
	default:
		_ASSERT(FALSE);
	}
}



void MGridPartition::GetCenter(float* x, float* y)
{
	*x = m_Rect.x+m_Rect.w/2.0f;
	*y = m_Rect.y+m_Rect.h/2.0f;
}

MGridPartition::MGridPartition(int x, int y, int w, int h)
{
	m_Rect.x = x;
	m_Rect.y = y;
	m_Rect.w = w;
	m_Rect.h = h;
}

MGridPartition::~MGridPartition(void)
{
}

bool MGridPartition::HasSuccessor(MGridPartition* pGP)
{
	for(int i=0; i<m_Successors.GetCount(); i++){
		MGridPartitionLink* pGPT = m_Successors.Get(i);
		if(pGPT->m_pGP==pGP) return true;
	}
	return false;
}

void MGridPartition::AddSuccessor(MGridPartition* pGP)
{
	if(HasSuccessor(pGP)==false){
		MGridPartitionLink* pGPL = new MGridPartitionLink;
		pGPL->m_pGP = pGP;
		MGPOVERLAPINFO OI;
		GetOverlapLine(&OI, m_Rect, pGP->m_Rect);
		memcpy(&(pGPL->m_Portal), OI.p, sizeof(MGPPOINT)*2);
		pGPL->m_nDivision = OI.nDivision[0];
		m_Successors.Add(pGPL);
	}
}

int MGridPartition::GetSuccessorCount(void)
{
	return m_Successors.GetCount();
}

MNodeModel* MGridPartition::GetSuccessor(int i)
{
	if(i<0 || i>=m_Successors.GetCount()) return NULL;
	return m_Successors.Get(i)->m_pGP;
}

float MGridPartition::GetSuccessorCost(MNodeModel* pSuccessor)
{
	return GetHeuristicCost(pSuccessor);
}

float MGridPartition::GetHeuristicCost(MNodeModel* pNode)
{
	float x, y;
	GetCenter(&x, &y);

	float sx, sy;
	((MGridPartition *)pNode)->GetCenter(&sx, &sy);

	return (float)sqrt(pow(x-sx, 2)+pow(y-sy, 2));
}

MGridPartitionLink* MGridPartition::GetLink(MGridPartition* pGP)
{
	for(int i=0; i<m_Successors.GetCount(); i++){
		MGridPartitionLink* pGPL = m_Successors.Get(i);
		if(pGPL->m_pGP==pGP) return pGPL;
	}
	return NULL;
}

struct MGPLINEINFO{
	MGPLINE l;
	int nDivision;
};
bool MGridPartition::GetOutLines(CMLinkedList<MGPLINE>* pLines, CMPtrList<MGridPartition>* pGPs)
{
	CMLinkedList<MGPLINEINFO> LIs;
	MGPPOINT p[2];
	GetDivisionStartPoint(&p[0], m_Rect, 0);
	for(int i=0; i<4; i++){
		GetDivisionStartPoint(&p[1], m_Rect, (i+1)%4);
		MGPLINEINFO* pLI = new MGPLINEINFO;
		memcpy(&(pLI->l), p, sizeof(MGPPOINT)*2);
		pLI->nDivision = i;
		LIs.Add(pLI);
		memcpy(&p[0], &p[1], sizeof(MGPPOINT));
	}

	for(i=0; i<pGPs->GetCount(); i++){
		MGridPartition* pGP = pGPs->Get(i);
		MGridPartitionLink* pGPL = GetLink(pGP);
		_ASSERT(pGPL!=NULL);	// Successor�� ���Ե� Partition�̾�� �Ѵ�.
		if(pGPL==NULL) return false;

		// Clipping
		for(int l=0; l<LIs.GetCount(); l++){
			MGPLINEINFO* pLI = LIs.Get(l);
			if(pLI->nDivision==pGPL->m_nDivision){
				int nDistance0 = GetDistanceFromPoint(pGPL->m_Portal.p[0], pLI->l.p[0], pLI->nDivision);
				int nDistance1 = GetDistanceFromPoint(pGPL->m_Portal.p[1], pLI->l.p[1], pLI->nDivision);
				if(nDistance0<=0){
					/*
						+--+
						++-++
						 +--+
					*/
					if(nDistance1<0){
						GetPointOfDistance(&(pLI->l.p[0]), pLI->l.p[1], pLI->nDivision, nDistance1);
					}
					/*
						+----+
						++--++
						 +--+
					*/
					else{
						LIs.Delete(l);
						l--;
					}

				}
				else{
					/*
						 +--+
						++--++
						+----+
					*/
					if(nDistance1<=0){
						MGPLINEINFO* pLI2 = new MGPLINEINFO;
						memcpy(pLI2, pLI, sizeof(MGPLINEINFO));
						GetPointOfDistance(&(pLI2->l.p[0]), pLI2->l.p[1], pLI->nDivision, nDistance1);
						LIs.Add(pLI2);
					}

					/*
						 +--+
						++-++
						+--+
					*/
					GetPointOfDistance(&(pLI->l.p[1]), pLI->l.p[0], pLI->nDivision, nDistance0);
				}
			}
		}
	}

	for(i=0; i<LIs.GetCount(); i++){
		MGPLINEINFO* pLI = LIs.Get(i);
		MGPLINE* pL = new MGPLINE;
		memcpy(pL, &pLI->l, sizeof(MGPLINE));
		pLines->Add(pL);
	}

	return true;
}



MGPMCELL* MGridPartitionModel::Get(int x, int y)
{
	if(x<0 || x>=m_nWidth) return NULL;
	if(y<0 || y>=m_nHeight) return NULL;
	return &(m_pCells[x+y*m_nWidth]);
}

bool MGridPartitionModel::IsEmptyRangeWidth(int x, int y, int w)
{
	for(int i=0; i<w; i++){
		MGPMCELL* pCell = Get(x+i, y);
		if(pCell->bEnable==false || pCell->pPart!=NULL) return false;
	}

	return true;
}

bool MGridPartitionModel::IsEmptyRangeHeight(int x, int y, int h)
{
	for(int i=0; i<h; i++){
		MGPMCELL* pCell = Get(x, y+i);
		if(pCell->bEnable==false || pCell->pPart!=NULL) return false;
	}

	return true;
}

void MGridPartitionModel::GetEmptyRangeWidthFirst(int* w, int* h, int x, int y)
{
	int mw = 0;
	int mh = 0;

	for(int ix=x; ix<m_nWidth; ix++){
		MGPMCELL* pCell = Get(ix, y);
		if(pCell->bEnable==true && pCell->pPart==NULL) mw++;	// ������ �����̸�..
		else break;
	}

	if(mw>0) mh = 1;

	for(int iy=y+1; iy<m_nHeight; iy++){
		if(IsEmptyRangeWidth(x, iy, mw)==true) mh++;
		else break;
	}

	_ASSERT(mw!=0);
	_ASSERT(mh!=0);

	*w = mw;
	*h = mh;
}

void MGridPartitionModel::GetEmptyRangeHeightFirst(int* w, int* h, int x, int y)
{
	int mw = 0;
	int mh = 0;

	for(int iy=y; iy<m_nHeight; iy++){
		MGPMCELL* pCell = Get(x, iy);
		if(pCell->bEnable==true && pCell->pPart==NULL) mh++;	// ������ �����̸�..
		else break;
	}

	if(mh>0) mw = 1;

	for(int ix=x+1; ix<m_nWidth; ix++){
		if(IsEmptyRangeHeight(ix, y, mh)==true) mw++;
		else break;
	}

	_ASSERT(mw!=0);
	_ASSERT(mh!=0);

	*w = mw;
	*h = mh;
}

int MGridPartitionModel::GetEmptySquareSideLength(int x, int y)
{
	int w = m_nWidth - x;
	int h = m_nHeight - y;
	int l = min(w, h);

	int el = 0;

	for(int i=0; i<l; i++){
		bool bEnable = true;
		for(int xx=0; xx<=i && bEnable==true; xx++){
			MGPMCELL* pCell = Get(x+xx, y+i);
			if(!(pCell->bEnable==true && pCell->pPart==NULL)) bEnable = false;
		}
		for(int yy=0; yy<=i && bEnable==true; yy++){
			MGPMCELL* pCell = Get(x+i, y+yy);
			if(!(pCell->bEnable==true && pCell->pPart==NULL)) bEnable = false;
		}
		if(bEnable==true) el++;	// ������ �����̸�..
		else break;
	}

	return el;
}

void MGridPartitionModel::FillPartition(int x, int y, int w, int h, MGridPartition* pGP)
{
	for(int iy=0; iy<h; iy++){
		for(int ix=0; ix<w; ix++){
			MGPMCELL* pCell = Get(x+ix, y+iy);
			_ASSERT(pCell->bEnable==true);
			_ASSERT(pCell->pPart==NULL);
			pCell->pPart = pGP;
		}
	}
}


MGridPartitionModel::MGridPartitionModel(void)
{
	m_pCells = NULL;
	m_nWidth = m_nHeight = 0;
}

MGridPartitionModel::~MGridPartitionModel(void)
{
	Destroy();
}

bool MGridPartitionModel::Create(int x, int y)
{
	Destroy();

	m_nWidth = x;
	m_nHeight = y;
	m_pCells = new MGPMCELL[x*y];

	for(int i=0; i<x*y; i++){
		m_pCells[i].bEnable = true;
		m_pCells[i].pPart = NULL;
	}

	return true;
}

void MGridPartitionModel::Destroy(void)
{
	if(m_pCells!=NULL){
		delete[] m_pCells;
		m_pCells = NULL;
	}
	m_Parts.DeleteAll();
	m_nWidth = m_nHeight = 0;
}

void MGridPartitionModel::Enable(int x, int y, bool bEnable)
{
	m_pCells[x+y*m_nWidth].bEnable = bEnable;
}

void MGridPartitionModel::Partition(void)
{
	// Partitioning
	for(int y=0; y<m_nHeight; y++){
		for(int x=0; x<m_nWidth; x++){
			MGPMCELL* pCell = Get(x, y);
			if(pCell->bEnable==true && pCell->pPart==NULL){	// �̼��� �� �� �ְ�, ���� ��Ƽ���� ������ ���� ���� ���
				int w = GetEmptySquareSideLength(x, y);
				MGridPartition* pGP = new MGridPartition(x, y, w, w);
				m_Parts.Add(pGP);
				FillPartition(x, y, w, w, pGP);
				/*
				int w0, h0, w1, h1, w, h;
				GetEmptyRangeWidthFirst(&w0, &h0, x, y);
				GetEmptyRangeHeightFirst(&w1, &h1, x, y);
				if(w0*h0>=w1*h1){	// ���� ������ �����Ѵ�.
					w = w0, h = h0;
				}
				else{
					w = w1, h = h1;
				}
				MGridPartition* pGP = new MGridPartition(x, y, w, h);
				m_Parts.Add(pGP);
				FillPartition(x, y, w, h, pGP);
				*/
			}
		}
	}

	// �����ִ� �ٸ� ���� ��ũ
	for(int i=0; i<m_Parts.GetCount(); i++){
		MGridPartition* pGP = m_Parts.Get(i);
		// �� Partition�� �� �Ʒ�
		for(int x=0; x<pGP->m_Rect.w; x++){
			MGPMCELL* pCell = Get(pGP->m_Rect.x+x, pGP->m_Rect.y-1);
			if(pCell!=NULL && pCell->bEnable==true){
				_ASSERT(pCell->pPart);	// ��� Partition���� ���� �־�� �Ѵ�.
				pGP->AddSuccessor(pCell->pPart);
			}
			pCell = Get(pGP->m_Rect.x+x, pGP->m_Rect.y+pGP->m_Rect.h);
			if(pCell!=NULL && pCell->bEnable==true){
				_ASSERT(pCell->pPart);	// ��� Partition���� ���� �־�� �Ѵ�.
				pGP->AddSuccessor(pCell->pPart);
			}
		}
		// �� Partition�� �� ��
		for(int y=0; y<pGP->m_Rect.h; y++){
			MGPMCELL* pCell = Get(pGP->m_Rect.x-1, pGP->m_Rect.y+y);
			if(pCell!=NULL && pCell->bEnable==true){
				_ASSERT(pCell->pPart);	// ��� Partition���� ���� �־�� �Ѵ�.
				pGP->AddSuccessor(pCell->pPart);
			}
			pCell = Get(pGP->m_Rect.x+pGP->m_Rect.w, pGP->m_Rect.y+y);
			if(pCell!=NULL && pCell->bEnable==true){
				_ASSERT(pCell->pPart);	// ��� Partition���� ���� �־�� �Ѵ�.
				pGP->AddSuccessor(pCell->pPart);
			}
		}
	}
}

/*
struct MGPNEARPOINTINFO{
	MGPPOINT	p;
	int			nPathInc;
	int			nDivisionInc;
};

void GetNearPoint(MGPNEARPOINTINFO* pNPI, MGridPartition* pGP, MGridPartition* pPGP, MGridPartition* pNGP, MGPPOINT& p, int nDivision)
{
	MGPOVERLAPINFO POI, NOI;
	if(pPGP!=NULL) GetOverlapLine(&POI, pGP->m_Rect, pPGP->m_Rect);
	if(pNGP!=NULL) GetOverlapLine(&NOI, pGP->m_Rect, pNGP->m_Rect);

	CMLinkedList<MGPPOINT> pl;
	MGPPOINT* pPoint = new MGPPOINT;
	GetDivisionEndPoint(pPoint, pGP->m_Rect, nDivision);
	pl.Add(pPoint);

	MGPPOINT sp, psp, nsp;
	GetDivsisionStartPoint(&sp, pGP->m_Rect, nDivision);
	if(nDivision==POI.nDivision[0]){
		GetDivsisionStartPoint(&psp, pPGP->m_Rect, POI.nDivision[1]);
		int nDistance = GetDistanceFromStartPoint(psp, pGP->m_Rect, nDivision);
		if(nDistance>=0){	// �������� ���η�����
			MGPPOINT* pPoint = new MGPPOINT;
			pPoint->x = POI.p[0].x;
			pPoint->y = POI.p[0].y;
			pl.Add(pPoint);
		}
		else{	// �ٱ������� ���η�����
			MGPPOINT* pPoint = new MGPPOINT;
			pPoint->x = POI.p[0].x;
			pPoint->y = POI.p[0].y;
			pl.Add(pPoint);
		}
		MGPPOINT* pPoint = new MGPPOINT;
		pPoint->x = POI.p[0
		pl.Add(
		|| nDivision==POI.nDivision[1]){
	}
	if(nDivision==NOI.nDivision[0]){
		GetDivsisionStartPoint(&nsp, pNGP->m_Rect, nDivision);
	}
}
*/

/*
void GenerateOutLine(CMLinkedList<MGPPOINT>* pPolygon, MAStar* pPF, int nLimitPath)
{
	int nCurrPath = 0;
	int nPathCount = pPF->GetShortestPathCount();
	if(nPathCount==0) return;

	int nEndPath = nPathCount;
	if(nLimitPath!=-1) nEndPath = min(nLimitPath, nPathCount);
	//MGPPOINT StartPoint;
	MGridPartition* pGP = (MGridPartition *)pPF->GetShortestPath(nCurrPath);
	MGPPOINT* pPoint = new MGPPOINT;
	pPoint->x = pGP->m_Rect.x;
	pPoint->y = pGP->m_Rect.y;
	pPolygon->Add(pPoint);
	int nDivision = 0;	// ���� Division
	MGPPOINT* pPrevPoint = NULL;

	while(1){
		// Get Next Point ( �ð� ����, Portal ��� )
		////////////////////////////////////////////////////////

		// �� ��и����� ���ϱ�.
		// �����Ǵ� �������� ���� ��Ż�� ��� �ִ��� ���ϱ�.
		// ���� ����Ʈ ���� ���ο� ��Ż�� ������.. �ű���� ���ϰ� ���� ��Ƽ������..
		//int nDivision = GetDivision(pPoint->x, pPoint->y, pGP->m_Rect);
		MGridPartition* pGP = (MGridPartition *)pPF->GetShortestPath(nCurrPath);
		MGridPartition* pNGP = NULL;	// ���� �н�
		MGridPartition* pPGP = NULL;	// ���� �н�
		MGPOVERLAPINFO	NOI, POI;
		if(nCurrPath+1<nEndPath){
			pNGP = (MGridPartition *)pPF->GetShortestPath(nCurrPath+1);
			GetOverlapLine(&NOI, pGP->m_Rect, pNGP->m_Rect);
		}
		if(nCurrPath-1>=0){
			pPGP = (MGridPartition *)pPF->GetShortestPath(nCurrPath-1);
			GetOverlapLine(&POI, pGP->m_Rect, pPGP->m_Rect);
		}
		MGPPOINT* pPoint = new MGPPOINT;

		if((pNGP!=NULL && NOI.nDivision[0]==nDivision) ||
			(pPGP!=NULL && POI.nDivision[0]==nDivision)){	// �ΰ��� �ɸ����
		}
		else{
		}

		if((pNGP!=NULL && NOI.nDivision[0]==nDivision) &&
			(pPGP!=NULL && POI.nDivision[0]==nDivision)){	// �ΰ��� �ɸ����
			int nNear = 0;
			switch(nDivision){
			case 0:
				if(NOI.p[0].x < NOI.p[0].x) nNear = 0;
				else nNear = 1;
				break;
			case 1:
				if(NOI.p[0].y < NOI.p[0].y) nNear = 0;
				else nNear = 1;
				break;
			case 2:
				if(NOI.p[0].x > NOI.p[0].x) nNear = 0;
				else nNear = 1;
				break;
			case 3:
				if(NOI.p[0].y > NOI.p[0].y) nNear = 0;
				else nNear = 1;
				break;
			}
			if(nNear==0){
				pPoint->x = NOI.p[0].x;
				pPoint->y = NOI.p[0].y;
				nCurrPath++;
			}
			else{
				pPoint->x = POI.p[0].x;
				pPoint->y = POI.p[0].y;
				nCurrPath--;
			}
			nDivision = (nDivision+3)%4;	// ���� Division�� ����
			pPrevPoint->x
		}
		else if(pNGP!=NULL && NOI.nDivision[0]==nDivision){
			pPoint->x = NOI.p[0].x;
			pPoint->y = NOI.p[0].y;
			nCurrPath++;
			nDivision = (nDivision+2)%4;	// ���� Division�� ����
		}
		else if(pPGP!=NULL && POI.nDivision[0]==nDivision){
			pPoint->x = POI.p[0].x;
			pPoint->y = POI.p[0].y;
			nCurrPath--;
			nDivision = (nDivision+2)%4;	// ���� Division�� ����
		}
		else{	// �ƹ� Portal���� �ɸ��� ���� ���
			GetDivisionEndPoint(pPoint, pGP->m_Rect, nDivision);
			nDivision = (nDivision+1)%4;	// ���� Division�� ����
		}

		if(pPrevPoint!=NULL && (pPrevPoint->x==pPoint->x && pPrevPoint->y==pPoint->y)){
			nDivision = (nDivision+3)%4;
			delete pPoint;
		}
		else
			pPolygon->Add(pPoint);

		pPrevPoint = pPoint;

		if(nCurrPath==0 && nDivision==0) break;	// ���� ó�� �����ͷ� �ٽ� ���ƿ��� ��!

		/*
		MGridPartition* pGP = (MGridPartition *)pPF->GetShortestPath(nCurrPath);
		MGridPartition* pNGP = NULL;	// ���� �н�
		MGridPartition* pPGP = NULL;	// ���� �н�
		MGPOVERLAPINFO	NOI, POI;
		if(nCurrPath+1<nEndPath){
			pNGP = (MGridPartition *)pPF->GetShortestPath(nCurrPath+1);
			GetOverlapLine(&NOI, pGP->m_Rect, pNGP->m_Rect);
		}
		if(nCurrPath-1>=0){
			pPGP = (MGridPartition *)pPF->GetShortestPath(nCurrPath-1);
			GetOverlapLine(&POI, pGP->m_Rect, pPGP->m_Rect);
		}

		MGPPOINT* pPoint = new MGPPOINT;
		if(pNGP!=NULL && NOI.nDivision[0]==nDivision){	// Next Portal�� �ɸ� ���
			MGPPOINT sp;
			GetDivisionStartPoint(&sp, pGP->m_Rect, nDivision);
			if(NOI.p[0].x==sp.x && NOI.p[0].y==sp.y){
				nDivision = (nDivision+2)%4;	// ���� �н� Division�� ����
				GetDivisionEndPoint(pPoint, pNGP->m_Rect, nDivision);
				if(!(pPoint->x==sp.x && pPoint->y==sp.y))
					pPolygon->Add(pPoint);
				else
					delete pPoint;
				nDivision = (nDivision+1)%4;	// ���� Division�� ����

				nCurrPath++;

				if(nCurrPath==0 && nDivision==0) break;	// ���� ó�� �����ͷ� �ٽ� ���ƿ��� ��!
				continue;
			}
			pPoint->x = NOI.p[0].x;
			pPoint->y = NOI.p[0].y;
			nCurrPath++;
			nDivision = (nDivision+3)%4;	// ���� �н� Division�� ����
		}
		else if(pPGP!=NULL && POI.nDivision[0]==nDivision){	// Previous Portal�� �ɸ� ���
			MGPPOINT sp;
			GetDivisionStartPoint(&sp, pGP->m_Rect, nDivision);
			if(POI.p[0].x==sp.x && POI.p[0].y==sp.y){
				nDivision = (nDivision+2)%4;	// ���� �н� Division�� ����
				GetDivisionEndPoint(pPoint, pPGP->m_Rect, nDivision);
				if(!(pPoint->x==sp.x && pPoint->y==sp.y))
					pPolygon->Add(pPoint);
				else
					delete pPoint;
				nDivision = (nDivision+1)%4;	// ���� Division�� ����

				nCurrPath--;

				if(nCurrPath==0 && nDivision==0) break;	// ���� ó�� �����ͷ� �ٽ� ���ƿ��� ��!
				continue;
			}
			pPoint->x = POI.p[0].x;
			pPoint->y = POI.p[0].y;
			nCurrPath--;
			nDivision = (nDivision+3)%4;	 // ���� �н� Division�� ����
		}
		else{	// �ƹ� Portal���� �ɸ��� ���� ���
			GetDivisionEndPoint(pPoint, pGP->m_Rect, nDivision);
			nDivision = (nDivision+1)%4;	// ���� Division�� ����
		}

		pPolygon->Add(pPoint);
		if(nCurrPath==0 && nDivision==0) break;	// ���� ó�� �����ͷ� �ٽ� ���ƿ��� ��!
		/////////


		if(pPolygon->GetCount()>1000) break;	// Debug...
	}
}
*/

void GenerateOutLine(CMLinkedList<MGPLINE>* pLines, MAStar* pPF, int nLimitPath)
{
	int nPathCount = pPF->GetShortestPathCount();
	if(nPathCount==0) return;

	int nEndPath = nPathCount;
	if(nLimitPath!=-1) nEndPath = min(nLimitPath, nPathCount);

	MGridPartition* pGP = (MGridPartition *)pPF->GetShortestPath(0);

	for(int i=0; i<nEndPath; i++){
		MGridPartition* pNGP = NULL;
		MGridPartition* pPGP = NULL;
		MGridPartition* pGP = (MGridPartition *)pPF->GetShortestPath(i);

		CMPtrList<MGridPartition> GPs;
		if(i+1<=nEndPath && i+1<nPathCount){	// Limit�� �ɷ����� ��Ż�� ����д�.
			MGridPartition* pNGP = (MGridPartition *)pPF->GetShortestPath(i+1);
			GPs.Add(pNGP);
		}
		if(i-1>=0){
			MGridPartition* pPGP = (MGridPartition *)pPF->GetShortestPath(i-1);
			GPs.Add(pPGP);
		}

		
		pGP->GetOutLines(pLines, &GPs);

	}
}

