#include "stdafx.h"

#include "ZGame.h"
#include "ZApplication.h"
#include "ZEffectAniMesh.h"
#include "Physics.h"
#include "ZCharacter.h"
#include "ZObject.h"
#include "RMaterialList.h"
#include "MDebug.h"


ZEffectAniMesh::ZEffectAniMesh(RMesh* pMesh, const rvector& Pos, rvector& dir)
{
	_ASSERT(pMesh);
	m_VMesh.Create(pMesh);
	bool bRet=m_VMesh.SetAnimation("play");
//	_ASSERT(bRet);
	m_VMesh.m_bCalcBoxWithScale = true;

	m_Pos = Pos;
	m_Dir = dir;
	m_DirOrg = dir;

	m_fRotateAngleZ = 0;
	m_fRotateAngleY = 0;
	m_nStartTime = timeGetTime();

	m_Scale = rvector(1.f,1.f,1.f);

	m_nDrawMode = ZEDM_NONE;
	m_nAlignType = 0;
	m_nLifeTime = -1;

	m_nStartAddTime = 0;

	m_bDelay	= false;
	m_isCheck	= false;
	m_bLoopType = false;

	AniFrameInfo* pInfo = m_VMesh.GetFrameInfo(ani_mode_lower);

	if(pInfo && pInfo->m_pAniSet) {
		if(pInfo->m_pAniSet->GetAnimationLoopType() == RAniLoopType_Loop) {
			m_bLoopType = true;
		}
	}

	m_Up = rvector(0.f,0.f,1.f);

	m_vBackupPos = rvector(-999.f,-999.f,-999.f);

	m_nAutoAddEffect = ZEffectAutoAddType_None;
}

ZEffectSlash::ZEffectSlash(RMesh* pMesh, const rvector& Pos, rvector& dir)
:ZEffectAniMesh(pMesh,Pos,dir)
{

}

#define SC_ROTATION	1.2f
#define SC_LIFETIME	2000


void ZEffectAniMesh::CheckCross(rvector& Dir,rvector& Up)
{
/*
	Normalize(Dir);
	Normalize(Up);

	rvector diff = Dir - Up;

	if( diff.x < 0.001 && diff.y < 0.001 && diff.z < 0.001)
		Up = rvector(0,1,0);// -.-;;
*/
	float dot = DotProduct(Dir,Up);

	if(dot > 0.99f || dot < -0.99f)
		Up = rvector(0,1,0);// -.-;;
}

bool ZEffectAniMesh::Draw(unsigned long int nTime)
{
	if(m_VMesh.m_pMesh==NULL)
		return false;

	AniFrameInfo* pInfo = m_VMesh.GetFrameInfo(ani_mode_lower);

	if(pInfo->m_pAniSet==NULL)// m_VMesh.m_pAniSet[0]==NULL
		return false;

	if(nTime < m_nStartTime + m_nStartAddTime) {
		return true;
	}
	// �׸��Ⱑ ���� �Ȱ�� ���� ������ ĳ���� �����͸� ����..

	if( m_bDelay && !m_isCheck ) {

		ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

		ZCharacterObject* pCObj = MDynamicCast(ZCharacterObject, pObj);

		if(pCObj) {

			if(!pCObj->IsRendered()) 
				return false;

			rvector _vp	  = pCObj->GetPosition();
			rvector _vdir = pCObj->m_Direction;
			_vdir.z = 0;
			Normalize(_vdir);

			/*
			int cm = g_pGame->SelectSlashEffectMotion(pObj);//���� Į �ֵθ��� ����
			if(cm==SEM_ManSlash5) {
				_vdir.z = 0.f;
				_vp += _vdir * 120.f;
				_vp.z -= 100.f;
			}
			else if(cm==SEM_WomanSlash5) {
				_vdir.z = 0.f;
				_vp += _vdir * 200.f;
				_vp.z -= 80.f;
			}
			*/

			m_Pos = _vp;
			m_Dir = _vdir;
		}

		m_isCheck = true;
	}

	DWORD dwDiff = nTime - m_nStartTime;

//	float fSec = (float)dwDiff/1000.0f;	
//	float fOpacity = (SC_LIFETIME-dwDiff)/(float)SC_LIFETIME;

	rvector Dir;

	rvector Up=m_Up;

	if (m_nAlignType == 0) {
		Dir = -RCameraDirection;
	}
	else if( m_nAlignType == 1 )
	{
		Dir = m_DirOrg;
	}
	else if( m_nAlignType == 2 )
	{
		Dir = rvector( 0,1,0 );
		Up = rvector( 0,0,1 );
	}

	CheckCross(Dir,Up);

	rmatrix World;

	MakeWorldMatrix(&World, m_Pos, Dir, Up);

	rmatrix Rotation;

	D3DXMatrixRotationZ(&Rotation,m_fRotateAngleZ);
	World = Rotation * World;

	D3DXMatrixRotationY(&Rotation,m_fRotateAngleY);
	World = Rotation * World;

	m_VMesh.SetScale(m_Scale);
	m_VMesh.SetWorldMatrix(World);
	m_VMesh.Frame();					// �ð��� �귯 ���� �Ѵ�~

	if(m_bRender){
		m_VMesh.Render();
		m_bisRendered = m_VMesh.m_bIsRender;
	} 
	else m_bisRendered = false;

	if( m_nAutoAddEffect != ZEffectAutoAddType_None ) {

		rvector vPos;
		RMeshNode* pNode = NULL;

		if(m_nAutoAddEffect == ZEffectAutoAddType_Methor ) {

			pNode = m_VMesh.m_pMesh->GetMeshData("methor");

			vPos.x = pNode->m_mat_result._41;
			vPos.y = pNode->m_mat_result._42;
			vPos.z = pNode->m_mat_result._43;

//			static rmatrix _mrot = RGetRotY(180) * RGetRotX(90);
			static rmatrix _mrot = RGetRotX(90);

			vPos = vPos * _mrot;
			vPos += m_Pos;

			if(m_vBackupPos != vPos) {
				ZGetEffectManager()->AddTrackMethor(vPos);
				m_vBackupPos = vPos;
			}
		}
	}

	if( m_nLifeTime != -1 && m_bLoopType ) {// �Ҹ� �ð��� ������ �ְ�..���ϸ��̼��� ���� Ÿ���� ���..
		if(m_VMesh.isOncePlayDone())
			if(nTime > m_nStartTime + m_nLifeTime) {
				return false;
			}
	}
	else {
		if(m_VMesh.isOncePlayDone()) {
			return false;
		}
	}

	return true;
}

RVisualMesh* ZEffectAniMesh::GetVMesh() 
{ 
	return &m_VMesh; 
}

void ZEffectAniMesh::SetUpVector(rvector& v) 
{ 
	m_Up = v; 
}

void ZEffectAniMesh::SetUid(MUID uid) 
{ 
	m_uid = uid; 
}

void ZEffectAniMesh::SetDelayPos(MUID id) 
{ 
	m_bDelay = true; 
	m_uid = id; 
}

void ZEffectAniMesh::SetScale(rvector s) 
{ 
	m_Scale = s; 
}

void ZEffectAniMesh::SetRotationAngleZ(float a) 
{ 
	m_fRotateAngleZ = a; 
}

void ZEffectAniMesh::SetRotationAngleY(float a) 
{ 
	m_fRotateAngleY = a; 
}

void ZEffectAniMesh::SetAlignType(int type) 
{ 
	m_nAlignType = type; 
}

void ZEffectAniMesh::SetStartTime(DWORD _time) 
{ 
	m_nStartAddTime = _time; 
}

rvector ZEffectAniMesh::GetSortPos() 
{ 
	return m_Pos; 
}

// ZEffectDash

ZEffectDash::ZEffectDash(RMesh* pMesh, rvector& Pos, rvector& Dir,MUID uidTarget)
: ZEffectAniMesh(pMesh,Pos,Dir)
{
	m_uid = uidTarget;
}


bool ZEffectDash::Draw(unsigned long int nTime)
{
	ZObject *pTarget = ZGetGame()->m_ObjectManager.GetObject(m_uid);

	if(pTarget) {
		ZObserver *pObserver = ZGetGameInterface()->GetCombatInterface()->GetObserver();
		if(pObserver->IsVisible())
		{
			rvector pos,dir;
			pTarget->GetHistory(&pos,&dir,ZGetGame()->GetTime()-pObserver->GetDelay());
			m_Pos = pos;
		}else
			m_Pos = pTarget->GetPosition();
		return ZEffectAniMesh::Draw(nTime);
	}
/*
	if(m_pObj) {
		m_Pos = m_pObj->GetPosition();
	}

	return ZEffectSlash::Draw(nTime);
*/	
	return false;
}

// ZEffectLevelUp

ZEffectLevelUp::ZEffectLevelUp(RMesh* pMesh, rvector& Pos, rvector& Dir,rvector& vAddPos,ZObject* pObj)
: ZEffectAniMesh(pMesh,Pos,Dir)
{
	if(pObj)
		m_uid = pObj->GetUID();

	m_vAddPos = vAddPos;
	m_type = eq_parts_pos_info_etc;
}


bool ZEffectLevelUp::Draw(unsigned long int nTime)
{
	ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

	if(pObj) {
		if(pObj->m_pVMesh) {
		
//			m_Pos = pObj->GetPosition() + m_vAddPos;
			m_Pos = pObj->m_pVMesh->GetBipTypePosition(m_type);
			m_DirOrg = pObj->m_Direction;
			return ZEffectAniMesh::Draw(nTime);
		}
	}
	return false;
}

// ZEffectPartsTypePos

ZEffectPartsTypePos::ZEffectPartsTypePos(RMesh* pMesh, rvector& Pos, rvector& Dir,rvector& vAddPos,ZObject* pObj)
: ZEffectAniMesh(pMesh,Pos,Dir)
{
	if(pObj)
		m_uid = pObj->GetUID();

	m_vAddPos = vAddPos;
	m_type = eq_parts_pos_info_etc;
}


bool ZEffectPartsTypePos::Draw(unsigned long int nTime)
{
	ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

	if(pObj) {
		if(pObj->m_pVMesh) {

			if(pObj->IsDie())
				return false;

			m_Pos = pObj->m_pVMesh->GetBipTypePosition(m_type);
//			m_DirOrg = pObj->m_Direction;
			return ZEffectAniMesh::Draw(nTime);
		}
	}
	return false;
}

// ZEffectWeaponEnchant

ZEffectWeaponEnchant::ZEffectWeaponEnchant(RMesh* pMesh, rvector& Pos, rvector& Dir, ZObject* pObj)
: ZEffectAniMesh(pMesh,Pos,Dir)
{
	if(pObj)
		m_uid = pObj->GetUID();

	m_VMesh.SetCheckViewFrustum(false);
}


bool ZEffectWeaponEnchant::Draw(unsigned long int nTime)
{
	ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

	if(pObj) {
		if(pObj->m_pVMesh) {

			// ������� �ȱ׷����� 
			RVisualMesh* pVMesh = pObj->m_pVMesh->GetSelectWeaponVMesh();//weapon_mesh

			if(pVMesh)
				if(!pVMesh->m_bIsRender) return false;
			
			rmatrix m = pObj->m_pVMesh->GetCurrentWeaponPositionMatrix();
			m_Pos	 = rvector(m._41,m._42,m._43);
			m_DirOrg = rvector(m._21,m._22,m._23);
			m_Up	 = rvector(m._11,m._12,m._13);

			bool hr = ZEffectAniMesh::Draw(nTime);

			// ��տ� ���⿡ ����ó��			

			if( pObj->m_pVMesh->IsDoubleWeapon() ) 
			{
				m = pObj->m_pVMesh->GetCurrentWeaponPositionMatrix(true);

				m_Pos	 = rvector(m._41,m._42,m._43);
				m_DirOrg = rvector(m._21,m._22,m._23);
				m_Up	 = rvector(m._11,m._12,m._13);

				hr = ZEffectAniMesh::Draw(nTime);
			}

			return hr;
		}
	}
	return false;
}

// ZEffectDash

ZEffectIcon::ZEffectIcon(RMesh* pMesh, ZObject* pObj)
: ZEffectAniMesh(pMesh,rvector(0,0,0),rvector(1,0,0))
{
	if(pObj)
		m_uid = pObj->GetUID();
	m_type = eq_parts_pos_info_etc;
}


bool ZEffectIcon::Draw(unsigned long int nTime)
{
	ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

	if(pObj) {
		m_Pos = pObj->GetVisualMesh()->GetHeadPosition()+rvector(0,0,60);
		return ZEffectAniMesh::Draw(nTime);
	}
	return false;
}


ZEffectShot::ZEffectShot(RMesh* pMesh, rvector& Pos, rvector& Dir,ZObject* pObj) 
: ZEffectAniMesh(pMesh,Pos,Dir)
{
	m_nStartAddTime = 0;

	m_isLeftWeapon = false;

	if(pObj) {
		m_isMovingPos = true;
		m_uid = pObj->GetUID();
	}
	else {
		m_isMovingPos = false;
		m_uid = MUID(0,0);
	}
}


bool ZEffectShot::Draw(unsigned long int nTime)
{
	if( m_isMovingPos ) {

		ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

		if(pObj) {

//			if( MIsDerivedFromClass(ZCharacterObject, pObj )==false) return false;
//			ZCharacterObject* pCObj = (ZCharacterObject*)pObj;

			ZCharacterObject* pCObj = MDynamicCast(ZCharacterObject, pObj);

			if(!pCObj) return false;

			if(!pCObj->IsRendered()) 
				return false;

			rvector pos;

			if(m_isLeftWeapon) {
				if(pCObj->GetWeaponTypePos(weapon_dummy_muzzle_flash,&pos,true) ) {
					m_Pos = pos;
					m_Dir = pCObj->m_Direction;
					Normalize(m_Dir);
				}
			}
			else {
				if(pCObj->GetWeaponTypePos(weapon_dummy_muzzle_flash,&pos) ) {
					m_Pos = pos;
					m_Dir = pCObj->m_Direction;
					Normalize(m_Dir);
				}
			}
		}
	}

	return ZEffectAniMesh::Draw(nTime);
}


ZEffectBerserkerIconLoop::ZEffectBerserkerIconLoop(RMesh* pMesh, ZObject* pObj) 
									: ZEffectIcon(pMesh,pObj)  
{	
	m_nElapsedTime = 0; 
	AniFrameInfo* pInfo = m_VMesh.GetFrameInfo(ani_mode_lower);
	RAnimation* pAni = pInfo->m_pAniSet;
	if( 0 != pAni )
	{
		int nTotalFrame = pAni->GetMaxFrame();
		pInfo->m_nAddFrame = rand() % nTotalFrame;
	}
}

bool ZEffectBerserkerIconLoop::Draw(unsigned long int nTime)
{
	ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);
	ZCharacter* pChar = MDynamicCast(ZCharacter, pObj);

	if( pChar ) {
		if(!pChar->IsTagger()) return false;
		if(!pChar->IsRendered()) return true;

		if( pChar->m_pVMesh ) {
			m_Pos = pObj->m_pVMesh->GetBipTypePosition(m_type);
			m_DirOrg = -pChar->m_Direction;
			ZEffectAniMesh::Draw(nTime);
			return true;
		}
	}

	return false;
}
