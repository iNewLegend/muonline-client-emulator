#include "stdafx.h"

#include "ZGame.h"
#include "ZWeaponMgr.h"
#include "RealSpace2.h"
#include "ZCharacter.h"
#include "ZApplication.h"

#include "ZEffectBillboard.h"
#include "ZEffectSmoke.h"

enum el_plane_type {
	el_plane_yz,//x
	el_plane_xz,//y
	el_plane_xy,//z
};

el_plane_type GetFaceType(rplane& p) {

	el_plane_type mode = el_plane_yz;

	float max = fabs(p.a);//x

	if( fabs(p.b) > max ) { //y
		max = fabs(p.b);
		mode = el_plane_xz;
	}

	if( fabs(p.c) > max ) {//z
		mode = el_plane_xy;
	}

	return mode;
}

////////////////////////////////////////////
// ZWeaponMgr

ZWeaponMgr::ZWeaponMgr() {
	m_fLastTime = 0;
	m_bEnableRender = true;
}

ZWeaponMgr::~ZWeaponMgr()
{
	Clear();
}

void ZWeaponMgr::Clear()
{
	if(m_list.size()==0)
		return;

	z_weapon_node node;
	ZWeapon* pWeapon;

	for(node = m_list.begin(); node != m_list.end(); ) {
		pWeapon = (*node);
		delete pWeapon;
		node=m_list.erase(node);
	}
}

void ZWeaponMgr::AddGrenade(rvector &pos,rvector &velocity,ZObject* pC)
{
	ZWeaponGrenade* pWeapon = new ZWeaponGrenade;

	RMesh* pMesh = ZGetWeaponMeshMgr()->Get("grenade01");												 
	if(!pMesh) return;

	pWeapon->Create(pMesh,pos,velocity,pC);

	Add(pWeapon);
}

void ZWeaponMgr::AddKit(rvector &pos, rvector &velocity, ZCharacter* pC, float Delaytime, char *szMeshName, int nLinkedWorldItemID)
{
	ZWeaponItemKit* pWeapon = new ZWeaponItemKit;

	RMesh* pMesh = ZGetWeaponMeshMgr()->Get(szMeshName);
	if(!pMesh) return;

	pWeapon->Create(pMesh,pos, velocity, pC);
	pWeapon->m_nWorldItemID = nLinkedWorldItemID;
	pWeapon->m_fDelayTime = Delaytime;

	Add(pWeapon);
}

void ZWeaponMgr::AddFlashBang( rvector &pos, rvector &velocity, ZObject* pC )
{
	ZWeaponFlashBang* pWeapon	= new ZWeaponFlashBang;

	RMesh* pMesh	= ZGetWeaponMeshMgr()->Get("flashbang01");
	if( !pMesh ) return;

	pWeapon->Create( pMesh, pos, velocity, pC );
	Add( pWeapon );
}
//*/
void ZWeaponMgr::AddSmokeGrenade( rvector &pos,rvector &velocity,ZObject* pC )
{
	ZWeaponSmokeGrenade* pWeapon	= new ZWeaponSmokeGrenade;

	RMesh* pMesh	= ZGetWeaponMeshMgr()->Get("smoke01");

	if( !pMesh )
	{
		return;
	}

	pWeapon->Create( pMesh, pos, velocity, pC );
	Add( pWeapon );
}

void ZWeaponMgr::AddRocket(rvector &pos,rvector &dir,ZObject* pC)
{
	ZWeaponRocket* pWeapon = new ZWeaponRocket;

	RMesh* pMesh = ZGetWeaponMeshMgr()->Get("rocket");
	if(!pMesh) return;

	pWeapon->Create(pMesh, pos, dir, pC);
	Add(pWeapon);
}

static char* GetTrapMeshName(int nItemId)
{
	MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemId);
	if (!pDesc) return NULL;

	switch (pDesc->m_nDamageType.Ref())
	{
	case MMDT_FIRE:		return "trap_firefield";
	case MMDT_COLD:		return "trap_frozenfield";
	}
	_ASSERT(0);
	return NULL;
}

void ZWeaponMgr::AddTrap(rvector &pos,rvector &velocity,int nItemId,ZObject* pC)
{
	ZWeaponTrap* pWeapon = new ZWeaponTrap;

	char* szMeshName = GetTrapMeshName(nItemId);
	if (!szMeshName) return;

	RMesh* pMesh = ZGetWeaponMeshMgr()->Get(szMeshName);
	if(!pMesh) return;

	if (pWeapon->Create(pMesh, pos, velocity, nItemId, pC))
		Add(pWeapon);
	else
		delete pWeapon;
}

void ZWeaponMgr::AddTrapAlreadyActivated(rvector& pos, float fActivatedTime, int nItemId, ZObject* pOwner)
{
	ZWeaponTrap* pWeapon = new ZWeaponTrap;
	
	char* szMeshName = GetTrapMeshName(nItemId);
	if (!szMeshName) return;

	RMesh* pMesh = ZGetWeaponMeshMgr()->Get(szMeshName);
	if(!pMesh) return;

	if (pWeapon->CreateActivated(pMesh, pos, fActivatedTime, nItemId, pOwner))
		Add(pWeapon);
	else
		delete pWeapon;
}

void ZWeaponMgr::AddDynamite(rvector &pos, rvector &velocity, ZObject* pC)
{
	ZWeaponDynamite* pWeapon = new ZWeaponDynamite;

	RMesh* pMesh = ZGetWeaponMeshMgr()->Get("dynamite");												 
	if(!pMesh) return;

	pWeapon->Create(pMesh, pos, velocity, pC);
	Add(pWeapon);
}

///////////////////////////////////////////////////////////////////////////////

void ZWeaponMgr::Render()
{
	if(!m_bEnableRender) return;
	if(m_list.size() <= 0) return;

	z_weapon_node node,node2;
	ZWeapon* pWeapon;

	for(node = m_list.begin(); node != m_list.end(); node++) {

		pWeapon = (*node);
		pWeapon->Render();
	}
}

void ZWeaponMgr::Update()
{
	float fTime=ZGetGame()->GetTime();

	if(	m_fLastTime==0 )	// �ʱ�ȭ �ȵǾ���.
		m_fLastTime=fTime;

	float fElapsedTime=fTime-m_fLastTime;
	m_fLastTime=fTime;

	z_weapon_node node;
	ZWeapon* pWeapon;

	for(node = m_list.begin(); node != m_list.end(); ) {

		pWeapon = (*node);

		if( !pWeapon->Update(fElapsedTime) ) {
			delete pWeapon;
			node=m_list.erase(node);
		}
		else ++node;
	}
}

bool ZWeaponMgr::SamePoint(rvector& p1,rvector& p2)
{
	if(p1.x == p2.x && p1.y == p2.y && p1.z == p2.z)
		return true;

	return false;
}

ZWeapon* ZWeaponMgr::GetWorldItem(int nItemID)
{
	z_weapon_node	node;

	ZWeapon* pWeapon = NULL;
	ZMovingWeapon*	pMWeapon = NULL;

	for(node = m_list.begin(); node != m_list.end(); ) {

		pWeapon = (*node);
//		pMWeapon = MDynamicCast( ZMovingWeapon, pWeapon );

		if(pWeapon->GetItemUID()==nItemID ) {
			return pWeapon;
		}

		++node;
	}

	return NULL;
}

ZMovingWeapon* ZWeaponMgr::UpdateWorldItem(int nItemID,rvector& pos)
{
	z_weapon_node	node;

	ZWeapon* pWeapon;
	ZMovingWeapon*	pMWeapon = NULL;
	ZMovingWeapon*	pMWeaponResult = NULL;

	float _min_weapon = 9999.f;

	for(node = m_list.begin(); node != m_list.end(); ) {

		pWeapon = (*node);

		pMWeapon = MDynamicCast( ZMovingWeapon, pWeapon );

		if(pMWeapon) {
			// pMWeapon->GetItemUID ���� -1�̸� ������ �ư� ���� �������� �Ҵ�� ��(UID)�� ������ �ȵƱ⶧����
			// ������ �ȵȰ͵� �߿��� ���� ����� �������� ã�� ���ǰ����� �������ش�
			// �������� ������ ������ ����ID�� �Բ������� �ٽ� ������ �װ��� �޾� ��ġ�Ǵ°�
			// ���� ���������� ��������� ������.......20090218 by kammir
			if(pMWeapon->GetItemUID() == -1)
//			if(pMWeapon->m_nWorldItemID == nItemID) 
			{
/*
				if( SamePoint(pMWeapon->m_Position,pos) ) {
					return pMWeapon;
				}
				// ���� ����� �������� �ڽ��ǰ�..���߿� ���� ��ġ��.. ID ���� �༭ �ĺ�..
*/				
				float fLen = Magnitude(pMWeapon->m_Position - pos);
				
				if(fLen < 100.f) {
					if(_min_weapon > fLen) {
						_min_weapon = fLen;
						pMWeaponResult = pMWeapon;
					}
				}
			}
		}

		++node;
	}
	
	return pMWeaponResult;
}

void ZWeaponMgr::AddMagic(ZSkill* pSkill, const rvector &pos, const rvector &dir,ZObject* pOwner)
{
	ZWeaponMagic* pWeapon = new ZWeaponMagic;

	RMeshMgr* pMeshMgr = ZGetEffectManager()->GetEffectMeshMgr();

	if(!pMeshMgr) return;

	ZSkillDesc* pDesc = pSkill->GetDesc();

	RMesh* pMesh = pMeshMgr->Get( pDesc->szTrailEffect );

	float fMagicScale = pDesc->fTrailEffectScale;

	if(!pMesh) return;

	pWeapon->Create(pMesh,pSkill,pos,dir,fMagicScale,pOwner);

	Add(pWeapon);
}

void ZWeaponMgr::DeleteWeaponHasTarget( const MUID& uidTargetChar )
{
	if(m_list.empty())
		return;

	z_weapon_node node;
	ZWeapon* pWeapon;
	ZWeaponMagic* pWeaponMagic;

	for(node = m_list.begin(); node != m_list.end(); ) {
		pWeapon = (*node);
		// Ÿ���� ���� �� �ִ� ź�� ���� ����ź ���̴�

		if (MIsExactlyClass(ZWeaponMagic, pWeapon)) {
			pWeaponMagic = MDynamicCast(ZWeaponMagic, pWeapon );
		
			if (pWeaponMagic) {
				if (pWeaponMagic->GetTarget() == uidTargetChar) {
					delete pWeaponMagic;
					node=m_list.erase(node);
					continue;
				}
			}
		}

		++node;
	}
}