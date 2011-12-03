#include "stdafx.h"
#include <algorithm>
#include "ZGame.h"
#include "ZCharacterManager.h"
#include "ZMyCharacter.h"
#include "ZNetCharacter.h"
#include "MDebug.h"
#include "ZApplication.h"
#include "ZShadow.h"
#include "RCharCloth.h"
#include "ZObjectManager.h"

extern bool Enable_Cloth;

ZCharacterManager::ZCharacterManager()
{
//	m_nRenderedCnt = 0;
//	m_nOnDrawCnt = 0;
}

ZCharacterManager::~ZCharacterManager()
{
	Clear();
}

void ZCharacterManager::Add(ZCharacter *pCharacter)
{	
	insert(value_type(pCharacter->GetUID(), pCharacter));
	ZGetObjectManager()->Add((ZObject*)pCharacter);
}

ZCharacter* ZCharacterManager::Add(MUID uid, rvector pos, bool bMyCharacter)
{	
	ZCharacter* pCharacter = Find(uid);
	if (pCharacter != NULL) return pCharacter;

	//pCharacter = new ZNetCharacter;
	if (bMyCharacter) pCharacter = new ZMyCharacter;
	else pCharacter = new ZNetCharacter;

	pCharacter->SetUID(uid);
	//pCharacter->m_Position= pos;
	pCharacter->SetPosition(pos);

	Add(pCharacter);

	return pCharacter;
}

void ZCharacterManager::Delete(MUID uid)
{
	iterator itor = find(uid);

	if (itor != end()) {

		ZCharacter* pCharacter = (*itor).second;

		ZGetObjectManager()->Delete((ZObject*)pCharacter);

		ZGetGame()->m_VisualMeshMgr.Del(pCharacter->m_nVMID.Ref());
		delete pCharacter; pCharacter = NULL;
		erase(itor);
	}
}


void ZCharacterManager::Clear()
{
	while (!empty())
	{
		ZCharacter* pCharacter = (*begin()).second;

		if( ZGetGame()->m_VisualMeshMgr.m_list.size() )//������ ����ô�..
			ZGetGame()->m_VisualMeshMgr.Del(pCharacter->m_nVMID.Ref());
		delete pCharacter;
		erase(begin());
	}

}


ZCharacter* ZCharacterManager::Find(MUID uid)
{
	iterator itor = find(uid);
	if (itor != end())
	{
		CHECK_RETURN_CALLSTACK(Find);
		return (*itor).second;
	}
	
	CHECK_RETURN_CALLSTACK(Find);
	return NULL;
}

/*
void ZCharacterManager::Update(float fElapsed)
{
	if(empty()) return;

	for (iterator itor = begin(); itor != end(); )
	{
		ZCharacter* pCharacter = (*itor).second;
		pCharacter->Update(fElapsed);


		//ĳ���� Ǯ���� �ű��..
		++itor;
	}
}
*/

/*
void ZCharacterManager::DrawMyCharacter()
{
	ZCharacter* pMyCharacter = g_pGame->m_pMyCharacter;

	if(pMyCharacter) {
		if (pMyCharacter->IsVisible()) {//��� �׻� ���δ�..
			pMyCharacter->Draw();
		}
	}
}
*/

/*
void ZCharacterManager::Draw()
{
//	RMeshRenderSBegin();

	ZCharacter* pMyCharacter = g_pGame->m_pMyCharacter;

	list<ZCharacter*> visibleCharacterList;

	m_nRenderedCnt = 0;//�ڽ����׻�~
	m_nOnDrawCnt = 0;

	for (iterator itor = begin(); itor != end(); ) {
		
		ZCharacter* pCharacter = (*itor).second;

		if(pCharacter){

			if(pCharacter!=pMyCharacter) {
			
			bool bRendered=false;
		
			if (pCharacter->IsVisible() && pCharacter->GetInitialized()) {

				RealSpace2::rboundingbox bb;
				bb.vmax=pCharacter->GetPosition()+rvector(50,50,190);
				bb.vmin=pCharacter->GetPosition()-rvector(50,50,0);
				if(ZGetGame()->GetWorld()->GetBsp()->IsVisible(bb))
				{
					// ĳ���� �߽����� ���� ���� - �ӵ�������
//					rvector p = pCharacter->GetPosition();
//					p.z += 80.0f;
//					if(isInViewFrustum(p, 150.0f, RGetViewFrustum())==true) 
					if(isInViewFrustum(&bb, RGetViewFrustum())==true) 
					{
						m_nOnDrawCnt++;

						pCharacter->Draw();
						bRendered=pCharacter->GetVisualMesh()->m_bIsRender;

						if (bRendered)
						{
							visibleCharacterList.push_back(pCharacter);
							m_nRenderedCnt++;
						}
					}
				}

			}

			pCharacter->m_bRendered = bRendered;

			}
		}
		++itor;
	}

	//�ڽ��� ������������ �־ ���߿� �׸���..	

	if(pMyCharacter) {

		bool bRendered=false;

		// ������忡�� �����������־...

		if (pMyCharacter->IsVisible() && pMyCharacter->GetInitialized()) {

			RealSpace2::rboundingbox bb;
			bb.vmax = pMyCharacter->GetPosition()+rvector(50,50,190);
			bb.vmin = pMyCharacter->GetPosition()-rvector(50,50,0);
			if(ZGetGame()->GetWorld()->GetBsp()->IsVisible(bb))
			{
				if(isInViewFrustum(&bb, RGetViewFrustum())==true) 
				{
					m_nOnDrawCnt++;
					pMyCharacter->Draw();

					bRendered = pMyCharacter->GetVisualMesh()->m_bIsRender;

					if (bRendered)
					{
						visibleCharacterList.push_back(pMyCharacter);
						m_nRenderedCnt++;
					}
					pMyCharacter->m_bRendered = bRendered;
				}
			}
		}
	}

	// �׸��� ������
	ZShadow::predraw();
	for (list<ZCharacter*>::iterator vcitor = visibleCharacterList.begin(); vcitor != visibleCharacterList.end(); ++vcitor)
	{
		(*vcitor)->DrawShadow();
	}
	ZShadow::postdraw();

	visibleCharacterList.clear();

//	RMeshRenderSEnd();

}
*/

//

/*
ZCharacter* ZCharacterManager::Pick(int x,int y,RPickInfo* pInfo)
{
	ZCharacter* pPickCharacter = NULL;
	float best_dist = 10000.f;
	RPickInfo PickInfo;

	ZCharacter* pCharacter;

	for (iterator itor = begin(); itor != end(); ) {

		pCharacter = (*itor).second;

		if(pCharacter->Pick(x,y,&PickInfo)) {
			if(PickInfo.t < best_dist) {
				pPickCharacter = pCharacter;
				*pInfo = PickInfo;
				best_dist = PickInfo.t;
			}
		}
		++itor;
	}
	return pPickCharacter;
}

ZCharacter* ZCharacterManager::Pick(ZCharacter* pMyChar,rvector& pos,rvector& dir, RPickInfo* pInfo)
{
	ZCharacter* pPickCharacter = NULL;
	float best_dist = 10000.f;
	RPickInfo PickInfo;

	memset(&PickInfo,0,sizeof(RPickInfo));

	ZCharacter* pCharacter;

	for (iterator itor = begin(); itor != end(); ++itor) {

		pCharacter = (*itor).second;

		if(pCharacter != pMyChar) {

			if(pCharacter->Pick(pos, dir, &PickInfo)) {

				if(PickInfo.t < best_dist) {
					pPickCharacter = pCharacter;
					*pInfo = PickInfo;
					best_dist = PickInfo.t;
				}
			}

		}
	}
	return pPickCharacter;
}

ZCharacter* ZCharacterManager::Pick(rvector& pos,rvector& dir, RPickInfo* pInfo)
{
	ZCharacter* pPickCharacter = NULL;
	float best_dist = 10000.f;
	RPickInfo PickInfo;

	for (iterator itor = begin(); itor != end(); ++itor) {

		ZCharacter* pCharacter = (*itor).second;

		if(pCharacter->Pick(pos, dir, &PickInfo)) {

			if(PickInfo.t < best_dist) {
				pPickCharacter = pCharacter;
				*pInfo = PickInfo;
				best_dist = PickInfo.t;
			}
		}
	}
	return pPickCharacter;
}

ZCharacter* ZCharacterManager::Pick( rvector& pos, float Radius )
{
	ZCharacter* pPickCharacter = NULL;
	ZCharacter* pCharacter = NULL;
	float best_dist = Radius;

	for( iterator iter = begin(); iter != end(); ++iter )
	{
		pCharacter	= iter->second;

		float temp =  D3DXVec3Length(&(pCharacter->GetPosition() - pos));

		if( D3DXVec3Length(&(pCharacter->GetPosition() - pos)) < best_dist )
		{
			pPickCharacter	= pCharacter;
		}
	}
	return pPickCharacter;
}
*/

void ZCharacterManager::OutputDebugString_CharacterState()
{
	ZMyCharacter* pMyChar = ZGetGame()->m_pMyCharacter;

	if(pMyChar) {
		pMyChar->OutputDebugString_CharacterState();
	}

	for(iterator i = begin(); i!=end(); i++) {
		ZCharacter* pCharacter = (*i).second;
		if(pCharacter&& (pCharacter!=pMyChar)) {
			pCharacter->OutputDebugString_CharacterState();
		}
	}
}

int ZCharacterManager::GetLiveCount()
{
	int nLiveCount = 0;
	for(iterator i = begin(); i!=end(); i++){
		ZCharacter* pCharacter = (*i).second;
		if(pCharacter->IsDie()==false) nLiveCount++;
	}

	return nLiveCount;
}

bool ZCharacterManager::ToggleClothSimulation()
{
	Enable_Cloth	= !Enable_Cloth;
	for( iterator i = begin(); i != end(); ++i )
	{
		ZCharacter* pCharacter = (*i).second ;
		if( pCharacter != 0 )
			pCharacter->ToggleClothSimulation();
	}
	return Enable_Cloth;
}

void ZCharacterManager::OnInvalidate()
{
	ZCharacter* pCharacter;
	
	for(iterator i = begin(); i!=end(); i++) {
		pCharacter = i->second;
		if( pCharacter && pCharacter->m_pVMesh ) 
			pCharacter->m_pVMesh->OnInvalidate();
	}
}

void ZCharacterManager::OnRestore()
{
	ZCharacter* pCharacter;
	
	for(iterator i = begin(); i!=end(); i++) {
		pCharacter = i->second;
		if( pCharacter && pCharacter->m_pVMesh ) 
			pCharacter->m_pVMesh->OnRestore();
	}
}

ZCharacter* ZCharacterManager::Get(int index)
{
	int nIndex = 0;

	for(iterator i = begin(); i!=end(); i++,nIndex++) {
		if(nIndex==index) return i->second;
	}

	return NULL;
}

void ZCharacterManager::InitRound()
{
	for(iterator i = begin(); i!=end(); i++) {
		ZCharacter* pCharacter = i->second;
		pCharacter->InitRound();
	}
}

int ZCharacterManager::GetCharacterIndex(MUID& uid, bool bIncludeHiddenChar)
{
	int nIndex = 0;
	for(iterator i = begin(); i!=end(); i++) 
	{
		if (i->second->GetUID() == uid)
		{
			return nIndex;
		}
		if ((bIncludeHiddenChar) || (!i->second->IsAdminHide())) nIndex++;
	}

/*
	iterator itor = find(uid);
	if (itor != end())
	{
		return distance(begin(), itor);
	}
*/
	return -1;
}