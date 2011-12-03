#include "stdafx.h"

#include "ZPrerequisites.h"

#include "ZEffectManager.h"
#include "ZEffectGunFire.h"
#include "ZEffectBulletMark.h"
#include "ZEffectLightFragment.h"
#include "ZEffectSmoke.h"
#include "ZEffectLightTracer.h"
#include "ZEffectMesh.h"
#include "ZEffectStaticMesh.h"
#include "ZEffectAniMesh.h"

#include "ZGame.h"
#include "ZConfiguration.h"

#include "RealSpace2.h"
#include "ZCharacter.h"
#include "ZMyCharacter.h"

#include "RMeshMgr.h"

#include <crtdbg.h>
#include "MDebug.h"

#include <dxerr9.h>


#ifndef _PUBLISH
class ZEffectValidator : public set<ZEffect*> 
{
public:
	void Add(ZEffect* pNew) {
		iterator itr = find(pNew);
		if(itr!=end())
		{
			ZEffect *pDup = *itr;

			_ASSERT(FALSE);
			mlog("effect duplicated.\n");
		}
		insert(pNew);
	}

	void Erase(ZEffect* pEffect) {
		if(find(pEffect)==end())
		{
			_ASSERT(FALSE);
			mlog("effect not exist.\n");
		}
		erase(pEffect);
	}

} g_EffectValidator;
#endif

// ����Ʈ ������ ����..�ɼ�

static int g_nEffectLevel = Z_VIDEO_EFFECT_HIGH;//�ֻ���.. 0 : �� 1 : �� 2 : ��

// ����ΰ�� ��� �׸���
// �߱��ΰ�� �Ÿ��� ���� �׸��� �ȱ׸���..
// �ϱ��ΰ�� ���ӿ�ҿ� ������ �ִ� ��Ұ� �ƴѰ��� �׸����� �ʴ´�..( ���� ����Ʈ ������ )

void SetEffectLevel(int level)
{
	g_nEffectLevel = level;
}

int	 GetEffectLevel()
{
	return g_nEffectLevel;
}

class ZEffectCharging : public ZEffectAniMesh, public CMemPoolSm<ZEffectCharging> {
public:
	ZEffectCharging(RMesh* pMesh, const rvector& Pos, rvector& Dir,ZObject* pObj)
		: ZEffectAniMesh(pMesh,Pos,Dir)
	{
		if(pObj)
			m_uid = pObj->GetUID();
	}

	virtual bool Draw(unsigned long int nTime)
	{
		ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

		if(!pObj) return false;

		ZCharacter* pChar = MDynamicCast(ZCharacter, pObj);

		if(!pChar) return false;

		//mmemory proxy
		if( !((pChar->m_bCharging->Ref())) ) return false;

		rmatrix m = pObj->m_pVMesh->GetCurrentWeaponPositionMatrix();

		m_Pos	 = rvector(m._41,m._42,m._43);
		m_DirOrg = rvector(m._21,m._22,m._23);
		m_Up	 = rvector(m._11,m._12,m._13);

		ZEffectAniMesh::Draw(nTime);

		if( pObj->m_pVMesh->IsDoubleWeapon() ) {//�̵���

			m = pObj->m_pVMesh->GetCurrentWeaponPositionMatrix(true);

			m_Pos	 = rvector(m._41,m._42,m._43);
			m_DirOrg = rvector(m._21,m._22,m._23);
			m_Up	 = rvector(m._11,m._12,m._13);

			ZEffectAniMesh::Draw(nTime);
		}

		return true;
	}
};

class ZEffectCharged : public ZEffectAniMesh, public CMemPoolSm<ZEffectCharged> {
public:
	ZEffectCharged(RMesh* pMesh, const rvector& Pos, rvector& Dir,ZObject* pObj)
		: ZEffectAniMesh(pMesh,Pos,Dir)
	{
		if(pObj)
			m_uid = pObj->GetUID();
	}

	virtual bool Draw(unsigned long int nTime)
	{
		ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

		if( pObj ) {

//			if(MIsExactlyClass(ZCharacter, pObj)==false) return false;
//			ZCharacter* pChar = (ZCharacter*)pObj;

			ZCharacter* pChar = MDynamicCast(ZCharacter, pObj);

			if(!pChar) return false;

			//mmemory proxy
			if(! (( pChar->m_bCharged->Ref() )) )	return false;
			if(!pChar->IsRendered()) return true;
			if( pObj->m_pVMesh ) {

				rmatrix m = pObj->m_pVMesh->GetCurrentWeaponPositionMatrix();
				m_Pos	 = rvector(m._41,m._42,m._43);
				m_DirOrg = rvector(m._21,m._22,m._23);
				m_Up	 = rvector(m._11,m._12,m._13);

				ZEffectAniMesh::Draw(nTime);

				if( pObj->m_pVMesh->IsDoubleWeapon() ) {//�̵���
					m = pObj->m_pVMesh->GetCurrentWeaponPositionMatrix(true);

					m_Pos	 = rvector(m._41,m._42,m._43);
					m_DirOrg = rvector(m._21,m._22,m._23);
					m_Up	 = rvector(m._11,m._12,m._13);

					ZEffectAniMesh::Draw(nTime);
				}

				return true;
			}
		}
		return false;
	}
};

//////////////////////////////////////////////////////////////////////////////////
// ZEffect

ZEffect::ZEffect()
{
	m_nDrawMode = ZEDM_NONE;
	m_nType = ZET_NONE;

	m_fDist = 0.f;

	m_fHideDist[0] = 5000.f;//�� 200M
	m_fHideDist[1] = 3000.f;//�� 100M
	m_fHideDist[2] = 1000.f;//��  50M

	m_bRender = true;
	m_bisRendered = false;
	m_bWaterSkip = false;
}

ZEffect::~ZEffect()
{

}

bool ZEffect::CheckRenderAble(int level,float dist)
{
	if(level < 0 || level > 2)
		return false;

	if(m_fHideDist[level] < dist ) {
		m_bRender = false;
		return false;
	}

	m_bRender = true;

	return true;
}

void ZEffect::CheckWaterSkip(int mode,float height) 
{
	if(mode==0)	// ����..
		m_bWaterSkip = true;
	else		// �������� �׸���
		m_bWaterSkip = false;
}

bool ZEffect::Draw(unsigned long int nTime)
{
	return true;
}

ZEffectDrawMode	ZEffect::GetDrawMode(void)
{
	return m_nDrawMode;
}

bool ZEffect::isEffectType(ZEffectType type)
{
	if(m_nType==type)
		return true;
	return false;
}

void ZEffect::SetEffectType(ZEffectType type)
{
	m_nType = type;
}

rvector ZEffect::GetSortPos() 
{
	return rvector(0.f,0.f,0.f);
}

void ZEffect::SetDistOption(float l0,float l1,float l2) 
{
	m_fHideDist[0] = l0;//�� 200M
	m_fHideDist[1] = l1;//�� 100M
	m_fHideDist[2] = l2;//��  50M
}

//////////////////////////////////////////////////////////////////////////////////////

// �� �������ϱ�� ������~

bool CreateCommonRectVertexBuffer();	
void RealeaseCommonRectVertexBuffer();	// ZEffectBillboard.cpp

ZEffectManager::ZEffectManager(void) : m_bEnableDraw(true)
{
}

bool ZEffectManager::Create(void)
{
	char szFileName[256];

	for(int i=0; i<MUZZLESMOKE_COUNT; i++){
		sprintf(szFileName, "SFX/muzzle_smoke0%d.tga", i+1);
		m_pEBSMuzzleSmoke[i]  = new ZEffectBillboardSource(szFileName);
	}

	for(int i=0; i<MUZZLESMOKE_SHOTGUN_COUNT; i++){
		sprintf(szFileName, "SFX/muzzle_smoke4%d.tga", i+1);
		m_pEBSMuzzleSmokeShotgun[i]  = new ZEffectBillboardSource(szFileName);
	}

	for(int i=0; i<SMOKE_COUNT; i++){
		sprintf(szFileName, "SFX/smoke0%d.tga", i+1);
		m_pEBSSmokes[i] = new ZEffectBillboardSource(szFileName);
	}
	for(int i=0; i<BLOOD_COUNT; i++){
		sprintf(szFileName, "SFX/blood0%d.tga", i+1);
		m_pEBSBloods[i] = new ZEffectBillboardSource(szFileName);
	}
	for(int i=0; i<BLOODMARK_COUNT; i++){
		sprintf(szFileName, "SFX/blood-mark0%d.tga", i+1);
		m_pEBSBloodMark[i] = new ZEffectBillboardSource(szFileName);
	}

	m_pEBSLightTracer = new ZEffectBillboardSource("SFX/gz_sfx_tracer.bmp");
	
//	m_pEBSBulletMark[0] = new ZEffectBillboardSource("SFX/bullet-mark01.tga");
//	m_pEBSBulletMark[1] = new ZEffectBillboardSource("SFX/bullet-mark02.tga");
	m_pEBSBulletMark[0] = new ZEffectBillboardSource("SFX/gz_sfx_shotgun_bulletmark01.tga");
	m_pEBSBulletMark[1] = new ZEffectBillboardSource("SFX/gz_sfx_shotgun_bulletmark02.tga");
	m_pEBSBulletMark[2] = new ZEffectBillboardSource("SFX/gz_effect004.tga");

	for(int i=0; i<RIFLEFIRE_COUNT; i++){
		sprintf(szFileName, "SFX/gz_sfx_mf0%d.bmp", i+1);
		m_pEBSRifleFire[i][0] = new ZEffectBillboardSource(szFileName);
		sprintf(szFileName, "SFX/gz_sfx_mf1%d.bmp", i+1);
		m_pEBSRifleFire[i][1] = new ZEffectBillboardSource(szFileName);
	}

	for(int i=0; i<SHOTGUNFIRE_COUNT; i++) {
		sprintf(szFileName, "SFX/gz_sfx_mf4%d.bmp", i+1);
		m_pEBSShotGunFire[i] = new ZEffectBillboardSource(szFileName);
	}

	for(int i=0; i<GUNFIRE_COUNT; i++){
		sprintf(szFileName, "SFX/gz_sfx_mf2%d.bmp", i+1);
		m_pEBSGunFire[i][0] = new ZEffectBillboardSource(szFileName);
		sprintf(szFileName, "SFX/gz_sfx_mf3%d.bmp", i+1);
		m_pEBSGunFire[i][1] = new ZEffectBillboardSource(szFileName);
	}

	m_pEffectMeshMgr = new RMeshMgr;

	if(m_pEffectMeshMgr->LoadXmlList("SFX/effect_list.xml")==-1) {
		mlog("effect_list loding error\n");
	}

	// �ӵ��� ���� index

	m_pMeshEmptyCartridges[0] = m_pEffectMeshMgr->Get("empty_cartridge1");
	m_pMeshEmptyCartridges[1] = m_pEffectMeshMgr->Get("empty_cartridge2");

	m_pSworddam[0] = m_pEffectMeshMgr->Get("sword_damage1");
	m_pSworddam[1] = m_pEffectMeshMgr->Get("sword_damage2");
	m_pSworddam[2] = m_pEffectMeshMgr->Get("sword_damage3");
	m_pSworddam[3] = m_pEffectMeshMgr->Get("sword_damage4");

	m_pRangeDamaged[0] = m_pEffectMeshMgr->Get("ef_damage01.elu");
	m_pRangeDamaged[1] = m_pEffectMeshMgr->Get("ef_damage02.elu");
	m_pRangeDamaged[2] = m_pEffectMeshMgr->Get("ef_damage03.elu");
	m_pRangeDamaged[3] = m_pEffectMeshMgr->Get("ef_damage04.elu");
	m_pRangeDamaged[4] = m_pEffectMeshMgr->Get("ef_damage05.elu");
	m_pRangeDamaged[5] = m_pEffectMeshMgr->Get("ef_damage06.elu");

	m_pSwordglaze  = m_pEffectMeshMgr->Get("sword_glaze");

	m_pDashEffect				= m_pEffectMeshMgr->Get("dash_effect");
	m_pGrenadeEffect			= m_pEffectMeshMgr->Get("grenade_effect");
	m_pGrenadeExpEffect			= m_pEffectMeshMgr->Get("ef_gre_ex");
	m_pRocketEffect				= m_pEffectMeshMgr->Get("rocket_effect");
	m_pDynamite					= m_pEffectMeshMgr->Get("ef_exgrenade");
	m_pSwordDefenceEffect[0]	= m_pEffectMeshMgr->Get("sword_defence_effect");
	m_pSwordDefenceEffect[1]	= m_pEffectMeshMgr->Get("sword_defence_effect2");
	m_pSwordDefenceEffect[2]	= m_pEffectMeshMgr->Get("sword_defence_effect3");
	m_pFragment[0]				= m_pEffectMeshMgr->Get("fragment01");
	m_pFragment[1]				= m_pEffectMeshMgr->Get("fragment02");
	m_pFragment[2]				= m_pEffectMeshMgr->Get("fragment03");
	m_pFragment[3]				= m_pEffectMeshMgr->Get("fragment04");
	m_pFragment[4]				= m_pEffectMeshMgr->Get("fragment05");
	m_pFragment[5]				= m_pEffectMeshMgr->Get("fragment06");

	m_pSwordWaveEffect[0]		= m_pEffectMeshMgr->Get("sword_wave_effect");
	m_pSwordWaveEffect[1]		= m_pEffectMeshMgr->Get("sword_slash_effect");

	m_pSwordEnchantEffect[0]	= m_pEffectMeshMgr->Get("ef_sworddam_fire");
	m_pSwordEnchantEffect[1]	= m_pEffectMeshMgr->Get("ef_sworddam_ice");
	m_pSwordEnchantEffect[2]	= m_pEffectMeshMgr->Get("ef_sworddam_flash");
	m_pSwordEnchantEffect[3]	= m_pEffectMeshMgr->Get("ef_sworddam_poison");

	m_pMagicDamageEffect		= m_pEffectMeshMgr->Get("magicmissile_damage");

	m_pMagicEffectWall[0]		= m_pEffectMeshMgr->Get("fireball_work_wall");
	m_pMagicEffectWall[1]		= m_pEffectMeshMgr->Get("icemissile_wall");
	m_pMagicEffectWall[2]		= m_pEffectMeshMgr->Get("magicmissile_wall");

//	m_pRocketSmokeEffect		= m_pEffectMeshMgr->Get("rocket_smoke_effect");
	m_pSwordUppercutEffect		= m_pEffectMeshMgr->Get("sword_uppercut_effect");
	m_pSwordUppercutDamageEffect= m_pEffectMeshMgr->Get("sword_uppercut_damage_effect");
	m_pFlameMG					= m_pEffectMeshMgr->Get("flame_mg");
	m_pFlamePistol				= m_pEffectMeshMgr->Get("flame_pistol");
	m_pFlameRifle				= m_pEffectMeshMgr->Get("flame_rifle");
	m_pFlameShotgun				= m_pEffectMeshMgr->Get("flame_shotgun");

	//m_pWaterSplash				= m_pEffectMeshMgr->Get(")

	m_pLevelUpEffect[0]	= m_pEffectMeshMgr->Get("levelup");
	m_pLevelUpEffect[1] = m_pEffectMeshMgr->Get("levelup01");
	m_pReBirthEffect	= m_pEffectMeshMgr->Get("rebirth");
	m_pTimeRewardEffect = m_pEffectMeshMgr->Get("timereward");
	m_pEatBoxEffect		= m_pEffectMeshMgr->Get("ef_eatbox");
	m_pHealEffect		= m_pEffectMeshMgr->Get("ef_heal");
	m_pRepairEffect		= m_pEffectMeshMgr->Get("ef_repair");
	m_pHealInstantEffect	= m_pEffectMeshMgr->Get("ef_heal_instant");
	m_pRepairInstantEffect	= m_pEffectMeshMgr->Get("ef_repair_instant");
	m_pHealOverTimeEffect	= m_pEffectMeshMgr->Get("ef_heal_overtime");
	m_pRepairOverTimeEffect	= m_pEffectMeshMgr->Get("ef_repair_overtime");
	m_pHasteEffect			= m_pEffectMeshMgr->Get("ef_haste");
	m_pHealOverTimeBeginEffect		= m_pEffectMeshMgr->Get("ef_heal_overtime_begin");
	m_pRepairOverTimeBeginEffect	= m_pEffectMeshMgr->Get("ef_repair_overtime_begin");
	m_pHasteBeginEffect				= m_pEffectMeshMgr->Get("ef_haste_begin");

	m_pExpanseAmmoEffect	= m_pEffectMeshMgr->Get("ef_ammunition");

	m_pDaggerUpper		= m_pEffectMeshMgr->Get("ef_dagger_upper");

	m_pSwordFire		= m_pEffectMeshMgr->Get("ef_sword_fire");
	m_pSwordElec		= m_pEffectMeshMgr->Get("ef_sword_flash");
	m_pSwordCold		= m_pEffectMeshMgr->Get("ef_sword_ice");
	m_pSwordPoison		= m_pEffectMeshMgr->Get("ef_sword_poison");

	m_pTrapGuideHostile	= m_pEffectMeshMgr->Get("ef_trap_guide_hostile");
	m_pTrapGuideFriendly= m_pEffectMeshMgr->Get("ef_trap_guide_friendly");
	m_pTrapFire		= m_pEffectMeshMgr->Get("ef_trap_fire");
	m_pTrapCold		= m_pEffectMeshMgr->Get("ef_trap_cold");

	m_pBulletOnWallEffect[0]	= m_pEffectMeshMgr->Get("ef_effect001.elu");
	m_pBulletOnWallEffect[1]	= m_pEffectMeshMgr->Get("ef_effect002.elu");

	m_pEBSRing[0] = new ZEffectBillboardSource("SFX/gd_effect_001.tga");
	m_pEBSRing[1] = new ZEffectBillboardSource("SFX/gd_effect_002.tga");

	m_pEBSDash[0] = new ZEffectBillboardSource("SFX/gz_effect_dash01.tga");
	m_pEBSDash[1] = new ZEffectBillboardSource("SFX/gz_effect_dash02.tga");

	m_pEBSLanding = new ZEffectBillboardSource("SFX/ef_gz_footstep.tga");

	m_pEBSWaterSplash	= new ZEffectBillboardSource("SFX/gd_effect_006.tga");
	m_pWaterSplash	= m_pEffectMeshMgr->Get("water_splash");

	m_pEBSWorldItemEaten	= new ZEffectBillboardSource("SFX/ef_sw.bmp");
	m_pWorldItemEaten = m_pEffectMeshMgr->Get("ef_eatitem");

	m_pCharacterIcons[0] = m_pEffectMeshMgr->Get("ef_pre_all.elu");
	m_pCharacterIcons[1] = m_pEffectMeshMgr->Get("ef_pre_un.elu");
	m_pCharacterIcons[2] = m_pEffectMeshMgr->Get("ef_pre_exe.elu");
	m_pCharacterIcons[3] = m_pEffectMeshMgr->Get("ef_pre_fan.elu");
	m_pCharacterIcons[4] = m_pEffectMeshMgr->Get("ef_pre_head.elu");

	m_pCommandIcons[0] = m_pEffectMeshMgr->Get("red_commander");
	m_pCommandIcons[1] = m_pEffectMeshMgr->Get("blue_commander");

	m_pLostConIcon = m_pEffectMeshMgr->Get("ef_lostcon.elu");
	m_pChatIcon = m_pEffectMeshMgr->Get("ef_chat.elu");

	m_pBerserkerEffect	= m_pEffectMeshMgr->Get("ef_berserker");

//	m_pLighteningEffect = m_pEffectMeshMgr->Get("ef_lightening");
	m_pBlizzardEffect = m_pEffectMeshMgr->Get("ef_blizzard");

	m__skip_cnt = 0;
	m__cnt = 0;
	m__rendered = 0;

	CreateCommonRectVertexBuffer();// ZEffectBillboard.cpp

	ZEffectBase::CreateBuffers();

	m_BulletMarkList.Create("SFX/gz_sfx_shotgun_bulletmark01.tga");
	m_LightFragments.Create("SFX/gz_sfx_tracer.bmp");
	m_LightFragments.SetScale(rvector(4.0f,0.8f,1.0f));

	m_BillboardLists[0].Create("SFX/muzzle_smoke01.tga");
	m_BillboardLists[1].Create("SFX/muzzle_smoke02.tga");
	m_BillboardLists[2].Create("SFX/muzzle_smoke03.tga");
	m_BillboardLists[3].Create("SFX/muzzle_smoke04.tga");

	m_BillboardLists[4].Create("SFX/smoke_rocket.tga");

	m_ShadowList.Create("SFX/gz_shadow.tga");

	m_BillBoardTexAniList[0].Create("SFX/gd_effect_020.bmp");
	m_BillBoardTexAniList[1].Create("SFX/gd_effect_021.bmp");
	m_BillBoardTexAniList[2].Create("SFX/gd_effect_019.bmp");
	m_BillBoardTexAniList[3].Create("SFX/ef_magicmissile.bmp");
	m_BillBoardTexAniList[4].Create("SFX/ef_methor_smoke.tga");

	m_BillBoardTexAniList[0].SetTile(4,4,0.25f,0.25f);
	m_BillBoardTexAniList[1].SetTile(4,4,0.25f,0.25f);
	m_BillBoardTexAniList[2].SetTile(4,4,0.25f,0.25f);
	m_BillBoardTexAniList[2].m_bFixFrame = true; // frame animation �� ���Ѵ�..
	m_BillBoardTexAniList[3].SetTile(2,2,0.375f,0.375f);
//	m_BillBoardTexAniList[3].m_nMaxFrame = 4;
	m_BillBoardTexAniList[3].m_bFixFrame = true; // frame animation �� ���Ѵ�..
	m_BillBoardTexAniList[4].SetTile(4,4,0.25f,0.25f);
//	m_BillBoardTexAniList[4].m_nRenderMode = ZEDM_ALPHAMAP;

//	m_BillboardLists[4].m_bUseRocketSmokeColor = true;
/*
	for(int i=0;i<REnchantType_End;i++)
	{
		
	}
*/
	rvector veczero = rvector(0.f,0.f,0.f);

	m_pWeaponEnchant[ZC_ENCHANT_NONE]		= NULL;
	m_pWeaponEnchant[ZC_ENCHANT_FIRE]		= new ZEffectWeaponEnchant( m_pSwordFire  ,veczero,veczero, NULL );
	m_pWeaponEnchant[ZC_ENCHANT_COLD]		= new ZEffectWeaponEnchant( m_pSwordCold  ,veczero,veczero, NULL );
	m_pWeaponEnchant[ZC_ENCHANT_LIGHTNING]	= new ZEffectWeaponEnchant( m_pSwordElec  ,veczero,veczero, NULL );
	m_pWeaponEnchant[ZC_ENCHANT_POISON]		= new ZEffectWeaponEnchant( m_pSwordPoison,veczero,veczero, NULL );

	return true;
}

ZEffectWeaponEnchant* ZEffectManager::GetWeaponEnchant(ZC_ENCHANT type)
{
	if(type < ZC_ENCHANT_END) {
		return m_pWeaponEnchant[type];
	}
	return NULL;
}

//#define _DELETE(node)

ZEffectManager::~ZEffectManager(void)
{
	Clear();

	int i;

	for(i=0; i<MUZZLESMOKE_COUNT; i++){
		delete m_pEBSMuzzleSmoke[i];
	}

	for(i=0; i<MUZZLESMOKE_SHOTGUN_COUNT; i++){
		delete m_pEBSMuzzleSmokeShotgun[i];
	}

	for(i=0; i<SMOKE_COUNT; i++){
		delete m_pEBSSmokes[i];
	}
	for(i=0; i<BLOOD_COUNT; i++){
		delete m_pEBSBloods[i];
	}
	for(i=0; i<BLOODMARK_COUNT; i++){
		delete m_pEBSBloodMark[i];
	}

	delete m_pEBSLightTracer;

	delete m_pEBSBulletMark[0];
	delete m_pEBSBulletMark[1];
	delete m_pEBSBulletMark[2];
//	delete m_pEBSBulletMark[3];

	for(int i=0; i<RIFLEFIRE_COUNT; i++){
		delete m_pEBSRifleFire[i][0];
		delete m_pEBSRifleFire[i][1];
	}

	for(int i=0; i<SHOTGUNFIRE_COUNT;i++) {
		delete m_pEBSShotGunFire[i];
	}

	for(int i=0; i<GUNFIRE_COUNT; i++){
		delete m_pEBSGunFire[i][0];
		delete m_pEBSGunFire[i][1];
	}

	if(m_pEffectMeshMgr)
		delete m_pEffectMeshMgr;

	delete m_pEBSRing[0];
	delete m_pEBSRing[1];

	delete m_pEBSDash[0];
	delete m_pEBSDash[1];

	delete m_pEBSLanding;
	delete m_pEBSWaterSplash;

	delete m_pEBSWorldItemEaten;

	for(int i=1;i<REnchantType_End;i++) {
		if(m_pWeaponEnchant[i]) {
			delete m_pWeaponEnchant[i];
			m_pWeaponEnchant[i] = NULL;
		}
	}

	RealeaseCommonRectVertexBuffer();// ZEffectBillboard.cpp

	ZEffectBase::ReleaseBuffers();

	// �޸�Ǯ ����
	ZEffectWeaponEnchant::Release();
	ZEffectStaticMesh::Release();
	ZEffectSlash::Release();
	ZEffectDash::Release();

	ZEffectPartsTypePos::Release();

	ZEffectShot::Release();

	ZEffectWeaponEnchant::Release();

	ZEffectSmoke::Release();
	ZEffectLandingSmoke::Release();
	ZEffectSmokeGrenade::Release();
	ZEffectLightTracer::Release();

	ZEffectLightFragment::Release();
	ZEffectLightFragment2::Release();

	ZEFFECTBILLBOARDITEM::Release();
	ZEFFECTBULLETMARKITEM::Release();
	ZEFFECTBILLBOARDTEXANIITEM::Release();

	ZEffectCharging::Release();
	ZEffectCharged::Release();
	ZEffectBerserkerIconLoop::Release();

	ZEffectLevelUp::Release();
}

// ZEffectAniMesh ����...

int ZEffectManager::DeleteSameType(ZEffectAniMesh* pNew)
{
	int d = pNew->GetDrawMode();

	int cnt=0;

	ZEffectList::iterator node;
	ZEffect* pEffect = NULL;

	for(node = m_Effects[d].begin(); node != m_Effects[d].end(); ) {

		pEffect = (*node);

		if( pEffect->isEffectType( pNew->m_nType ) ) {
				
			if( ((ZEffectAniMesh*)pEffect)->GetUID() == pNew->GetUID() ) {

#ifndef _PUBLISH
				g_EffectValidator.Erase(pEffect);
#endif
				delete pEffect;
				node = m_Effects[d].erase( node );
				cnt++;
				continue;

			}
		}

		++node;
	}
	return cnt;
}

void ZEffectManager::Add(ZEffect* pNew)
{
	if(pNew==NULL) return;

#define MAX_WATER_DEEP 150

	rvector	water_pos;
	rvector	src_pos = pNew->GetSortPos();
	/*
	if(g_pGame->m_waters.Pick( src_pos, rvector( 0,0,1), &water_pos ))
	{
		if( D3DXVec3Length( &( water_pos - src_pos ) ) < MAX_WATER_DEEP )
		{
//			mlog("water\n");
			SAFE_DELETE(pNew);
			return;
		}
	}
	//*/

	_ASSERT(pNew->GetDrawMode()<ZEDM_COUNT);
	m_Effects[pNew->GetDrawMode()].insert(m_Effects[pNew->GetDrawMode()].end(), pNew);

#ifndef _PUBLISH
	g_EffectValidator.Add(pNew);
#endif
}

void ZEffectManager::Clear()
{
	for( int d = 0; d < ZEDM_COUNT; d++ ) {

		ZEffectList::iterator node;
		ZEffect* pEffect = NULL;

		for(node = m_Effects[d].begin(); node != m_Effects[d].end(); ) {
			pEffect = (*node);

#ifndef _PUBLISH
			g_EffectValidator.Erase(pEffect);
#endif

			delete pEffect;
			node = m_Effects[d].erase(node);
		}
	}

	m_LightFragments.Clear();

	for(int i=0;i<BILLBOARDLISTS_COUNT;i++)
		m_BillboardLists[i].Clear();

	m_ShadowList.Clear();

	for(i=0;i<BILLBOARDTEXANILIST_COUNT;i++)
		m_BillBoardTexAniList[i].Clear();

	m_BulletMarkList.Clear();
}

bool e_effect_sort_float(ZEffect* _a,ZEffect* _b) {
	if( _a->m_fDist > _b->m_fDist )
		return true;
	return false;
}

static int g_zeffectmanager_cnt = 0;

#define ZEFFECT_RENDER_MAX 4000.f
/*
void ZWorldItemManager::Draw(int mode,float height)//�ӽ�..
{
	ZWorldItem* pWorldItem	= 0;

	float _h = 0.f;

	for( WIL_Iterator iter = mItemList.begin(); iter != mItemList.end(); ++iter )
	{
		pWorldItem	= iter->second;
		if( pWorldItem->GetState() == WORLD_ITEM_VALIDATE )
		{
			_h = pWorldItem->GetPosition().z;

			bool bDraw = false;

			if(mode==0) {//����
				if(_h <= height) 
					bDraw = true;
			} else if(mode==1) {
				if(_h > height)//����
					bDraw = true;
			}

			if(bDraw)
				mDrawer.DrawWorldItem( pWorldItem );
		}
	}
}
*/

// �� ���� ������ �ӽ�..
void ZEffectManager::CheckWaterSkip(int mode,float height)
{
	for( int d = 0; d < ZEDM_COUNT; d++ ) {

		ZEffectList::iterator node;
		ZEffect* pEffect = NULL;

		for(node = m_Effects[d].begin(); node != m_Effects[d].end(); ) {
			pEffect = (*node);
			if(pEffect)
				pEffect->CheckWaterSkip(mode,height);
			++node;
		}

		m_Effects[d].sort( e_effect_sort_float );
	}
}

void ZEffectManager::Draw(unsigned long int nTime,int mode,float height)
{
	if (!m_bEnableDraw) return;

	IDirect3DStateBlock9* pStateBlock = NULL;
	HRESULT hr = RGetDevice()->CreateStateBlock(D3DSBT_PIXELSTATE, &pStateBlock);
	if (hr!=D3D_OK) {
		// �����ߴ�
		static int nErrorLogCount = 0;
		if(nErrorLogCount<100) {
			mlog("CreateStateBlock failed : %s",DXGetErrorString9(hr));
		}
	}

	if(pStateBlock)	pStateBlock->Capture();

	rvector camera_pos = RealSpace2::RCameraPosition;
	rvector t_vec;

	CheckWaterSkip( mode , height );

	if(mode==1)
		m_ShadowList.Draw();

	/////////////////////////////////////////////////////////////////////
	// ī�޶���� �Ÿ����� + �Ÿ��� ���� ������ ���ΰ���

	for( int d = 0; d < ZEDM_COUNT; d++ ) {

		ZEffectList::iterator node;
		ZEffect* pEffect = NULL;

		int cnt = 0;

		for(node = m_Effects[d].begin(); node != m_Effects[d].end(); ) {
			pEffect = (*node);

			if(pEffect==NULL) {
				int _size = (int)m_Effects[d].size();
				mlog("�߾�.. EffectManager NULL ���� �߻� ( %d list ���) : size : %d \n",d,_size);
				++node;
			} else {
				if(!pEffect->m_bWaterSkip) {
					t_vec = camera_pos - pEffect->GetSortPos();
					pEffect->m_fDist = Magnitude(t_vec);
					pEffect->CheckRenderAble( g_nEffectLevel,pEffect->m_fDist );// �Ÿ��� ���� �׷������ΰ��� üũ
				}
				++node;
			}
			cnt++;
		}
		m_Effects[d].sort(e_effect_sort_float);
	}
	

	//////////////////////////////////////////////////////////////////////

	m__skip_cnt = 0;
	m__cnt = 0;
	m__rendered = 0;

	for(int d=0; d<ZEDM_COUNT; d++) {

		// Draw Mode�� ���� ������Ʈ ��ȯ
		if(d==ZEDM_NONE) {
			RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
			RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		else if(d==ZEDM_ALPHAMAP) {
			RGetDevice()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000000);
			RGetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );
			RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	TRUE);

			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1 , D3DTA_TEXTURE );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP	, D3DTOP_SELECTARG1 );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2 , D3DTA_TFACTOR );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP , D3DTOP_MODULATE );

			RGetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			RGetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
		else if(d==ZEDM_ADD) {
			RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	TRUE);
			RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

			RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1 , D3DTA_TEXTURE );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP	, D3DTOP_SELECTARG1 );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP	, D3DTOP_SELECTARG1 );

			RGetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			RGetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		}
		else{
			_ASSERT(FALSE);
		}

		ZEffectList::iterator node;
		ZEffect* pEffect = NULL;

		for(node = m_Effects[d].begin(); node != m_Effects[d].end(); ) {

			pEffect = (*node);

			if( pEffect ) {

				if(pEffect->m_bRender==false)
					m__skip_cnt++;

				if( !pEffect->m_bWaterSkip ) {

					if( pEffect->Draw(nTime)==false ) {
						if(pEffect->m_bisRendered) m__rendered++;//for debug
#ifndef _PUBLISH
						g_EffectValidator.Erase(pEffect);
#endif
						delete pEffect;
						node = m_Effects[d].erase(node);
						m__cnt++;
					} else {
						if(pEffect->m_bisRendered) m__rendered++;//for debug
						++node;
						m__cnt++;
					}

				}
				else {
					++node;
				}
			}
		}

	}

	if( mode==1 ) {// �� �ۿ����� �׸���..

		m_BulletMarkList.Draw();
		m_LightFragments.Draw();

		for(int i=0;i<BILLBOARDLISTS_COUNT;i++)
			m_BillboardLists[i].Draw();

		for(int i=0;i<BILLBOARDTEXANILIST_COUNT;i++)
			m_BillBoardTexAniList[i].Draw();
	}

	if(pStateBlock) pStateBlock->Apply();
	SAFE_RELEASE( pStateBlock  );
}

void ZEffectManager::Draw(unsigned long int nTime)
{
	if (!m_bEnableDraw) return;

	LPDIRECT3DDEVICE9 pDevice = RGetDevice();

	IDirect3DStateBlock9* pStateBlock = NULL;
	RGetDevice()->CreateStateBlock(D3DSBT_PIXELSTATE, &pStateBlock);
	if(pStateBlock)	pStateBlock->Capture();

	// �׸��� ����Ʈ�� ����..

	m_ShadowList.Draw();

	// �⺻ ������Ʈ ����
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );


	// �׸��� ���� �Ÿ� ������ �� ������ ����~
	// �ذ� �ȵǸ� ������ ������ ��������~

	// ź��,ź�� ���� �������ֱ�..

	rvector camera_pos = RealSpace2::RCameraPosition;
	rvector t_vec;

	/////////////////////////////////////////////////////////////////////
	// ī�޶���� �Ÿ����� + �Ÿ��� ���� ������ ���ΰ���

	for( int d = 0; d < ZEDM_COUNT; d++ ) {

		ZEffectList::iterator node;
		ZEffect* pEffect = NULL;

		int cnt = 0;

		for(node = m_Effects[d].begin(); node != m_Effects[d].end(); ) {
			pEffect = (*node);

			if(pEffect==NULL) {

				int _size = (int)m_Effects[d].size();
				mlog("�߾�.. EffectManager NULL ���� �߻� ( %d list ���) : size : %d \n",d,_size);
				++node;

			} else {

				t_vec = camera_pos - pEffect->GetSortPos();
				pEffect->m_fDist = Magnitude(t_vec);

				// ���� �׽�Ʈ�ϰ� �۵� ��Ű��~
				pEffect->CheckRenderAble( g_nEffectLevel,pEffect->m_fDist );// �Ÿ��� ���� �׷������ΰ��� üũ

				++node;
			}
			cnt++;
		}

		m_Effects[d].sort(e_effect_sort_float);
	}

	//////////////////////////////////////////////////////////////////////

	m__skip_cnt = 0;
	m__cnt = 0;
	m__rendered = 0;

	for(int d=0; d<ZEDM_COUNT; d++) {

		// Draw Mode�� ���� ������Ʈ ��ȯ
		if(d==ZEDM_NONE) {
			RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
			RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		else if(d==ZEDM_ALPHAMAP) {
			RGetDevice()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000000);
			RGetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );
			RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	TRUE);

			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1 , D3DTA_TEXTURE );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP	, D3DTOP_SELECTARG1 );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2 , D3DTA_TFACTOR );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP , D3DTOP_MODULATE );

			RGetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			RGetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
		else if(d==ZEDM_ADD) {
			RGetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,	TRUE);
			RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

			RGetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			RGetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			RGetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1 , D3DTA_TEXTURE );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP	, D3DTOP_SELECTARG1 );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
			RGetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP	, D3DTOP_SELECTARG1 );

			RGetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			RGetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		}
		else{
			_ASSERT(FALSE);
		}

		ZEffectList::iterator node;
		ZEffect* pEffect = NULL;

		for(node = m_Effects[d].begin(); node != m_Effects[d].end(); ) {

			pEffect = (*node);

			if( pEffect ) {

				if(pEffect->m_bRender==false)
					m__skip_cnt++;

				if( pEffect->Draw(nTime)==false ) {
					if(pEffect->m_bisRendered) m__rendered++;//for debug
#ifndef _PUBLISH
					g_EffectValidator.Erase(pEffect);
#endif
					delete pEffect;
					node = m_Effects[d].erase(node);
					m__cnt++;
				} else {
					if(pEffect->m_bisRendered) m__rendered++;//for debug
					++node;
					m__cnt++;
				}
			}
		}
	}

	/////////////////////////////////////////////////////////
/*
	static char _temp[256];
	sprintf(_temp,"skip cnt : %d ,rendered :%d / %d \n", m__skip_cnt , m__rendered , m__cnt );
	OutputDebugString(_temp);
*/
	m_BulletMarkList.Draw();

	m_LightFragments.Draw();

	// TODO: �̰͵��� BeginState() �� ������ �ִ�

	for(int i=0;i<BILLBOARDLISTS_COUNT;i++)
		m_BillboardLists[i].Draw();

	for(int i=0;i<BILLBOARDTEXANILIST_COUNT;i++)
		m_BillBoardTexAniList[i].Draw();

	if(pStateBlock) pStateBlock->Apply();
	SAFE_RELEASE( pStateBlock  );
}

void ZEffectManager::Update(float fElapsed)
{
	m_LightFragments.Update(fElapsed);
	for(int i=0;i<BILLBOARDLISTS_COUNT;i++)
		m_BillboardLists[i].Update(fElapsed);

	m_BulletMarkList.Update(fElapsed);

	for(int i=0;i<BILLBOARDTEXANILIST_COUNT;i++)
		m_BillBoardTexAniList[i].Update(fElapsed);

	m_ShadowList.Update(fElapsed);
}

void ZEffectManager::AddLevelUpEffect(ZObject* pObj)
{
	ZEffect* pNew = NULL;

	ZCharacter* pChar = MDynamicCast(ZCharacter, pObj);

	if(!pChar) return;

	rvector Target = pChar->GetPosition();
	rvector dir = pChar->m_DirectionLower;

	pNew = new ZEffectLevelUp(m_pLevelUpEffect[0],Target,dir,rvector(0.f,0.f,0.f),pObj);
	((ZEffectLevelUp*)pNew)->SetAlignType(1);
	((ZEffectLevelUp*)pNew)->m_type = eq_parts_pos_info_Spine2;
	Add(pNew);

	pNew = new ZEffectLevelUp(m_pLevelUpEffect[1],Target,dir,rvector(0.f,0.f,0.f),pObj);
	((ZEffectLevelUp*)pNew)->SetAlignType(1);
	((ZEffectLevelUp*)pNew)->m_type = eq_parts_pos_info_Root;

	Add(pNew);
}

void ZEffectManager::AddReBirthEffect(const rvector& Target)
{
	ZEffect* pNew = NULL;

	rvector dir = rvector(0.f,1.f,0.f);
	pNew = new ZEffectSlash(m_pReBirthEffect,Target,dir);
	((ZEffectSlash*)pNew)->SetAlignType(1);
	Add(pNew);
}

void ZEffectManager::AddTimeRewardEffect(const rvector& Target,ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector pos = Target;

	rvector dir = -RealSpace2::RCameraDirection;// rvector(0.f,0.f,1.f);
	dir.z = 0.f;
	pos.z -= 120.f;//��~
	pNew = new ZEffectDash(m_pTimeRewardEffect,pos,dir,pObj->GetUID());
	pNew->SetEffectType(ZET_HEAL);
	((ZEffectDash*)pNew)->SetAlignType(1);
	// �������� ������ �������� ����..
	DeleteSameType((ZEffectAniMesh*)pNew);
	Add(pNew);
}

void ZEffectManager::AddLightFragment(rvector Target,rvector TargetNormal)
{
#define TARGET_FIREFRAGMENT_COUNT		5
#define TARGET_FIREFRAGMENT_MAXCOUNT	16

	if(g_nEffectLevel > Z_VIDEO_EFFECT_NORMAL)//�ϱ� ����Ʈ��� �� �׸���..
		return;
	
	else if( g_nEffectLevel == Z_VIDEO_EFFECT_NORMAL ) 
	{
		float fDistacneSQ = D3DXVec3LengthSq( &(ZGetGame()->m_pMyCharacter->GetPosition() - Target) );
		if( fDistacneSQ > 640000 ) return;
	}

	int nFireFragmentCount = 0;
	int j = 0;
	while(nFireFragmentCount<TARGET_FIREFRAGMENT_COUNT && j<TARGET_FIREFRAGMENT_MAXCOUNT){
		rvector r(float(rand()%200-100), float(rand()%200-100), float(rand()%200-100));
		if(r.x==0 && r.y==0 && r.z==0) continue;
		if(DotProduct(r, TargetNormal)<0) continue;

#define RANDOM_POSITION_MAXDISTANCE	3
		rvector rp(RANDOM_POSITION_MAXDISTANCE*(rand()%200-100)/100.0f, RANDOM_POSITION_MAXDISTANCE*(rand()%200-100)/100.0f, RANDOM_POSITION_MAXDISTANCE*(rand()%200-100)/100.0f);
#define LIGHTTRACER_SPEED	4.0f	// m/s
		Normalize(r);

		//������ : 1.0f, 1.0f by ����

		m_LightFragments.Add(Target+rp,r*LIGHTTRACER_SPEED*50.f,rvector(0,0,-500.f),1.6f,1.6f,1.2f);
		// LifeTime�� �⺻���� 2.0f

		nFireFragmentCount++;
	}
}

#pragma optimize ( "", off )

void ZEffectManager::AddDashEffect(rvector& Target,rvector& TargetNormal,ZObject* pObj)
{
	// ������� : ����Ÿ�ֿ̹� pObj�� �ùٸ��� �˰�ʹ�
//	char szLastName[64];
//	strcpy(szLastName,pObj->GetProperty()->szName);

	if(!pObj->IsVisible()) return;

	ZEffect* pNew = NULL;
	pNew = new ZEffectDash(m_pDashEffect,Target,TargetNormal,pObj->GetUID());
	((ZEffectSlash*)pNew)->SetAlignType(1);
	Add(pNew);
}
#pragma optimize ( "", on)

void ZEffectManager::AddSkillDashEffect(rvector& Target,rvector& TargetNormal,ZObject* pObj)
{
	ZEffect* pNew = NULL;
	pNew = new ZEffectDash(m_pDaggerUpper,Target,TargetNormal,pObj->GetUID());
	((ZEffectSlash*)pNew)->SetAlignType(1);
	Add(pNew);
}

void ZEffectManager::AddLandingEffect(rvector& Target, rvector& TargetNormal)
{
	// �ɼǿ� ����? �߱� �̻�?
	if(g_nEffectLevel > Z_VIDEO_EFFECT_NORMAL)//�ϱ� ����Ʈ��� �� �׸���..
		return;

#define LANDING_SMOKE_MAX_SCALE				70.0f	
#define LANDING_SMOKE_MIN_SCALE				70.0f	
#define LANDING_SMOKE_LIFE_TIME				3000

	ZEffect* pNew = NULL;

	Target.z += 5.f;

	pNew = new ZEffectLandingSmoke(m_pEBSLanding, Target, 
		TargetNormal, LANDING_SMOKE_MIN_SCALE, LANDING_SMOKE_MAX_SCALE, LANDING_SMOKE_LIFE_TIME);

	Add(pNew);

}

void ZEffectManager::AddBulletMark(rvector& Target, rvector& TargetNormal)
{
	if(g_nEffectLevel > Z_VIDEO_EFFECT_NORMAL) return; // �ϱ��� ����~

	m_BulletMarkList.Add(Target+TargetNormal,TargetNormal);

	ZEffect* pNew = NULL;
	pNew = new ZEffectSlash(m_pBulletOnWallEffect[rand()%BULLETONWALL_COUNT],Target,TargetNormal);
	((ZEffectSlash*)pNew)->SetAlignType(1);
	Add(pNew);
}

void ZEffectManager::AddTrackMagic(rvector &pos)
{
	int Add = rand() % 10;
	float fStartSize = 16 + Add;
	float fEndSize = 28 + Add;
	float fLife = 1.0f;// + (1+Add) / 10.f;

	int frame = rand()%4;
	rvector vel = rvector(0,0,-25.f);

	m_BillBoardTexAniList[3].Add( pos, vel,frame, 0.f,fStartSize , fEndSize, fLife );
}

void ZEffectManager::AddTrackFire(rvector &pos)
{
	int Add = rand() % 20;
	float fStartSize = 10 + Add;
	float fEndSize = 20 + Add;
	float fLife = 0.4f;// + (1+Add) / 10.f;
	rvector vel = rvector(0,0,25);

	m_BillBoardTexAniList[1].Add( pos, vel, 0, 0.f,fStartSize , fEndSize, fLife );
}

void ZEffectManager::AddTrackCold(rvector &pos)
{
	int Add = rand() % 10;
	float fStartSize = 8 + Add;
	float fEndSize = 14 + Add;
	float fLife = 1.0f;// + (1+Add) / 10.f;

	int frame = rand()%8;
	rvector vel = rvector(0,0,-25.f);

	m_BillBoardTexAniList[2].Add( pos, vel,frame, 0.f,fStartSize , fEndSize, fLife );
}

void ZEffectManager::AddTrackPoison(rvector &pos)
{
	int Add = rand() % 10;
	float fStartSize = 10 + Add;
	float fEndSize = 20 + Add;
	float fLife = 1.0f;// + (1+Add) / 10.f;

	static int r_frame[4] = { 8,9,12,13 };

	int frame = rand()%4;

	rvector vel = rvector(0,0,0);

	m_BillBoardTexAniList[2].Add( pos, vel,r_frame[frame],0.f, fStartSize , fEndSize, fLife );
}

void ZEffectManager::AddTrackMethor(rvector &pos)
{
	int Add = rand() % 60;
	float fStartSize = 80 + Add;
	float fEndSize = 100 + Add;
	float fLife = 1.0f;

	rvector vel = rvector(0,0,0);
	rvector add = 50.f*rvector(RANDOMFLOAT-0.5f,RANDOMFLOAT-0.5f,RANDOMFLOAT-0.5f);

	m_BillBoardTexAniList[4].Add( pos + add, vel, 0, 0.f,fStartSize , fEndSize, fLife );

//	m_BillboardLists[4].SetVanishTime(2.9f);
	
	add = 50.f*rvector(RANDOMFLOAT-0.5f,RANDOMFLOAT-0.5f,RANDOMFLOAT-0.5f);
	m_BillboardLists[4].Add(pos,add,rvector(0,0,0), 30.f, 120.f, 3.f );
}


static RMeshPartsPosInfoType g_EnchantEffectPartsPos[5]=
{
	eq_parts_pos_info_Head,
	eq_parts_pos_info_Spine,
	eq_parts_pos_info_Spine1,
	eq_parts_pos_info_LUpperArm,
	eq_parts_pos_info_RUpperArm
};

#define EFRand rand() % 15
#define EFRandTime (1 + rand() % 6) / 10.f

rvector GetRandVec(int V)
{
	rvector v;

	v.x = rand()%10; if(rand()%2) v.x = -v.x;
	v.y = rand()%10; if(rand()%2) v.y = -v.y;
	v.z = rand()%10; if(rand()%2) v.z = -v.z;

	return v;
}

// ������ ���õȰ� ������ ����..

int GetRandType(int nRand,int nEx,int nMax)
{
	int hr = 0;
	int _rand = 0;

	for(int i=0;i<nMax;i++)
	{
		_rand = rand() % nRand;

		if(_rand != nEx)
			return _rand;
	}

	return _rand;
}

float ZEffectManager::GetEnchantDamageObjectSIze(ZObject* pObj)
{
	float fSize = 1.f; // ĳ���� ����..

	ZActor* pActor = MDynamicCast(ZActor,pObj);

	if(pActor&&pActor->GetNPCInfo()) { 

		// ���ŷ�� 70 
		// ���� ǥ�� 35.f ����..

		fSize = pActor->GetNPCInfo()->fCollRadius / 35.f;
		fSize *= fSize;
	}

	return fSize;
}

void ZEffectManager::AddEnchantFire2(ZObject* pObj)
{
	if(pObj==NULL) return;

	float fSize = GetEnchantDamageObjectSIze( pObj );

	float fStartSize = (10.f + EFRand) * fSize;
	float fEndSize = (20.f + EFRand) * fSize;
	float fLife = 1.0f;

	rvector pos,parts_pos;
	rvector vel = rvector(0.f,0.f,25.f);

	static int partstype;
	
	partstype = GetRandType(5,partstype,10);

	rvector camera_dir = RCameraDirection * 20.f * fSize;

	parts_pos = pObj->m_pVMesh->GetBipTypePosition( g_EnchantEffectPartsPos[partstype] );

	if( Magnitude(parts_pos) < 0.1f ) // ���� ��ã�� ��
		return;

	pos = parts_pos - camera_dir;

	pos += GetRandVec(10);

	m_BillBoardTexAniList[0].Add(pos,vel, 0, 0 ,fStartSize, fEndSize, fLife );
}

void ZEffectManager::AddEnchantCold2(ZObject* pObj)
{
	if(pObj==NULL) return;

	float fSize = GetEnchantDamageObjectSIze( pObj );

	float fStartSize = (10.f + EFRand) * fSize;
	float fEndSize = (20.f + EFRand) * fSize;
	float fLife = 1.0f;

	rvector pos;
	rvector vel = rvector(0.f,0.f,-25.f);

	static int partstype;

	partstype = GetRandType(5,partstype,10);

	rvector camera_dir = RCameraDirection * 20.f * fSize;

	pos = pObj->m_pVMesh->GetBipTypePosition( g_EnchantEffectPartsPos[partstype] ) - camera_dir;

	pos += GetRandVec(10);

	int nTex = rand()%7;// ( 0 ~ 6 )

	m_BillBoardTexAniList[2].Add(pos,vel, nTex, 0 ,fStartSize, fEndSize, fLife );
}

void ZEffectManager::AddEnchantPoison2(ZObject* pObj)
{
	if(pObj==NULL) return;

	float fSize = GetEnchantDamageObjectSIze( pObj );

	float fStartSize = (10.f + EFRand) * fSize;
	float fEndSize = (20.f + EFRand) * fSize;
	float fLife = 1.0f;

	rvector pos;
	rvector vel = rvector(0,0,0);

	static int partstype;
	
	partstype = GetRandType(5,partstype,10);

	rvector camera_dir = RCameraDirection * 20.f * fSize;

	pos = pObj->m_pVMesh->GetBipTypePosition( g_EnchantEffectPartsPos[partstype] ) - camera_dir;

	pos += GetRandVec(10);

	static int _tex_data[] = {8,9,12,13};

	int nTex = _tex_data[ rand()%4 ];

	m_BillBoardTexAniList[2].Add(pos,vel, nTex, 0 ,fStartSize, fEndSize, fLife );
}
/*
void ZEffectManager::AddEnchantFire(ZObject* pObj)
{
	// ��þƮ �����ۿ� ���� ĳ������ ���... ���ӽð����� ���� ������ �ٿ��ش�..
	// pos �� �پ�� �ϴ� ������ Ÿ������ �׷������� ����Ѵ�.


	if(pObj==NULL) return;

	int Add = 0;

	float fStartSize = 10;
	float fEndSize = 20;
	float fLife = 1.3f;// + (1+Add) / 10.f;

	rvector pos;
	rvector vel = rvector(0,0,0);
   
	rvector camera_dir = RCameraDirection * 30.f;

	Add = EFRand;
	pos = pObj->m_pVMesh->GetBipTypePosition( eq_parts_pos_info_Head ) - camera_dir;
	m_BillBoardTexAniList[0].Add(pos,vel, 0, EFRandTime,fStartSize+Add, fEndSize+Add, fLife );

	Add = EFRand;
	pos = pObj->m_pVMesh->GetBipTypePosition( eq_parts_pos_info_Spine1 ) - camera_dir;
	m_BillBoardTexAniList[0].Add(pos,vel, 0, EFRandTime,fStartSize+Add, fEndSize+Add, fLife );

	Add = EFRand;
	pos = pObj->m_pVMesh->GetBipTypePosition( eq_parts_pos_info_Spine2 ) - camera_dir;
	m_BillBoardTexAniList[0].Add(pos,vel, 0, EFRandTime,fStartSize+Add , fEndSize+Add, fLife );

	Add = EFRand;
	pos = pObj->m_pVMesh->GetBipTypePosition( eq_parts_pos_info_LUpperArm ) - camera_dir;
	m_BillBoardTexAniList[0].Add(pos,vel, 0, EFRandTime,fStartSize+Add , fEndSize+Add, fLife );

	Add = EFRand;
	pos = pObj->m_pVMesh->GetBipTypePosition( eq_parts_pos_info_RUpperArm ) - camera_dir;
	m_BillBoardTexAniList[0].Add(pos,vel, 0, EFRandTime,fStartSize+Add , fEndSize+Add, fLife );

}

void ZEffectManager::AddEnchantCold(ZObject* pObj)
{
	int Add = rand() % 20;
	float fStartSize = 30 + Add;
	float fEndSize = 40 + Add;
	float fLife = 1.3f;// + (1+Add) / 10.f;
	rvector pos = rvector(0,0,0);
	rvector vel = rvector(0,0,0);

	// cold frame �� 0-7 ����

	m_BillBoardTexAniList[2].Add(pos,vel, 0, 0.1f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_Head);
	m_BillBoardTexAniList[2].Add(pos,vel, 1, 0.2f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_Spine);
	m_BillBoardTexAniList[2].Add(pos,vel, 2, 0.3f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_LHand);
	m_BillBoardTexAniList[2].Add(pos,vel, 3, 0.4f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_LUpperArm);
	m_BillBoardTexAniList[2].Add(pos,vel, 5, 0.5f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_RHand);
	m_BillBoardTexAniList[2].Add(pos,vel, 6, 0.6f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_RUpperArm);

}

void ZEffectManager::AddEnchantPoison(ZObject* pObj)
{
	int Add = rand() % 20;
	float fStartSize = 30 + Add;
	float fEndSize = 40 + Add;
	float fLife = 1.3f;// + (1+Add) / 10.f;
	rvector pos = rvector(0,0,0);
	rvector vel = rvector(0,0,0);

	// poison frame �� 8,9,12,13 ��..

	m_BillBoardTexAniList[2].Add(pos,vel,  8, 0.1f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_Head);
	m_BillBoardTexAniList[2].Add(pos,vel,  9, 0.2f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_Spine);
	m_BillBoardTexAniList[2].Add(pos,vel, 12, 0.3f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_LHand);
	m_BillBoardTexAniList[2].Add(pos,vel, 13, 0.4f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_LUpperArm);
	m_BillBoardTexAniList[2].Add(pos,vel,  9, 0.5f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_RHand);
	m_BillBoardTexAniList[2].Add(pos,vel, 12, 0.6f, fStartSize , fEndSize, fLife ,pObj,eq_parts_pos_info_RUpperArm);

}

*/

void ZEffectManager::AddShotgunEffect(rvector &pos,rvector &out,rvector &dir,ZObject* pChar )
{
	ZEffect * pNew = NULL;

#define SHOTGUN_SMOKE_LIFE_TIME		0.9f
#define SHOTGUN_SMOKE_MAX_SCALE		60.0f	// �ִ�� ������ �� �ִ� ũ�� ��
#define SHOTGUN_SMOKE_MIN_SCALE		100.0f	// �ִ�� ������ �� �ִ� ũ�� �� - �Ϻη� ���ٷ� �������(by ����)

	rvector _pos = pos;

	for(int i=0;i<1;i++)
	{
		_pos = rvector(pos.x + rand()%30,pos.y + rand()%30,pos.z + rand()%30);
		AddSmokeEffect(m_pEBSMuzzleSmokeShotgun[rand()%MUZZLESMOKE_SHOTGUN_COUNT], _pos, rvector(0,0,0), rvector(0,0,0), SHOTGUN_SMOKE_MIN_SCALE , SHOTGUN_SMOKE_MAX_SCALE, SHOTGUN_SMOKE_LIFE_TIME);
	}

	rvector _dir=rvector(0,0,1);

	if(pChar) {
		if(pChar->m_pVMesh) {
			rmatrix* mat = &pChar->m_pVMesh->m_WeaponDummyMatrix[weapon_dummy_muzzle_flash];

			_dir.x = mat->_21;
			_dir.y = mat->_22;
			_dir.z = mat->_23;
		}
	}


	pNew = NULL;
	pNew = new ZEffectShot(m_pFlameShotgun, pos, _dir , NULL);
	((ZEffectShot*)pNew)->SetAlignType(1);

	Add(pNew);

#define EM_VELOCITY		2.0f	// meter/sec
#define EM_RANDOM_SCALE	0.5f

	rvector right;
	CrossProduct(&right,-dir,rvector(0, 0, 1));
	Normalize(right);

	rvector EMRandom((rand()%100)/100.0f, (rand()%100)/100.0f, (rand()%100)/100.0f);
	EMRandom *= EM_RANDOM_SCALE;
	rvector EMVelocity = (right+rvector(0, 0, 1) + EMRandom) * EM_VELOCITY;
	pNew = new ZEffectStaticMesh( m_pMeshEmptyCartridges[1], out, EMVelocity, pChar->GetUID() );
	Add(pNew); pNew = NULL;
}

void ZEffectManager::AddShotEffect(rvector* pSource,int size, rvector& Target, rvector& TargetNormal, ZTargetType nTargetType,
								   MMatchWeaponType wtype, bool bSlugOutput ,ZObject* pObj,bool bDrawFireEffects,bool bDrawTargetEffects)
{
	// bDrawFireEffects : �߻�Ǵ��� ����Ʈ
	// bDrawTargetEffects : �´��� ����Ʈ �׸��� ����



	rvector Source  = pSource[0];//�ѱ���ġ
	rvector SourceL = pSource[3];//�ѱ���ġ

	rvector right;
	CrossProduct(&right,TargetNormal,rvector(0, 0, 1));
	Normalize(right);

	rvector GTargetL = Target + right*(5+(rand()%10));
	rvector GTargetR = Target - right*(5+(rand()%10));

	// ���� �߻�Ǵ� ����
	rvector TargetDir = Target - Source;
	Normalize(TargetDir);

	ZEffect* pNew = NULL;

	// ����ź
	if(rand()%5==0){
		pNew = new ZEffectLightTracer(m_pEBSLightTracer, Source, Target);
		Add(pNew); pNew = NULL;
	}

	if(bDrawFireEffects) {

		// ���� ����
#define SMOKE_MAX_SCALE				30.0f	// �ִ�� ������ �� �ִ� ũ�� ��
#define SMOKE_MIN_SCALE				90.0f	// �ִ�� ������ �� �ִ� ũ�� �� - �Ϻη� ���ٷ� �������(by ����)
#define SMOKE_LIFE_TIME				0.6f	// Smoke Life Time
#define SMOKE_SHOTGUN_LIFE_TIME		300		// Smoke Shotgun Life Time - ���� ���� �ʴ�(by ����)
#define SMOKE_ACCEL					rvector(0,0,50.f)	// Smoke ���ӵ�
#define SMOKE_VELOCITY				110.f

		if(wtype==MWT_ROCKET) {//rluncher
			pNew = NULL;
		} 
		else if(wtype==MWT_MACHINEGUN) {//ǳ���ϰ�~

			rvector _Add;

			float min_scale = SMOKE_MIN_SCALE;
			float max_scale = SMOKE_MAX_SCALE;
			DWORD life = SMOKE_LIFE_TIME;

			for(int i=0;i<1;i++) {

				min_scale = SMOKE_MIN_SCALE * (rand()%2);
				max_scale = SMOKE_MAX_SCALE * (rand()%2);
				life	  = SMOKE_LIFE_TIME * (rand()%3);

					if(i%4==0)	_Add = rvector(-rand()%20 , rand()%20 , rand()%20 );
				else if(i%4==1)	_Add = rvector( rand()%20 ,-rand()%20 , rand()%20 );
				else if(i%4==2)	_Add = rvector( rand()%20 , rand()%20 ,-rand()%20 );
				else if(i%4==3)	_Add = rvector( rand()%20 , rand()%20 , rand()%20 );

				AddSmokeEffect(m_pEBSMuzzleSmoke[rand()%MUZZLESMOKE_COUNT], Source+_Add, SMOKE_VELOCITY*TargetDir,SMOKE_ACCEL, min_scale, max_scale, SMOKE_LIFE_TIME );
			}
		}
		else {
			AddSmokeEffect(m_pEBSMuzzleSmoke[rand()%MUZZLESMOKE_COUNT], Source, SMOKE_VELOCITY*TargetDir,SMOKE_ACCEL, SMOKE_MIN_SCALE, SMOKE_MAX_SCALE, SMOKE_LIFE_TIME);
		}

		// ���� ȭ��
		rvector _dir=rvector(0,0,1);

		if(pObj) {
			if(pObj->m_pVMesh) {
				rmatrix* mat = &pObj->m_pVMesh->m_WeaponDummyMatrix[weapon_dummy_muzzle_flash];
				
				_dir.x = mat->_21;
				_dir.y = mat->_22;
				_dir.z = mat->_23;
			}
		}

		Normalize(_dir);

		switch(wtype) {
			case MWT_PISTOL:
			case MWT_PISTOLx2:
			case MWT_REVOLVER:
			case MWT_REVOLVERx2:
			case MWT_SMG:
			case MWT_SMGx2:
				{
					pNew = new ZEffectShot(m_pFlamePistol, Source , _dir, pObj);
					((ZEffectShot*)pNew)->SetAlignType(1);
				}
				break;
			case MWT_RIFLE:
				{
					pNew = new ZEffectShot(m_pFlameRifle, Source , _dir , pObj);
					((ZEffectShot*)pNew)->SetAlignType(1);
				}
				break;
			case MWT_MACHINEGUN:
				{
					pNew = new ZEffectShot(m_pFlameMG, Source , _dir , pObj);
					((ZEffectShot*)pNew)->SetAlignType(1);
				}
				break;
			default:
				pNew = NULL;
				break;
		}

		Add(pNew); pNew = NULL;

		//��� ���� ���

		if(size==6) {//�ֱ���

			rvector _dir = GTargetR-Source;//���⿡ ���� �ణ�� ��Ʋ������
			Normalize(_dir);

			// �ֱ��� ���� ȭ��
			pNew = new ZEffectShot(m_pFlamePistol, Source, _dir,pObj);
			((ZEffectShot*)pNew)->SetStartTime(120);
			((ZEffectShot*)pNew)->SetIsLeftWeapon(true);

			Add(pNew); pNew = NULL;

			// �ֱ��� ���� ����
			AddSmokeEffect(m_pEBSMuzzleSmoke[rand()%MUZZLESMOKE_COUNT], Source, SMOKE_VELOCITY*TargetDir,SMOKE_ACCEL, SMOKE_MIN_SCALE, SMOKE_MAX_SCALE, SMOKE_LIFE_TIME);
		}

		// ź��

		// �ϱ��� ����,�߱��� �����ϳ�,����� �� �׸���

		bool bRender = true;

		if(g_nEffectLevel==Z_VIDEO_EFFECT_HIGH) {
			bRender = true;
		}
		else if(g_nEffectLevel==Z_VIDEO_EFFECT_NORMAL) {
			static bool toggle = true;
			if(toggle) {
				bRender = true;
			}
			else {
				bRender = false;
			}
			toggle = !toggle;
		}
		else if(g_nEffectLevel==Z_VIDEO_EFFECT_LOW) {
			bRender = false;
		}

	
		if(bRender) {
		
			ZCharacterObject* pCObj = MDynamicCast(ZCharacterObject, pObj);
			if(pCObj)
			{
				if(wtype!=MWT_SHOTGUN && pCObj->IsHero() && bSlugOutput == true)	// ������ �Ѿ� �Ѿ��� ź�� ����
				{
				
		#define EM_VELOCITY		2.0f	// meter/sec
		#define EM_RANDOM_SCALE	0.5f

					rvector EMRandom((rand()%100)/100.0f, (rand()%100)/100.0f, (rand()%100)/100.0f);
					EMRandom*=EM_RANDOM_SCALE;
					rvector EMVelocity = (right+rvector(0, 0, 1)+EMRandom)* EM_VELOCITY;
					pNew = new ZEffectStaticMesh(m_pMeshEmptyCartridges[0], pSource[1], EMVelocity, pObj->GetUID());
					Add(pNew); pNew = NULL;

					if(size==6) {//�����..
						rvector EMVelocityL = (-right+rvector(0, 0, 1)+EMRandom)* EM_VELOCITY;
						pNew = new ZEffectStaticMesh(m_pMeshEmptyCartridges[0], pSource[4], EMVelocityL, pObj->GetUID() );
						Add(pNew); pNew = NULL;
					}
				}

			}
		}
	}

	if(bDrawTargetEffects) {
		if(nTargetType==ZTT_CHARACTER_GUARD) {

			AddLightFragment(Target,TargetNormal);
		}

		if(nTargetType==ZTT_OBJECT){	// Ÿ���� ���϶�

			// Ÿ�� ��ź ����

			if(size==6) { //�ֱ��ѷ�
				AddBulletMark(GTargetL,TargetNormal);
				AddBulletMark(GTargetR,TargetNormal);
			}
			else {

				AddBulletMark(Target,TargetNormal);
			}

			if(wtype != MWT_SHOTGUN) // ������ �Ѿ� �Ѿ��� ���� ����
			{
				// Ÿ�� ��ź ����
			#define TARGET_SMOKE_MAX_SCALE		50.0f
			#define TARGET_SMOKE_MIN_SCALE		40.0f
			#define TARGET_SMOKE_LIFE_TIME		0.9f
			#define TARGET_SMOKE_VELOCITY		0.2f				// meter/sec
			#define TARGET_SMOKE_ACCEL			rvector(0,0,100.f)	// meter/sec

				int max_cnt = 0;

					if(g_nEffectLevel==Z_VIDEO_EFFECT_HIGH)		max_cnt = 3;
				else if(g_nEffectLevel==Z_VIDEO_EFFECT_NORMAL)	max_cnt = 2;
				else if(g_nEffectLevel==Z_VIDEO_EFFECT_LOW)		max_cnt = 1;

				if(max_cnt) {
				
					for(int i=0; i<max_cnt; i++) {
						rvector p = Target+TargetNormal*TARGET_SMOKE_MIN_SCALE*float(i)*0.5f + rvector(fmod((float)rand(), TARGET_SMOKE_MIN_SCALE), fmod((float)rand(), TARGET_SMOKE_MIN_SCALE), fmod((float)rand(), TARGET_SMOKE_MIN_SCALE));
						float fSize = 1.0f+float(rand()%100)/100.0f;
						AddSmokeEffect(m_pEBSSmokes[rand()%SMOKE_COUNT], p, TargetNormal*TARGET_SMOKE_VELOCITY,rvector(0,100.f,0), TARGET_SMOKE_MIN_SCALE*fSize, TARGET_SMOKE_MAX_SCALE*fSize, TARGET_SMOKE_LIFE_TIME);
						
					}
				}
			}

		}
		else if(nTargetType==ZTT_CHARACTER){	// Ÿ���� ����϶�
			// �� ����
		#define TARGET_BLOOD_MAX_SCALE		50.0f
		#define TARGET_BLOOD_MIN_SCALE		20.0f
		#define TARGET_BLOOD_LIFE_TIME		500
		#define TARGET_BLOOD_VELOCITY		4.0f	// meter/sec

			/*	// �� ����
			for(int i=0; i<3; i++){
				rvector p = Target+TargetNormal*TARGET_BLOOD_MIN_SCALE*float(i)*0.5f + rvector(fmod((float)rand(), TARGET_BLOOD_MIN_SCALE), fmod((float)rand(), TARGET_BLOOD_MIN_SCALE), fmod((float)rand(), TARGET_BLOOD_MIN_SCALE));
				float fSize = 1.0f+float(rand()%100)/100.0f;
				AddSmokeEffect(m_pEBSBloods[rand()%BLOOD_COUNT], p, TargetNormal*TARGET_BLOOD_VELOCITY, TARGET_BLOOD_MIN_SCALE*fSize, TARGET_BLOOD_MAX_SCALE*fSize, TARGET_BLOOD_LIFE_TIME);
				Add(pNew);
			}
			*/
			
			// low �� �ƴ� �Ÿ����..

			static DWORD last_add_time = timeGetTime();
			static DWORD this_time;

			this_time = timeGetTime();

			// ����ϰ�� �´� ���� ������ ����Ʈ

			if(rand()%3==0)
			{
				pNew = new ZEffectSlash(m_pRangeDamaged[rand()%6],Target,TargetNormal);	
				((ZEffectSlash*)pNew)->SetScale(rvector(1.0f,1.0f,1.0f));
				//		((ZEffectSlash*)pNew)->SetAlignType(1);
				Add(pNew);
			}

			// ���� �� Ƣ��
			/*
			for(int i=0; i<3; i++){
				pNew = new ZEffectBulletMark(m_pEBSBloodMark[rand()%BLOODMARK_COUNT], Target+TargetNormal, TargetNormal);
				Add(pNew);
			}
			*/
		}
	}
}

void ZEffectManager::AddSwordWaveEffect(rvector& Target,DWORD start_time,ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector dir = pObj->GetDirection();//rvector(0.f,1.f,0.f);
	dir.z = 0.f;
	Normalize(dir);

	pNew = new ZEffectSlash(m_pSwordWaveEffect[1],Target,dir);
//	((ZEffectSlash*)pNew)->SetScale(rvector(3.5f,3.5f,3.5f));
	((ZEffectSlash*)pNew)->SetAlignType(1);
//	((ZEffectSlash*)pNew)->SetStartTime(start_time);
//	((ZEffectSlash*)pNew)->SetDelayPos(pObj->m_UID);

	Add(pNew);
}

void ZEffectManager::AddSwordEnchantEffect(ZC_ENCHANT type,const rvector& Target,DWORD start_time, float fScale)
{
	ZEffect* pNew = NULL;

	rvector dir = rvector(0.f,1.f,0.f);

	RMesh* pMesh = NULL;

		 if( type ==  ZC_ENCHANT_FIRE )			pMesh = m_pSwordEnchantEffect[0];
	else if( type ==  ZC_ENCHANT_COLD )			pMesh = m_pSwordEnchantEffect[1];
	else if( type ==  ZC_ENCHANT_LIGHTNING )	pMesh = m_pSwordEnchantEffect[2];
	else if( type ==  ZC_ENCHANT_POISON )		pMesh = m_pSwordEnchantEffect[3];
	else 
		return;


	pNew = new ZEffectSlash(pMesh,Target,dir);
	((ZEffectSlash*)pNew)->SetScale(rvector(fScale,fScale,fScale));
	((ZEffectSlash*)pNew)->SetAlignType(1);
	((ZEffectSlash*)pNew)->SetStartTime(start_time);
//	((ZEffectSlash*)pNew)->SetDelayPos(pObj->m_UID);
	Add(pNew);
}

void ZEffectManager::AddMagicEffect(rvector& Target,DWORD start_time, float fScale)
{
	ZEffect* pNew = NULL;

	rvector dir = rvector(0.f,1.f,0.f);

	pNew = new ZEffectSlash(m_pMagicDamageEffect,Target,dir);
	((ZEffectSlash*)pNew)->SetScale(rvector(fScale,fScale,fScale));
	((ZEffectSlash*)pNew)->SetAlignType(1);
	((ZEffectSlash*)pNew)->SetStartTime(start_time);
//	((ZEffectSlash*)pNew)->SetDelayPos(pObj->m_UID);
	Add(pNew);
}
/*
void ZEffectManager::AddLighteningEffect(rvector& Target)
{
	ZEffect* pNew = NULL;

	rvector dir = rvector(0.f,1.f,0.f);

	pNew = new ZEffectSlash(m_pLighteningEffect,Target,dir);

	((ZEffectSlash*)pNew)->SetAlignType(1);
//	((ZEffectSlash*)pNew)->SetStartTime(start_time);

	Add(pNew);
}
*/

void ZEffectManager::AddMethorEffect(rvector& Target,int nCnt)
{
	ZEffect* pNew = NULL;

	rvector dir		= rvector(0.f,1.f,0.f);
	rvector AddPos	= rvector(0.f,0.f,0.f);

	RMesh* pMesh = m_pEffectMeshMgr->Get("ef_methor");
	
	pNew = new ZEffectSlash(pMesh,Target,dir);

	((ZEffectSlash*)pNew)->SetAlignType(1);
	((ZEffectSlash*)pNew)->SetStartTime(0);
	((ZEffectSlash*)pNew)->m_nAutoAddEffect = ZEffectAutoAddType_Methor;

	Add(pNew);
	
	// ���ϴ� ������ŭ ���⸦ �� �Ѹ���..
/*
	for(int i=0;i<nCnt;i++) {

		AddPos.x = rand() % 100;
		AddPos.y = rand() % 100;
		AddPos.z = 0.f;

		if(rand()%2) AddPos.x = -AddPos.x;
		if(rand()%2) AddPos.y = -AddPos.y;

		AddPos = AddPos + Target;

		pNew = new ZEffectSlash(m_pBlizzardEffect,AddPos,dir);

		((ZEffectSlash*)pNew)->SetAlignType(1);
		((ZEffectSlash*)pNew)->SetStartTime( i * 200 );

		Add(pNew);
	}
*/
}

void ZEffectManager::AddBlizzardEffect(rvector& Target,int nCnt)
{
	ZEffect* pNew = NULL;

	rvector dir = rvector(0.f,1.f,0.f);
	rvector AddPos;

	// ���ϴ� ������ŭ �� �Ѹ���..

	for(int i=0;i<nCnt;i++) {

		AddPos.x = rand() % 100;
		AddPos.y = rand() % 100;
		AddPos.z = 0.f;

		if(rand()%2) AddPos.x = -AddPos.x;
		if(rand()%2) AddPos.y = -AddPos.y;

		AddPos = AddPos + Target;

		pNew = new ZEffectSlash(m_pBlizzardEffect,AddPos,dir);

		((ZEffectSlash*)pNew)->SetAlignType(1);
		((ZEffectSlash*)pNew)->SetStartTime( i * 200 );

		Add(pNew);
	}
}


void ZEffectManager::AddMagicEffectWall(int type,rvector& Target,rvector& vDir,DWORD start_time, float fScale)
{
	ZEffect* pNew = NULL;

//	rvector dir = rvector(0.f,1.f,0.f);

	RMesh* pMesh = NULL;

		 if( type ==  0 )	pMesh = m_pMagicEffectWall[0];
	else if( type ==  1 )	pMesh = m_pMagicEffectWall[1];
	else if( type ==  2 )	pMesh = m_pMagicEffectWall[2];
	else 
		return;


	pNew = new ZEffectSlash(pMesh,Target,vDir);
	((ZEffectSlash*)pNew)->SetScale(rvector(fScale,fScale,fScale));
	((ZEffectSlash*)pNew)->SetAlignType(1);
	((ZEffectSlash*)pNew)->SetStartTime(start_time);
//	((ZEffectSlash*)pNew)->SetDelayPos(pObj->m_UID);
	Add(pNew);

}


void ZEffectManager::AddSwordDefenceEffect(rvector& Target,rvector& vDir)
{
	ZEffect* pNew = NULL;

#define _SPREAD_SWORD_DEFENCE_EFFECT 50

	rvector add;

	add.x = rand()%_SPREAD_SWORD_DEFENCE_EFFECT;
	add.y = rand()%_SPREAD_SWORD_DEFENCE_EFFECT;
	add.z = rand()%_SPREAD_SWORD_DEFENCE_EFFECT;

	if(rand()%2) add.x=-add.x;
	if(rand()%2) add.y=-add.y;
	if(rand()%2) add.z=-add.z;

	float rot_angle = 3.14f/8.f * (rand()%16);

	int mode = rand()%3;

	pNew = new ZEffectSlash(m_pSwordDefenceEffect[mode],Target+add,vDir);
	((ZEffectSlash*)pNew)->SetRotationAngleZ(rot_angle);
	((ZEffectSlash*)pNew)->SetAlignType(1);
	Add(pNew);
}

void ZEffectManager::AddWaterSplashEffect( rvector& Target, rvector& Scale  )
{
	rvector dir = -RealSpace2::RCameraDirection;
 	ZEffect* pNew	= new ZEffectSlash( m_pWaterSplash, Target, dir );
	((ZEffectSlash*)pNew)->SetScale( Scale );

	Add(pNew);
}

void ZEffectManager::AddRocketEffect(rvector& Target, rvector& TargetNormal)
{
	ZEffect* pNew = NULL;

	// Ÿ�� ���� ����

	rvector vv = Target;
	vv.z-=50;//����ź��� ���̽Ἥ �� ��ü�� �ణ����
	pNew = new ZEffectSlash(m_pRocketEffect,vv,TargetNormal);
	((ZEffectSlash*)pNew)->SetAlignType(1);
	((ZEffectSlash*)pNew)->SetDistOption(29999.f,29999.f,29999.f);//�� �����ش�~
	Add(pNew);

	rvector _add;
	float _min,_max;
	DWORD _life;

	for(int i=0;i<2;i++) {

		_add.x = rand()%30;
		_add.y = rand()%30;
		_add.z = rand()%30;

		_min = 100 + (rand()%100)/2.f;
		_max = 200 + (rand()%200)/3.f;

		_life = 3000.f + (1000*(rand()%6/3.f));

		AddGrenadeSmokeEffect(Target+_add,_min,_max,_life);
	}

}

bool ZEffectManager::RenderCheckEffectLevel()
{
	bool bRender = true;

	if(g_nEffectLevel==Z_VIDEO_EFFECT_HIGH) {	bRender = true;	}
	else if(g_nEffectLevel==Z_VIDEO_EFFECT_NORMAL) {
		static bool toggle = true;
		if(toggle)	bRender = true;
		else 		bRender = false;
		toggle = !toggle;
	}
	else if(g_nEffectLevel==Z_VIDEO_EFFECT_LOW) { bRender = false; }

	return bRender;
}
// �߷¾��� �ڱ� �������θ�...
void ZEffectManager::AddMapSmokeSTEffect(rvector& Target,rvector& dir,rvector& acc,rvector& acc2,DWORD scolor,DWORD delay,float fLife,float fStartScale,float fEndScale)
{
	if( !RenderCheckEffectLevel() )	return;
	
	static DWORD _color[] = {
		0x858585,
		0x909090,
		0x959595,
		0xa0a0a0,
		0xa5a5a5,
		0xb0b0b0,
		0xb5b5b5,
		0xc5c5c5,
	};

	m_BillboardLists[4].SetVanishTime(2.9f);

	rvector _Acc = acc;

	DWORD tcolor = _color[rand()%8];

	if(scolor!=0x01010101) {

		D3DXCOLOR src_color(tcolor);
		D3DXCOLOR dest_color(scolor);

		float r = src_color.r * dest_color.r;
		float g = src_color.g * dest_color.g;
		float b = src_color.b * dest_color.b;

#define CLIP_COLOR(c) min(max(c,0.f),1.f)

		tcolor=D3DCOLOR_COLORVALUE( CLIP_COLOR(r),CLIP_COLOR(g),CLIP_COLOR(b),0.f);
	}

	ZEFFECTBILLBOARDITEM* pBItem =
		m_BillboardLists[4].Add(Target,dir,_Acc, fStartScale, fEndScale, fLife ,tcolor,false);

	if(pBItem) {
		pBItem->bUseSteamSmoke = true;
		pBItem->accel2 = acc2;
	}
}

void ZEffectManager::AddMapSmokeTSEffect(rvector& Target,rvector& dir,rvector& acc,DWORD scolor,DWORD delay,float fLife,float fStartScale,float fEndScale)
{
	if( !RenderCheckEffectLevel() )	return;

	static DWORD _color[] = {
		0x656565,
		0x707070,
		0x757575,
		0x808080,
		0x858585,
		0x909090,
		0x959595,
		0xa5a5a5,
	};
	
	m_BillboardLists[4].SetVanishTime(2.9f);

	rvector _Acc = acc;

	DWORD tcolor = _color[rand()%8];

	if(scolor!=0x01010101) {

		D3DXCOLOR src_color(tcolor);
		D3DXCOLOR dest_color(scolor);

		float r = src_color.r * dest_color.r;
		float g = src_color.g * dest_color.g;
		float b = src_color.b * dest_color.b;

#define CLIP_COLOR(c) min(max(c,0.f),1.f)

		tcolor=D3DCOLOR_COLORVALUE( CLIP_COLOR(r),CLIP_COLOR(g),CLIP_COLOR(b),0.f);
	}

	m_BillboardLists[4].Add(Target,dir,_Acc, fStartScale, fEndScale, fLife ,tcolor,true);
}

void ZEffectManager::AddMapSmokeSSEffect(rvector& Target,rvector& dir,rvector& acc,DWORD scolor,DWORD delay,float fLife,float fStartScale,float fEndScale)
{
	if( !RenderCheckEffectLevel() )	return;

	static DWORD _color[] = {
		0x858585,
		0x909090,
		0x959595,
		0xa0a0a0
	};

	// TODO : �̰� ����Ʈ �������� ������ �ؾ��ҵ�
	m_BillboardLists[4].SetVanishTime(2.9f);

	rvector _Acc = acc;

	DWORD tcolor = _color[rand()%4];

	if(scolor!=0x01010101) {
		
		D3DXCOLOR src_color(tcolor);
		D3DXCOLOR dest_color(scolor);

		float r = src_color.r * dest_color.r;
		float g = src_color.g * dest_color.g;
		float b = src_color.b * dest_color.b;

#define CLIP_COLOR(c) min(max(c,0.f),1.f)

		tcolor=D3DCOLOR_COLORVALUE( CLIP_COLOR(r),CLIP_COLOR(g),CLIP_COLOR(b),0.f);
	}

	m_BillboardLists[4].Add(Target,dir,_Acc, fStartScale, fEndScale, fLife ,tcolor,false);
}

void ZEffectManager::AddRocketSmokeEffect(rvector& Target)
{
	bool bRender = true;

	if(g_nEffectLevel==Z_VIDEO_EFFECT_HIGH) {
		bRender = true;
	}
	else if(g_nEffectLevel==Z_VIDEO_EFFECT_NORMAL) {
		static bool toggle = true;
		if(toggle) {
			bRender = true;
		}
		else {
			bRender = false;
		}
		toggle = !toggle;
	}
	else if(g_nEffectLevel==Z_VIDEO_EFFECT_LOW) {
		bRender = false;
	}

	// TODO : �̰� ����Ʈ �������� ������ �ؾ��ҵ�
	m_BillboardLists[4].SetVanishTime(2.9f);
	if(bRender) {

		rvector add = 50.f*rvector(RANDOMFLOAT-0.5f,RANDOMFLOAT-0.5f,RANDOMFLOAT-0.5f);
		m_BillboardLists[4].Add(Target,add,rvector(0,0,0), 30.f, 120.f, 3.f );

	}
}

void ZEffectManager::AddGrenadeEffect(rvector& Target, rvector& TargetNormal)
{
	ZEffect* pNew = NULL;

	if(g_nEffectLevel != Z_VIDEO_EFFECT_LOW) {//�������� ó������

	// Ÿ�� ���� ����
	rvector up = rvector(0.f,0.f,1.f);

	//������ ��ġ�� ����~

	float distance = 0.f;

	RBSPPICKINFO info;

	if( ZGetGame()->GetWorld()->GetBsp()->Pick( Target, -up, &info ) ) {
		distance = D3DXVec3LengthSq( &( Target - info.PickPos ) );
	}

	if(distance < 150.f) {

		rvector pos = rvector(0.f,0.f,0.f);
		rvector up = rvector(0.3f,0.3f,0.3f);
		rvector scale;

        for(int i=0;i<5;i++) {

			up = rvector(0.3f,0.3f,0.3f);

			float s = 0.5 + (rand()%10)/10.f;

			scale = rvector(s,s,s);

			up.x += (float)(rand()%10)/10.f;
			if((rand()%2)==0) up.x =-up.x; 

			up.y += (float)(rand()%10)/10.f;
			if((rand()%2)==0) up.y =-up.y; 

			up.z += (float)(rand()%10)/10.f;

			float speed = 2.8f + rand()%3;
			Normalize(up);
			
			pos.x = rand()%20;
			pos.y = rand()%20;
			pos.z = rand()%50;

			pNew = new ZEffectSlash( m_pGrenadeExpEffect,Target+pos,TargetNormal );

			((ZEffectSlash*)pNew)->SetUpVector(up);
			((ZEffectSlash*)pNew)->SetScale(scale);
			((ZEffectSlash*)pNew)->GetVMesh()->SetSpeed(speed);

			Add(pNew);
		}

	}

	if(distance < 10.f) {

		pNew = new ZEffectBulletMark( m_pEBSBulletMark[2], Target+up,up );
		((ZEffectBulletMark*)pNew)->m_Scale = rvector(120.f,120.f,120.f);
		Add(pNew);

	}

	rvector _add;
	float _min,_max,_life;

	for(int i=0;i<4;i++) {

		_add.x = rand()%60;
		_add.y = rand()%60;
		_add.z = rand()%60;

		_min = 100 + (rand()%100)/2.f;
		_max = 200 + (rand()%200)/3.f;

		_life = 3000.f + (1000*(rand()%6/3.f));

		AddGrenadeSmokeEffect(Target+_add,_min,_max,_life);

	}

	AddLightFragment(Target,TargetNormal);

	}

	rvector vv = Target;
	vv.z+=50;
	pNew = new ZEffectSlash( m_pGrenadeEffect,vv,TargetNormal );	
	((ZEffectSlash*)pNew)->SetDistOption(29999.f,29999.f,29999.f);//�� �����ش�~
	((ZEffectSlash*)pNew)->SetAlignType(1);
	Add(pNew);

}


void ZEffectManager::AddBloodEffect(rvector& Target, rvector& TargetNormal)
{
	// �� ����..
	/*
	ZEffect* pNew = NULL;

	// �� ����
#define TARGET_BLOOD_MAX_SCALE2		20.0f
#define TARGET_BLOOD_MIN_SCALE2		10.0f
#define TARGET_BLOOD_LIFE_TIME2		200
	//#define TARGET_BLOOD_VELOCITY		4.0f	// meter/sec

	for(int i=0; i<3; i++){
		rvector p = Target+TargetNormal*TARGET_BLOOD_MIN_SCALE2*float(i)*0.5f + rvector(fmod((float)rand(), TARGET_BLOOD_MIN_SCALE2), fmod((float)rand(), TARGET_BLOOD_MIN_SCALE2), fmod((float)rand(), TARGET_BLOOD_MIN_SCALE2));
		float fSize = 1.0f+float(rand()%100)/100.0f;
		AddSmokeEffect(m_pEBSBloods[rand()%BLOOD_COUNT], p, TargetNormal, TARGET_BLOOD_MIN_SCALE2*fSize, TARGET_BLOOD_MAX_SCALE2*fSize, TARGET_BLOOD_LIFE_TIME2);
		Add(pNew);
	}
	*/
}

void ZEffectManager::AddSlashEffect(rvector& Target, rvector& TargetNormal,int nType)
{
	ZEffect* pNew = NULL;

	// Ÿ�� ����Ʈ

	bool _add = false;
	bool _add_uppercut = false;

	float rot_angle = 0.f;

	int mode = rand()%3;

	switch(nType) {

		case SEM_None:			_add = false;													break;	

		case SEM_ManSlash1:		_add = false;													break;
		case SEM_ManSlash2:		_add = true;	rot_angle = 0.f;								break;
		case SEM_ManSlash3:		_add = true;	rot_angle = 3.14f;								break;
		case SEM_ManSlash4:		_add = true;	rot_angle = 3.14f/2.f;							break;
		case SEM_ManSlash5:		_add = true;	rot_angle = -3.14f/2.f;							break;

		case SEM_ManDoubleSlash1:		_add = false;											break;
		case SEM_ManDoubleSlash2:		_add = true;	rot_angle = 0.f;						break;
		case SEM_ManDoubleSlash3:		_add = true;	rot_angle = 3.14f;						break;
		case SEM_ManDoubleSlash4:		_add = true;	rot_angle = 3.14f/2.f;					break;
		case SEM_ManDoubleSlash5:		_add = true;	rot_angle = -3.14f/2.f;					break;

		case SEM_ManGreatSwordSlash1:		_add = false;										break;
		case SEM_ManGreatSwordSlash2:		_add = true;	rot_angle = 0.f;					break;
		case SEM_ManGreatSwordSlash3:		_add = true;	rot_angle = 3.14f;					break;
		case SEM_ManGreatSwordSlash4:		_add = true;	rot_angle = 3.14f/2.f;				break;
		case SEM_ManGreatSwordSlash5:		_add = true;	rot_angle = -3.14f/2.f;				break;

		case SEM_ManUppercut:	_add_uppercut = true;	rot_angle = -3.14f/2.f;					break;

		case SEM_WomanSlash1:	_add = true;	rot_angle = 3.14f + 3.14f/2.f;		mode = 0;	break;
		case SEM_WomanSlash2:	_add = true;	rot_angle = 3.14f;								break;
		case SEM_WomanSlash3:	_add = false;													break;
		case SEM_WomanSlash4:	_add = false;													break;
		case SEM_WomanSlash5:	_add = false;													break;

		case SEM_WomanDoubleSlash1:	_add = true;	rot_angle = 3.14f + 3.14f/2.f;	mode = 0;	break;
		case SEM_WomanDoubleSlash2:	_add = true;	rot_angle = 3.14f;							break;
		case SEM_WomanDoubleSlash3:	_add = false;												break;
		case SEM_WomanDoubleSlash4:	_add = false;												break;
		case SEM_WomanDoubleSlash5:	_add = false;												break;

		case SEM_WomanGreatSwordSlash1:	_add = true;	rot_angle = 3.14f + 3.14f/2.f;mode = 0;	break;
		case SEM_WomanGreatSwordSlash2:	_add = true;	rot_angle = 3.14f;						break;
		case SEM_WomanGreatSwordSlash3:	_add = false;											break;
		case SEM_WomanGreatSwordSlash4:	_add = false;											break;
		case SEM_WomanGreatSwordSlash5:	_add = false;											break;

		case SEM_WomanUppercut:	_add_uppercut = true;	rot_angle = -3.14f/2.f;					break;
	}

	if(!_add_uppercut) {

		pNew = new ZEffectSlash(m_pSworddam[mode],Target,TargetNormal);	
		((ZEffectSlash*)pNew)->SetScale(rvector(1.0f,1.0f,1.0f));
		((ZEffectSlash*)pNew)->SetAlignType(1);
		Add(pNew);

	}

	// ���� Į �ֵθ��� ���⿡ ���� ����Ʈ ȸ���� ����..

	if(_add) {
		pNew = new ZEffectSlash(m_pSwordglaze,Target,TargetNormal);	
		((ZEffectSlash*)pNew)->SetRotationAngleZ(rot_angle);
		Add(pNew);
	}

	if(_add_uppercut) {//����
		pNew = new ZEffectSlash(m_pSwordUppercutEffect,Target,TargetNormal);
		((ZEffectSlash*)pNew)->SetRotationAngleZ(rot_angle);
		((ZEffectSlash*)pNew)->SetScale(rvector(1.5f,1.5f,1.5f));
		((ZEffectSlash*)pNew)->SetAlignType(1);
		Add(pNew);
	}
}

void ZEffectManager::AddSwordUppercutDamageEffect(rvector& Target,MUID uidTarget,DWORD time)
{
	ZEffect* pNew = NULL;

	rvector dir = -RealSpace2::RCameraDirection;// rvector(0.f,0.f,1.f);
	dir.z = 0.f;
	pNew = new ZEffectDash(m_pSwordUppercutDamageEffect,Target,dir,uidTarget);
	((ZEffectDash*)pNew)->SetAlignType(1);

	if(time)
		((ZEffectDash*)pNew)->SetStartTime(time);

	Add(pNew);
}

void ZEffectManager::AddEatBoxEffect(const rvector& Target,ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector pos = Target;

	rvector dir = -RealSpace2::RCameraDirection;// rvector(0.f,0.f,1.f);
	dir.z = 0.f;
	pos.z -= 120.f;//��~
	pNew = new ZEffectDash(m_pEatBoxEffect,pos,dir,pObj->GetUID());
	pNew->SetEffectType(ZET_HEAL);
	((ZEffectDash*)pNew)->SetAlignType(1);
	// �������� ������ �������� ����..
	DeleteSameType((ZEffectAniMesh*)pNew);
	Add(pNew);
}

void ZEffectManager::AddHealEffect(const rvector& Target,ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector pos = Target;

	rvector dir = -RealSpace2::RCameraDirection;// rvector(0.f,0.f,1.f);
	dir.z = 0.f;
	pos.z -= 120.f;//��~
	pNew = new ZEffectDash(m_pHealEffect,pos,dir,pObj->GetUID());
	pNew->SetEffectType(ZET_HEAL);
	((ZEffectDash*)pNew)->SetAlignType(1);
	// �������� ������ �������� ����..
	DeleteSameType((ZEffectAniMesh*)pNew);
	Add(pNew);
}

void ZEffectManager::AddRepairEffect(const rvector& Target,ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector pos = Target;

	rvector dir = -RealSpace2::RCameraDirection;// rvector(0.f,0.f,1.f);
	dir.z = 0.f;
	pos.z -= 120.f;//��~
	pNew = new ZEffectDash(m_pRepairEffect,pos,dir,pObj->GetUID());
//	pNew->SetEffectType(ZET_REPARE);
	pNew->SetEffectType(ZET_HEAL);//������ �ʿ��������~
	((ZEffectSlash*)pNew)->SetAlignType(1);
	// �������� ������ �������� ����..
	DeleteSameType((ZEffectAniMesh*)pNew);
	Add(pNew);
}

void ZEffectManager::AddPotionEffect(const rvector& Target, ZObject* pObj, MMatchItemEffectId effectId)
{
	ZEffect* pNew = NULL;

	RMesh* pPotionEffect = NULL;
	switch(effectId)
	{
	case MMIEI_POTION_HEAL_INSTANT:		pPotionEffect = m_pHealInstantEffect;	break;
	case MMIEI_POTION_HEAL_OVERTIME:	pPotionEffect = m_pHealOverTimeEffect;	break;
	case MMIEI_POTION_REPAIR_INSTANT:	pPotionEffect = m_pRepairInstantEffect;	break;
	case MMIEI_POTION_REPAIR_OVERTIME:	pPotionEffect = m_pRepairOverTimeEffect; break;
	default: 
		_ASSERT(0);
		break;
	}

	if (!pPotionEffect) { _ASSERT(0); return; }

	rvector pos = Target;
	rvector dir = -RealSpace2::RCameraDirection;
	dir.z = 0.f;

	pNew = new ZEffectDash(pPotionEffect,pos,dir,pObj->GetUID());
	((ZEffectDash*)pNew)->SetScale(rvector(15,15,15));
	((ZEffectDash*)pNew)->SetAlignType(1);

	if (pObj->GetUID() == ZGetMyUID())
		((ZEffectDash*)pNew)->GetVMesh()->m_bCheckViewFrustum = false;

	Add(pNew);
}

void ZEffectManager::AddHasteEffect(const rvector& Target, ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector pos = Target;
	rvector dir = -RealSpace2::RCameraDirection;
	dir.z = 0.f;

	pNew = new ZEffectDash(m_pHasteEffect,pos,dir,pObj->GetUID());
	((ZEffectDash*)pNew)->SetScale(rvector(15,15,15));
	((ZEffectDash*)pNew)->SetAlignType(1);

	if (pObj->GetUID() == ZGetMyUID())
		((ZEffectDash*)pNew)->GetVMesh()->m_bCheckViewFrustum = false;

	Add(pNew);
}

void ZEffectManager::AddHealOverTimeBeginEffect(const rvector& Target, ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector pos = Target;
	rvector dir = -RealSpace2::RCameraDirection;
	dir.z = 0.f;

	pNew = new ZEffectDash(m_pHealOverTimeBeginEffect,pos,dir,pObj->GetUID());
	((ZEffectDash*)pNew)->SetScale(rvector(15,15,15));
	((ZEffectDash*)pNew)->SetAlignType(1);

	if (pObj->GetUID() == ZGetMyUID())
		((ZEffectDash*)pNew)->GetVMesh()->m_bCheckViewFrustum = false;

	Add(pNew);
}

void ZEffectManager::AddRepairOverTimeBeginEffect(const rvector& Target, ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector pos = Target;
	rvector dir = -RealSpace2::RCameraDirection;
	dir.z = 0.f;

	pNew = new ZEffectDash(m_pRepairOverTimeBeginEffect,pos,dir,pObj->GetUID());
	((ZEffectDash*)pNew)->SetScale(rvector(15,15,15));
	((ZEffectDash*)pNew)->SetAlignType(1);

	if (pObj->GetUID() == ZGetMyUID())
		((ZEffectDash*)pNew)->GetVMesh()->m_bCheckViewFrustum = false;

	Add(pNew);
}

void ZEffectManager::AddHasteBeginEffect(const rvector& Target, ZObject* pObj)
{
	ZEffect* pNew = NULL;

	rvector pos = Target;
	rvector dir = -RealSpace2::RCameraDirection;
	dir.z = 0.f;

	pNew = new ZEffectDash(m_pHasteBeginEffect,pos,dir,pObj->GetUID());
	((ZEffectDash*)pNew)->SetScale(rvector(15,15,15));
	((ZEffectDash*)pNew)->SetAlignType(1);

	if (pObj->GetUID() == ZGetMyUID())
		((ZEffectDash*)pNew)->GetVMesh()->m_bCheckViewFrustum = false;

	Add(pNew);
}

void ZEffectManager::AddExpanseAmmoEffect(const rvector& Target,ZObject* pObj )
{
	ZEffect* pNew = NULL;

	rvector pos = Target;

	rvector dir = -RealSpace2::RCameraDirection;
	dir.z = 0.f;
	pos.z -= 120.f;
	pNew = new ZEffectDash( m_pExpanseAmmoEffect, pos, dir, pObj->GetUID() );
	((ZEffectSlash*)pNew)->SetAlignType(2);
	Add(pNew);
}

void ZEffectManager::AddSlashEffectWall(rvector& Target, rvector& TargetNormal,int nType)
{
	ZEffect* pNew = NULL;

	bool _add = false;
	bool _add_left = false;

	float rot_angle = 0.f;
	float rot_angle_left = 0.f;
	
	DWORD startTime = 250;

	int mode = 3;

#define _CASE_DEF(_c ,_rot,_rot_left,tadd,taddleft ,_stime ) case _c: rot_angle = _rot;rot_angle_left = _rot_left; _add = tadd;_add_left = taddleft; startTime = _stime; break;

	switch(nType) {

		_CASE_DEF(SEM_None     ,0.f,0.f,false,false,250);

		_CASE_DEF(SEM_ManSlash1,-2.8f,0.f,true,false,250);
		_CASE_DEF(SEM_ManSlash2,-1.3f,0.f,true,false,250);
		_CASE_DEF(SEM_ManSlash3, 1.3f,0.f,true,false,250);
		_CASE_DEF(SEM_ManSlash4, 0.8f,0.f,true,false,250);
		_CASE_DEF(SEM_ManSlash5,-1.5f,0.f,true,false,250);

		_CASE_DEF(SEM_ManDoubleSlash1,-0.1f, 0.0f,true,false , 10);
		_CASE_DEF(SEM_ManDoubleSlash2, 2.5f, 0.0f,true,false ,250);
		_CASE_DEF(SEM_ManDoubleSlash3, 0.0f, 0.0f,false,false,250);
		_CASE_DEF(SEM_ManDoubleSlash4, 3.2f,-0.1f,true,true  ,250);
		_CASE_DEF(SEM_ManDoubleSlash5,-1.5f, 0.0f,true,false ,250);

		_CASE_DEF(SEM_ManGreatSwordSlash1, 0.7f,0.f,true,false,250);
		_CASE_DEF(SEM_ManGreatSwordSlash2, 3.5f,0.f,true,false,250);
		_CASE_DEF(SEM_ManGreatSwordSlash3,-0.3f,0.f,true,false,250);
		_CASE_DEF(SEM_ManGreatSwordSlash4, 3.3f,0.f,true,false,250);
		_CASE_DEF(SEM_ManGreatSwordSlash5,-1.5f,0.f,true,false,250);

		_CASE_DEF(SEM_ManUppercut,-1.5f,0.f,true,false,250);

		_CASE_DEF(SEM_WomanSlash1,-1.0f,0.f,true,false,250);
		_CASE_DEF(SEM_WomanSlash2, 2.3f,0.f,true,false,250);
		_CASE_DEF(SEM_WomanSlash3,-0.5f,0.f,true,false,250);
		_CASE_DEF(SEM_WomanSlash4, 2.5f,0.f,true,false,250);
		_CASE_DEF(SEM_WomanSlash5,-1.5f,0.f,true,false,250);

		_CASE_DEF(SEM_WomanDoubleSlash1,-0.1f, 0.0f,true,false , 10);
		_CASE_DEF(SEM_WomanDoubleSlash2, 2.5f, 0.0f,true,false ,250);
		_CASE_DEF(SEM_WomanDoubleSlash3, 0.0f, 0.0f,false,false,250);
		_CASE_DEF(SEM_WomanDoubleSlash4, 3.2f,-0.1f,true,true  ,250);
		_CASE_DEF(SEM_WomanDoubleSlash5,-1.5f, 0.0f,true,false ,250);

		_CASE_DEF(SEM_WomanGreatSwordSlash1, 0.7f,0.f,true,false,250);
		_CASE_DEF(SEM_WomanGreatSwordSlash2, 4.0f,0.f,true,false,250);
		_CASE_DEF(SEM_WomanGreatSwordSlash3,-0.3f,0.f,true,false,250);
		_CASE_DEF(SEM_WomanGreatSwordSlash4, 1.2f,0.f,true,false,250);
		_CASE_DEF(SEM_WomanGreatSwordSlash5,-1.5f,0.f,true,false,250);

		_CASE_DEF(SEM_WomanUppercut,-1.5f,0.f,true,false,250);
	}

	if(_add) {
	
		pNew = new ZEffectSlash(m_pSworddam[mode],Target,TargetNormal);

		((ZEffectSlash*)pNew)->SetRotationAngleZ(rot_angle);
		((ZEffectSlash*)pNew)->SetStartTime(startTime);
		((ZEffectSlash*)pNew)->SetAlignType(1);

		Add(pNew);
	}

	if(_add_left) {

		pNew = new ZEffectSlash(m_pSworddam[mode],Target,TargetNormal);

		((ZEffectSlash*)pNew)->SetRotationAngleZ(rot_angle_left);
		((ZEffectSlash*)pNew)->SetStartTime(startTime);
		((ZEffectSlash*)pNew)->SetAlignType(1);

		Add(pNew);
	}

}

void ZEffectManager::OnInvalidate()
{
	ZEffectBase::OnInvalidate();
}

void ZEffectManager::OnRestore()
{
	ZEffectBase::OnRestore();
}

void ZEffectManager::AddShadowEffect(rmatrix& m,DWORD _color)
{
	m_ShadowList.Add(m,_color);
}

void ZEffectManager::AddSmokeEffect(rvector& Target )
{
	rvector v = rvector( 0,0,0 );
	ZEffect* pNew	= new ZEffectSmoke( m_pEBSSmokes[0], Target, v, 10, 3000, 50000 );
	Add( pNew );
}

void ZEffectManager::AddSmokeEffect( ZEffectBillboardSource* pEffectBillboardSource,rvector& Pos, rvector& Velocity, rvector &Accel, float fMinScale, float fMaxScale, float fLifeTime )
{
	m_BillboardLists[rand()%SMOKE_COUNT].Add(Pos,Velocity,Accel,fMinScale,fMaxScale,fLifeTime);
}

#define MAX_SG_VELOCITY	10
void ZEffectManager::AddSmokeGrenadeEffect( rvector& Target  )
{
	rvector v;
	
	srand( timeGetTime() );
	v.x	= (rand() % MAX_SG_VELOCITY) * 2 - MAX_SG_VELOCITY;
	v.y	= (rand() % MAX_SG_VELOCITY) * 2 - MAX_SG_VELOCITY;
	v.z	= 0.f;
	D3DXVec3Normalize( &v, &v );
	ZEffect* pNew	= new ZEffectSmokeGrenade( m_pEBSSmokes[0], Target, v, 10, 1000, 20000 );
	((ZEffectSmokeGrenade*)pNew)->SetDistOption(29999.f,29999.f,29999.f);//�� �����ش�~
	Add( pNew );
}

void ZEffectManager::AddGrenadeSmokeEffect( rvector& Target ,float min,float max,float time)
{
	rvector v;
	
	srand( timeGetTime() );
	v.x	= (rand() % MAX_SG_VELOCITY) * 2 - MAX_SG_VELOCITY;
	v.y	= (rand() % MAX_SG_VELOCITY) * 2 - MAX_SG_VELOCITY;
	v.z	= 0.f;

	D3DXVec3Normalize( &v, &v );
	ZEffect* pNew	= new ZEffectSmokeGrenade( m_pEBSSmokes[0], Target, v, min, max, time );
	Add( pNew );
}

void ZEffectManager::AddWorldItemEatenEffect( rvector& pos )
{
	ZEffect* pNew = new ZEffectSlash(m_pWorldItemEaten, pos, RCameraDirection );
	((ZEffectSlash*)pNew)->SetAlignType(2);
	Add(pNew);
}

void ZEffectManager::AddCharacterIcon(ZObject* pObj,int nIcon)
{
	if(!pObj->GetInitialized()) return;
	if(!pObj->GetVisualMesh()->m_bIsRender) return;

	ZEffect* pNew = NULL;
	pNew = new ZEffectIcon(m_pCharacterIcons[nIcon],pObj);
	((ZEffectIcon*)pNew)->SetAlignType(2);
	Add(pNew);
}

class ZEffectIconLoop : public ZEffectIcon { // ����� �߰��ϸ� ����
private:
public:
	ZEffectIconLoop(RMesh* pMesh, ZObject* pObj) 
		: ZEffectIcon(pMesh,pObj)
	{}

		virtual bool Draw(unsigned long int nTime)
		{
			ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

			if( pObj ) {
				if(!pObj->IsRendered())
					return true;
				if( pObj->m_pVMesh ) {
					m_Pos = pObj->m_pVMesh->GetBipTypePosition(m_type);
					m_DirOrg = -pObj->m_Direction;
					ZEffectAniMesh::Draw(nTime);
					return true;
				}
			}
			return false;
		}
};

#include "ZGameClient.h"
class ZEffectIconLoopStar : public ZEffectIconLoop {
	// ����� �߰��ϸ� ����
private:
public:
	ZEffectIconLoopStar(RMesh* pMesh, ZObject* pObj)
		: ZEffectIconLoop(pMesh,pObj)
	{}

		virtual bool Draw(unsigned long int nTime)
		{
			MMatchObjCache* pCache = ZGetGameClient()->FindObjCache(m_uid);
			if (pCache && pCache->GetUGrade() != MMUG_STAR)
				return false;

			return ZEffectIconLoop::Draw(nTime);
		}
};

void ZEffectManager::AddBerserkerIcon(ZObject* pObj)
{
	ZEffect* pNew;

	for (int i = 0; i < eq_parts_pos_info_end; i++)
	{
		if (
			(i == eq_parts_pos_info_RFoot) || 
			(i == eq_parts_pos_info_LFoot) ||
			(i == eq_parts_pos_info_RToe0) || 
			(i == eq_parts_pos_info_LToe0) ||
			(i == eq_parts_pos_info_RToe0Nub) || 
			(i == eq_parts_pos_info_LToe0Nub) ||
			(i == eq_parts_pos_info_LFingerNub) ||
			(i == eq_parts_pos_info_RFingerNub) ||
			(i == eq_parts_pos_info_etc) ||
			(i == eq_parts_pos_info_LClavicle) ||
			(i == eq_parts_pos_info_RClavicle) ||
			(i == eq_parts_pos_info_Effect)) continue;


		pNew = new ZEffectBerserkerIconLoop(m_pBerserkerEffect,pObj);
		((ZEffectBerserkerIconLoop*)pNew)->SetAlignType(2);
		((ZEffectBerserkerIconLoop*)pNew)->m_type = _RMeshPartsPosInfoType(i);
		Add(pNew);
	}
}

void ZEffectManager::AddCommanderIcon(ZObject* pObj,int nTeam)
{
	if(nTeam<0 || nTeam>=2) return;

	ZEffect* pNew = NULL;
	pNew = new ZEffectIconLoop(m_pCommandIcons[nTeam],pObj);
	((ZEffectIconLoop*)pNew)->SetAlignType(2);
	((ZEffectIconLoop*)pNew)->m_type = eq_parts_pos_info_Spine1;
	Add(pNew);

	pNew = new ZEffectIconLoop(m_pCommandIcons[nTeam],pObj);
	((ZEffectIconLoop*)pNew)->SetAlignType(2);
	((ZEffectIconLoop*)pNew)->m_type = eq_parts_pos_info_Root;
	Add(pNew);
}

void ZEffectManager::AddChatIcon(ZObject* pObj)
{
	class ZEffectChatIconLoop : public ZEffectIcon {
	public:
		ZEffectChatIconLoop(RMesh* pMesh, ZObject* pObj) 
			: ZEffectIcon(pMesh,pObj) {
			}

			virtual bool Draw(unsigned long int nTime)
			{
				ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

				ZCharacter* pChar = MDynamicCast(ZCharacter, pObj);

				if( pChar ) {
					if(!pChar->m_dwStatusBitPackingValue.Ref().m_bChatEffect) return false;
					if(!pChar->m_bRendered)
						return true;
					if(!pChar->IsVisible())
						return true;
					if( pObj->m_pVMesh ) {
						m_Pos = pObj->GetVisualMesh()->GetHeadPosition()+rvector(0,0,60);

						RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);//��������۰��� ��Ĩ����߻�..

						ZEffectAniMesh::Draw(nTime);

						RGetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

						return true;
					}
				}
				return false;
			}
	};

	ZEffect* pNew = NULL;
	pNew = new ZEffectChatIconLoop(m_pChatIcon,pObj);
	((ZEffectChatIconLoop*)pNew)->SetAlignType(2);
	Add(pNew);
}

void ZEffectManager::AddLostConIcon(ZObject* pObj)
{
	class ZEffectLostConIconLoop : public ZEffectIcon 
	{
	public:
		ZEffectLostConIconLoop(RMesh* pMesh, ZObject* pObj) 
			: ZEffectIcon(pMesh,pObj) {
			}

			virtual bool Draw(unsigned long int nTime)
			{
				ZObject* pObj = ZGetObjectManager()->GetObject(m_uid);

				ZCharacter* pChar = MDynamicCast(ZCharacter, pObj);

				if( pChar ) {
					if(pChar->IsDie()) return false;
					if(!pChar->m_dwStatusBitPackingValue.Ref().m_bLostConEffect) return false;
					if(!pChar->m_bRendered)
						return true;
					if( pObj->m_pVMesh ) {
						m_Pos = pObj->GetVisualMesh()->GetHeadPosition()+rvector(0,0,60);
						ZEffectAniMesh::Draw(nTime);
						return true;
					}
				}
				return false;
			}
	};

	ZEffect* pNew = NULL;
	pNew = new ZEffectLostConIconLoop(m_pLostConIcon,pObj);
	((ZEffectLostConIconLoop*)pNew)->SetAlignType(2);
	Add(pNew);
}

void ZEffectManager::AddChargingEffect( ZObject *pObj )
{
	rvector TargetNormal = rvector(1,0,0);
	ZEffectCharging *pNew = new ZEffectCharging(m_pEffectMeshMgr->Get("ef_spirits.elu"),pObj->GetPosition(),TargetNormal,pObj);	
	((ZEffectCharging*)pNew)->SetAlignType(1);
	Add(pNew);
}

void ZEffectManager::AddChargedEffect( ZObject *pObj )
{
	rvector TargetNormal = rvector(1,0,0);
	ZEffectCharged *pNew = new ZEffectCharged(m_pEffectMeshMgr->Get("ef_spirits.elu_1.elu"),pObj->GetPosition(),TargetNormal,pObj);
	((ZEffectCharged*)pNew)->SetAlignType(1);
	Add(pNew);
}

void ZEffectManager::AddStarEffect( ZObject *pObj )
{
	ZEffect* pNew = NULL;
	pNew = new ZEffectIconLoopStar(m_pEffectMeshMgr->Get("event_ongame_jjang"),pObj);
	((ZEffectIconLoop*)pNew)->SetAlignType(1);
	((ZEffectIconLoop*)pNew)->m_type = eq_parts_pos_info_Spine2;
	Add(pNew);
/*
	ZEffect* pNew = NULL;
	pNew = new ZEffectIconLoop(m_pEffectMeshMgr->Get("event_ongame_jjang"),pObj, rvector(0,0,90));
	((ZEffectIconLoop*)pNew)->SetAlignType(2);
	((ZEffectIconLoop*)pNew)->m_type = eq_parts_pos_info_Spine1;
	Add(pNew);
*/
}

void ZEffectManager::Add(const char* szName,const rvector& pos, const rvector& dir,const MUID& uidOwner,int nLifeTime)
{
	ZEffect* pNew = NULL;
	RMesh *pMesh = m_pEffectMeshMgr->Get((char*)szName);
	if(!pMesh) return;

	pNew = new ZEffectSlash(pMesh,(rvector&)pos,(rvector&)dir);
	((ZEffectSlash*)pNew)->SetAlignType(1);
	((ZEffectSlash*)pNew)->SetLifeTime(nLifeTime);
	
	Add(pNew);
}
// �̸����� Ư���� �Լ��� �����ϰ� �������...
void ZEffectManager::AddSp(const char* szName,int nCnt,const rvector& pos, const rvector& dir,const MUID& uidOwner)
{
	if(stricmp(szName,"BlizzardEffect")==0) {
		AddBlizzardEffect((rvector&)pos , nCnt );
	}
	else if(stricmp(szName,"MethorEffect")==0) {
		AddMethorEffect((rvector&)pos , nCnt );
	}
	else {

	}
}

void ZEffectManager::AddPartsPosType(const char* szName,const MUID& uidOwner,RMeshPartsPosInfoType type,int nLifeTime)
{
	ZEffect* pNew = NULL;
	RMesh *pMesh = m_pEffectMeshMgr->Get((char*)szName);
	if(!pMesh) return;

	ZObject* pObj = ZGetObjectManager()->GetObject(uidOwner);

	ZCharacterObject* pCObj = MDynamicCast(ZCharacterObject, pObj);

	rvector pos = pCObj->GetPosition();// rvector(0,0,0);
	rvector dir = pCObj->GetDirection();// rvector(0,0,1);

	pNew = new ZEffectPartsTypePos(pMesh,(rvector&)pos,(rvector&)dir,rvector(0.f,0.f,0.f),pObj);
	((ZEffectPartsTypePos*)pNew)->SetAlignType(1);
	((ZEffectPartsTypePos*)pNew)->m_type = type;
	((ZEffectPartsTypePos*)pNew)->SetLifeTime(nLifeTime);
	Add(pNew);
}

void ZEffectManager::AddItemTrapEffect(rvector& Target, rvector& TargetNormal)
{
	ZEffect* pNew = NULL;

	if(g_nEffectLevel != Z_VIDEO_EFFECT_LOW) 
	{		
		rvector up = rvector(0.f,0.f,1.f);	// Ÿ�� ���� ����

		float distance = 0.f;
		RBSPPICKINFO info;

		if( ZGetGame()->GetWorld()->GetBsp()->Pick( Target, -up, &info ) ) {
			distance = D3DXVec3LengthSq( &( Target - info.PickPos ) );
		}

		if(distance < 150.f) 
		{
			rvector pos = rvector(0.f,0.f,0.f);
			rvector up = rvector(0.3f,0.3f,0.3f);
			rvector scale;

			for(int i = 0; i < 5; i++) 
			{
				up = rvector(0.3f,0.3f,0.3f);

				float s = 0.5 + (rand()%10)/10.f;

				scale = rvector(s,s,s);

				up.x += (float)(rand()%10)/10.f;
				if((rand()%2)==0) up.x =-up.x; 

				up.y += (float)(rand()%10)/10.f;
				if((rand()%2)==0) up.y =-up.y; 

				up.z += (float)(rand()%10)/10.f;

				float speed = 2.8f + rand()%3;
				Normalize(up);

				pos.x = rand()%20;
				pos.y = rand()%20;
				pos.z = rand()%50;

				pNew = new ZEffectSlash( m_pGrenadeExpEffect,Target+pos,TargetNormal );

				((ZEffectSlash*)pNew)->SetUpVector(up);
				((ZEffectSlash*)pNew)->SetScale(scale);
				((ZEffectSlash*)pNew)->GetVMesh()->SetSpeed(speed);

				Add(pNew);
			}

		}

		if(distance < 10.f) {

			pNew = new ZEffectBulletMark( m_pEBSBulletMark[2], Target+up,up );
			((ZEffectBulletMark*)pNew)->m_Scale = rvector(120.f,120.f,120.f);
			Add(pNew);

		}

		rvector _add;
		float _min,_max,_life;

		for(int i=0;i<4;i++) {

			_add.x = rand()%60;
			_add.y = rand()%60;
			_add.z = rand()%60;

			_min = 100 + (rand()%100)/2.f;
			_max = 200 + (rand()%200)/3.f;

			_life = 3000.f + (1000*(rand()%6/3.f));

			AddGrenadeSmokeEffect(Target+_add,_min,_max,_life);

		}

		AddLightFragment(Target,TargetNormal);
	}

	rvector vv = Target;
	vv.z+=50;
	pNew = new ZEffectSlash( m_pGrenadeEffect,vv,TargetNormal );	
	((ZEffectSlash*)pNew)->SetDistOption(29999.f,29999.f,29999.f);//�� �����ش�~
	((ZEffectSlash*)pNew)->SetAlignType(1);

	Add(pNew);
}

void ZEffectManager::AddDynamiteEffect(rvector& Target, rvector& TargetNormal)
{
	ZEffect* pNew = NULL;

	if(g_nEffectLevel != Z_VIDEO_EFFECT_LOW) 
	{
		// Ÿ�� ���� ����
		rvector up = rvector(0.f,0.f,1.f);

		//������ ��ġ�� ����~
		float distance = 0.f;

		RBSPPICKINFO info;

		if( ZGetGame()->GetWorld()->GetBsp()->Pick( Target, -up, &info ) ) {
			distance = D3DXVec3LengthSq( &( Target - info.PickPos ) );
		}

		if(distance < 150.f) {

			rvector pos = rvector(0.f,0.f,0.f);
			rvector up = rvector(0.3f,0.3f,0.3f);
			rvector scale;

			for(int i = 0; i < 5; i++) {

				up = rvector(0.3f,0.3f,0.3f);

				float s = 0.5 + (rand()%10)/10.f;

				scale = rvector(s,s,s);

				up.x += (float)(rand()%10)/10.f;
				if((rand()%2)==0) up.x =-up.x; 

				up.y += (float)(rand()%10)/10.f;
				if((rand()%2)==0) up.y =-up.y; 

				up.z += (float)(rand()%10)/10.f;

				float speed = 2.8f + rand()%3;
				Normalize(up);

				pos.x = rand()%20;
				pos.y = rand()%20;
				pos.z = rand()%50;

				pNew = new ZEffectSlash( m_pGrenadeExpEffect,Target+pos,TargetNormal );

				((ZEffectSlash*)pNew)->SetUpVector(up);
				((ZEffectSlash*)pNew)->SetScale(scale);
				((ZEffectSlash*)pNew)->GetVMesh()->SetSpeed(speed);

				Add(pNew);
			}
		}

		if(distance < 10.f) {
			pNew = new ZEffectBulletMark( m_pEBSBulletMark[2], Target+up,up );
			((ZEffectBulletMark*)pNew)->m_Scale = rvector(120.f,120.f,120.f);
			Add(pNew);
		}

		rvector _add;
		float _min,_max,_life;

		for(int i = 0; i < 4; i++) 
		{
			_add.x = rand() % 60;
			_add.y = rand() % 60;
			_add.z = rand() % 60;

			_min = 100 + (rand()%100)/2.f;
			_max = 200 + (rand()%200)/3.f;

			_life = 3000.f + (1000*(rand()%6/3.f));

			AddGrenadeSmokeEffect(Target+_add,_min,_max,_life);
		}

		AddLightFragment(Target,TargetNormal);
	}

	rvector dir = -RealSpace2::RCameraDirection;
	pNew = new ZEffectSlash( m_pDynamite, Target, dir);
	((ZEffectSlash*)pNew)->SetDistOption(29999.f,29999.f,29999.f);//�� �����ش�~
	((ZEffectSlash*)pNew)->SetScale(rvector(30,30,30));
	Add(pNew);
}

void ZEffectManager::AddTrapFireEffect(rvector& Target, rvector& TargetNormal)
{
	ZEffect* pNew = NULL;

	pNew = new ZEffectSlash(m_pTrapFire,Target,rvector(0,1,0));
	((ZEffectSlash*)pNew)->SetScale(rvector(25.5f,25.5f,25.5f));
	((ZEffectSlash*)pNew)->SetRotationAngleY(rand()%(314*2)*0.01f);	// 0~2pi
	((ZEffectSlash*)pNew)->SetDistOption(5000.f, 3000.f, 1500.f);
	((ZEffectSlash*)pNew)->SetAlignType(1);

	Add(pNew);

	if(g_nEffectLevel != Z_VIDEO_EFFECT_LOW) 
	{
		// ���� ����Ʈ�� �����ش�
		rvector _add;
		float _min,_max,_life;

		for(int i=0;i<1;i++) {

			_add.x = rand()%60;
			_add.y = rand()%60;
			_add.z = rand()%60;

			_min = 200 + (rand()%100)/2.f;
			_max = 400 + (rand()%200)/3.f;

			_life = 1000.f + (1000*(rand()%6/3.f));

			AddGrenadeSmokeEffect(Target+_add,_min,_max,_life);

		}
	}
}

void ZEffectManager::AddTrapColdEffect(rvector& Target, rvector& TargetNormal)
{
	ZEffect* pNew = NULL;

	pNew = new ZEffectSlash(m_pTrapCold,Target,TargetNormal);
	((ZEffectSlash*)pNew)->SetScale(rvector(51,51,51));
	((ZEffectSlash*)pNew)->SetRotationAngleY(rand()%(314*2)*0.01f);	// 0~2pi
	((ZEffectSlash*)pNew)->SetDistOption(5000.f, 3000.f, 1500.f);
	((ZEffectSlash*)pNew)->SetAlignType(1);

	Add(pNew);
}

void ZEffectManager::AddTrapGuideEffect(rvector& Target, rvector& TargetNormal, int nLifeTime, bool bFriendly, float fScaleRatio, float fSpeed)
{
	ZEffect* pNew = NULL;

	RMesh* pMesh = bFriendly ? m_pTrapGuideFriendly : m_pTrapGuideHostile;

	pNew = new ZEffectSlash(pMesh,Target,TargetNormal);
	float fScale = 32.f * fScaleRatio;
	((ZEffectSlash*)pNew)->SetScale(rvector(fScale,fScale,fScale));
	((ZEffectSlash*)pNew)->SetDistOption(29999.f,29999.f,29999.f);//�� �����ش�~
	((ZEffectSlash*)pNew)->SetAlignType(1);
	((ZEffectSlash*)pNew)->SetLifeTime(nLifeTime);
	((ZEffectSlash*)pNew)->GetVMesh()->SetSpeed(fSpeed);

	Add(pNew);
}

