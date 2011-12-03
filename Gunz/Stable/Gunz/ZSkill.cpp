#include "StdAfx.h"
#include "ZSKill.h"
#include "MXml.h"
#include "MZFileSystem.h"
#include "ZGame.h"
#include "ZModule_Resistance.h"
#include "ZChat.h"

ZSkillDesc::ZSkillDesc()
{
	szName[0]=0;
	szMessage[0]=0;
	szEffectSound[0]=0;
	szExplosionSound[0] = 0;

	szCastingEffect[0]=0;
	szCastingEffectSp[0]=0;
	szCastingPreEffect[0]=0;

	vCastingEffectAddPos = rvector(0,0,0);
	nCastingEffectSpCount = 0;

	szTrailEffect[0] = 0;
	szTargetEffect[0] = 0;

	fTrailEffectScale = 1.f;
	bDrawTrack = true;
	fColRadius = 0.0f;

	nEffectStartPosType = ZSTP_NONE;
	fEffectAreaMin = 0.0f;
	fEffectAngle = pi / 2.0f;		// 90��

	bCameraShock = true;
	fCameraPower = 1.0f;
	fCameraDuration = 0.5f;
	fCameraRange = 1500.0f;

	pRepeatList = new vector<ZSkillRepeat>;
}

ZSkillDesc::~ZSkillDesc()
{
	delete pRepeatList;
}


bool ZSkillDesc::IsEffectiveTo(ZObject *pTarget)	// �� ��ų�� ��󿡰� ȿ���� �ִ���..
{
	if(pTarget->IsDie()) return false;	// �׾��ִ� Ÿ��

	ZModule_HPAP *pModule = (ZModule_HPAP*)pTarget->GetModule(ZMID_HPAP);

	if(pModule) {
		if(nModHeal>0 &&  !pModule->IsFullHP()) return true;
		if(nModRepair>0 && !pModule->IsFullAP()) return true;
	
		if(nModDamage>0 || nModLastDamage>0 || nModDoT>0) return true;
		if(bModRoot) return true;
	}

	ZModule_Movable *pModule_Movable = (ZModule_Movable*)pTarget->GetModule(ZMID_MOVABLE);
	if(pModule_Movable) {
		if(nModSpeed<100 && nModSpeed*.01f<pModule_Movable->GetMoveSpeedRatio()) return true;
	}

	return false;
}

// ���׿� ���������� false�� ����
bool ZSkillDesc::CheckResist(ZObject *pCurrent,float *pfDamage)
{
	// ���������� �Ѵ�. ��ȹ���� step 4���� �°����� �����Ѵ�

	ZModule_Resistance *pModule = (ZModule_Resistance *)pCurrent->GetModule(ZMID_RESISTANCE);
	if(!pModule) return true;

	int nResist = 0;
	switch(ResistType)
	{
		case ZSR_FIRE		: nResist = pModule->GetFR();break;
		case ZSR_POISON		: nResist = pModule->GetPR();break;
		case ZSR_COLD		: nResist = pModule->GetCR();break;
		case ZSR_LIGHTNING	: nResist = pModule->GetLR();break;
		
		case ZSR_NONE		:
		default				: return true;
	}

	switch(ResistType)
	{
		case ZSR_FIRE		: 
		case ZSR_POISON		: 
			*pfDamage = *pfDamage * ( 100.f - nResist ) * .01f;
			return true;

		case ZSR_COLD		: 
		case ZSR_LIGHTNING	: 
			nResist = nResist - nDifficulty;
			if(rand()%100 > nResist) return true;
			return false;
	}

	_ASSERT(pfDamage!=NULL);
	return true;
}


#define ZAS_SKILL			"SKILL"
#define ZAS_ID				"id"
#define ZAS_NAME			"name"
#define ZAS_EFFCTTYPE		"effecttype"
#define ZAS_HITCHECK		"hitcheck"
#define ZAS_GUIDABLE		"guidable"
#define ZAS_THROUGHNPC		"throughnpc"
#define ZAS_SURFACEMOUNT	"surfacemount"
#define ZAS_VELOCITY		"velocity"
#define ZAS_DELAY			"delay"
#define ZAS_LIFETIME		"lifetime"
#define ZAS_EFFECTSTARTTIME	"effectstarttime"
#define ZAS_EFFECTTIME		"effecttime"
#define ZAS_EFFECTAREA		"effectarea"
#define ZAS_EFFECTAREAMIN	"effectareamin"
#define ZAS_EFFECTANGLE		"effectangle"
#define ZAS_RESISTTYPE		"resisttype"
#define ZAS_DIFFICULTY		"difficulty"
#define ZAS_MOD_DAMAGE		"mod.damage"
#define ZAS_MOD_LAST_DAMAGE	"mod.lastdamage"
#define ZAS_MOD_DOT			"mod.dot"
#define ZAS_MOD_CRITICAL	"mod.criticalrate"
#define ZAS_MOD_SPEED		"mod.speed"
#define ZAS_MOD_ANTIMOTION	"mod.antimotion"
#define ZAS_MOD_ROOT		"mod.root"
#define ZAS_MOD_HEAL		"mod.heal"
#define ZAS_MOD_REPAIR		"mod.repair"
#define ZAS_CASTINGANIMATION "castinganimation"
#define ZAS_TARGETANIMATION	"targetanimation"
#define ZAS_MESSAGE			"message"
#define ZAS_EFFECTSOUND		"effectsound"
#define ZAS_CAMERA_SHOCK	"camera.shock"
#define ZAS_CAMERA_POWER	"camera.power"
#define ZAS_CAMERA_DURATION	"camera.duration"
#define ZAS_CAMERA_RANGE	"camera.range"
#define ZAS_EXPLOSIONSOUND	"sound.explosion"
#define ZAS_DRAW_TRACK		"draw.track"
#define ZAS_OFF_DLIGHT		"offdlight"
#define ZAS_COL_RADIUS		"colradius"
#define ZAS_KNOCKBACK		"knockback"

#define ZAS_CASTINGEFFECTTYPE		"castingeffectType"		// ����Ʈ ���� ��� Ÿ��
#define ZAS_CASTINGPREEFFECTTYPE	"castingpreeffectType"	// ����Ʈ ���� ��� Ÿ��
#define ZAS_CASTINGEFFECTADDPOS		"castingeffectAddPos"	// ����Ʈ ���� ���� ��ġ
#define ZAS_CASTINGEFFECT			"castingeffect"			// ����Ʈ �̸�
#define ZAS_CASTINGEFFECTSP			"castingeffectSp"		// Ư�� ����Ʈ �̸�
#define ZAS_CASTINGEFFECTSPCOUNT	"castingeffectSpCount"	// Ư�� ����Ʈ ����
#define ZAS_CASTINGPREEFFECT		"castingpreeffect"		// ��ų �ߵ��� ���ÿ� ���� ����Ʈ

#define ZAS_EFFECTSTARTPOSTYPE		"effect_startpos_type"	// ��ų �ߵ��� ���ÿ� ���� ����Ʈ

#define ZAS_TARGETEFFECT	 "targeteffect"			// ����Ʈ �̸�
#define ZAS_TRAILEFFECT		 "traileffect"			// ����Ʈ �̸�
#define ZAS_TRAILEFFECTTYPE	 "traileffecttype"		// ����Ʈ ����
#define ZAS_TRAILEFFECTSCALE "traileffectscale"		// ����Ʈ ������

#define ZAS_ANGLE					"angle"


ZSkillManager::~ZSkillManager()
{
	Destroy();
}

void ZSkillManager::Destroy()
{
	while(!empty()) {
		delete begin()->second;
		erase(begin());
	}
}

bool ZSkillManager::Create()
{
	
#ifdef _DEBUG
	FILE* fp = fopen("zskillxml.cache", "rb");
	if (fp)
	{
		mlog("load zskillxml.cache !\n");
		int iCount = (int)size();
		fread(&iCount, sizeof(int), 1, fp);
		ZSkillDesc* pDesc;
		for (int i=0; i<iCount; ++i)
		{
			pDesc = new ZSkillDesc;
			delete pDesc->pRepeatList;
			fread(pDesc, sizeof(ZSkillDesc), 1, fp);
			(*this)[pDesc->nID] = pDesc;
			pDesc->pRepeatList = new vector<ZSkillRepeat>;
			size_t numRepeat;
			fread(&numRepeat, sizeof(size_t), 1, fp);
			ZSkillRepeat skillRepeat;
			for (int i=0; i<(int)numRepeat; ++i)
			{
				fread(&skillRepeat, sizeof(ZSkillRepeat), 1, fp);
				pDesc->pRepeatList->push_back(skillRepeat);
			}
		}
		fclose(fp);
		return true;
	}

	mlog("failed to load zskillxml.cache, load zskill.xml\n");
#endif

	MXmlDocument	xmlDocument;

	xmlDocument.Create();

	char *buffer;
	MZFile mzf;

	string strFileNameZSkill(FILENAME_ZSKILL_DESC);
//#ifndef _DEBUG
//	strFileNameZSkill += ".mef";
//#endif
	if(!mzf.Open(strFileNameZSkill.c_str(), ZGetFileSystem())) 
	{
		if(!mzf.Open(strFileNameZSkill.c_str())) 
		{
			xmlDocument.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength()+1];
	buffer[mzf.GetLength()] = 0;

	mzf.Read(buffer,mzf.GetLength());

	if(!xmlDocument.LoadFromMemory(buffer))
	{
		xmlDocument.Destroy();
		delete[] buffer;
		return false;
	}
	delete[] buffer;
	mzf.Close();


	MXmlElement rootElement, element;
	char szTagName[256];
	char szTemp[256];
	char szAttrName[256];
	char szAttrValue[256];

	rootElement = xmlDocument.GetDocumentElement();
	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		element = rootElement.GetChildNode(i);
		element.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, ZAS_SKILL))
		{
			ZSkillDesc *pNewSkill = new ZSkillDesc;

			element.GetAttribute(&pNewSkill->nID,				ZAS_ID);
			element.GetAttribute(pNewSkill->szName,				ZAS_NAME);
			element.GetAttribute((int*)&pNewSkill->nEffectType, ZAS_EFFCTTYPE);
			element.GetAttribute(&pNewSkill->bHitCheck,			ZAS_HITCHECK);
			element.GetAttribute(&pNewSkill->bGuidable,			ZAS_GUIDABLE);
			element.GetAttribute(&pNewSkill->bThroughNPC,		ZAS_THROUGHNPC);
			element.GetAttribute(&pNewSkill->bSurfaceMount,		ZAS_SURFACEMOUNT);
			element.GetAttribute(&pNewSkill->fVelocity,			ZAS_VELOCITY,			2500.f);
			element.GetAttribute(&pNewSkill->nDelay,			ZAS_DELAY);
			element.GetAttribute(&pNewSkill->nLifeTime,			ZAS_LIFETIME);
			element.GetAttribute(&pNewSkill->nEffectStartTime,	ZAS_EFFECTSTARTTIME);
			element.GetAttribute(&pNewSkill->nEffectTime,		ZAS_EFFECTTIME);
			element.GetAttribute(&pNewSkill->fEffectArea,		ZAS_EFFECTAREA);
			element.GetAttribute(&pNewSkill->fEffectAreaMin,	ZAS_EFFECTAREAMIN);
			element.GetAttribute((int*)&pNewSkill->ResistType,	ZAS_RESISTTYPE);
			element.GetAttribute(&pNewSkill->nDifficulty,		ZAS_DIFFICULTY);
			element.GetAttribute(&pNewSkill->nModDamage,		ZAS_MOD_DAMAGE);
			element.GetAttribute(&pNewSkill->nModLastDamage,	ZAS_MOD_LAST_DAMAGE);
			element.GetAttribute(&pNewSkill->nModDoT,			ZAS_MOD_DOT);
			element.GetAttribute(&pNewSkill->nModCriticalRate,	ZAS_MOD_CRITICAL);
			element.GetAttribute(&pNewSkill->nModSpeed,			ZAS_MOD_SPEED);
			element.GetAttribute(&pNewSkill->bModAntiMotion,	ZAS_MOD_ANTIMOTION);			
			element.GetAttribute(&pNewSkill->bModRoot,			ZAS_MOD_ROOT);
			element.GetAttribute(&pNewSkill->nModHeal,			ZAS_MOD_HEAL);
			element.GetAttribute(&pNewSkill->nModRepair,		ZAS_MOD_REPAIR);
			element.GetAttribute(&pNewSkill->nCastingAnimation,	ZAS_CASTINGANIMATION	,1);
			element.GetAttribute(&pNewSkill->nTargetAnimation,	ZAS_TARGETANIMATION		,0);
			element.GetAttribute(pNewSkill->szMessage,			ZAS_MESSAGE);
			element.GetAttribute(pNewSkill->szEffectSound,		ZAS_EFFECTSOUND);
			element.GetAttribute(pNewSkill->szExplosionSound,	ZAS_EXPLOSIONSOUND);
			element.GetAttribute(&pNewSkill->bCameraShock,		ZAS_CAMERA_SHOCK		, true);
			element.GetAttribute(&pNewSkill->fCameraPower,		ZAS_CAMERA_POWER		, 1.0f);
			element.GetAttribute(&pNewSkill->fCameraDuration,	ZAS_CAMERA_DURATION		, 0.5f);
			element.GetAttribute(&pNewSkill->fCameraRange,		ZAS_CAMERA_RANGE		, 1500.0f);
			element.GetAttribute(&pNewSkill->bDrawTrack,		ZAS_DRAW_TRACK);
			element.GetAttribute(&pNewSkill->bOffDLight,		ZAS_OFF_DLIGHT			, false);
			element.GetAttribute(&pNewSkill->fColRadius,		ZAS_COL_RADIUS);
			element.GetAttribute(&pNewSkill->fModKnockback,		ZAS_KNOCKBACK			, 0.0f);
			

			element.GetAttribute((int*)&pNewSkill->nCastingEffectType,ZAS_CASTINGEFFECTTYPE);
			element.GetAttribute((int*)&pNewSkill->nCastingPreEffectType,ZAS_CASTINGPREEFFECTTYPE);
			element.GetAttribute((int*)&pNewSkill->nEffectStartPosType,ZAS_EFFECTSTARTPOSTYPE);

			element.GetAttribute(szTemp,ZAS_CASTINGEFFECTADDPOS);

			

			int nAngle=90;
			if (element.GetAttribute(&nAngle,						ZAS_EFFECTANGLE))
			{
				pNewSkill->fEffectAngle = ToRadian(nAngle);
			}

			

			if(szTemp[0]) {
			
			sscanf(szTemp,"%f %f %f",	&pNewSkill->vCastingEffectAddPos.x,
										&pNewSkill->vCastingEffectAddPos.y,
										&pNewSkill->vCastingEffectAddPos.z );
			}
			else 
				pNewSkill->vCastingEffectAddPos = rvector(0,0,0);


			element.GetAttribute(pNewSkill->szCastingEffect			,ZAS_CASTINGEFFECT);
			element.GetAttribute(pNewSkill->szCastingEffectSp		,ZAS_CASTINGEFFECTSP);
			element.GetAttribute(&pNewSkill->nCastingEffectSpCount	,ZAS_CASTINGEFFECTSPCOUNT);
			element.GetAttribute(pNewSkill->szCastingPreEffect		,ZAS_CASTINGPREEFFECT);

			element.GetAttribute(pNewSkill->szTargetEffect			,ZAS_TARGETEFFECT);
			element.GetAttribute(pNewSkill->szTrailEffect			,ZAS_TRAILEFFECT);
			element.GetAttribute((int*)&pNewSkill->nTrailEffectType	,ZAS_TRAILEFFECTTYPE);
			element.GetAttribute(&pNewSkill->fTrailEffectScale		,ZAS_TRAILEFFECTSCALE);


			int iChildCount = element.GetChildNodeCount();
			MXmlElement chrElement;
			for (int k = 0; k < iChildCount; k++)
			{
				chrElement = element.GetChildNode(k);
				chrElement.GetTagName(szTagName);
				if (szTagName[0] == '#') continue;

				// REPEAT �±� --------------------
				if (!stricmp(szTagName, "REPEAT"))
				{
					int nAttrCount = chrElement.GetAttributeCount();
					ZSkillRepeat repeat;
					memset(&repeat, 0, sizeof(ZSkillRepeat));

					for (int t = 0; t < nAttrCount; t++)
					{
						chrElement.GetAttribute(t, szAttrName, szAttrValue);
						if (!stricmp(szAttrName, ZAS_DELAY))
						{
							repeat.fDelay = (float)atof(szAttrValue);
						}
						else if (!stricmp(szAttrName, ZAS_ANGLE))
						{
							if (szAttrValue[0])
							{
								sscanf(szAttrValue,"%f %f %f",	&repeat.vAngle.x,
														&repeat.vAngle.y,
														&repeat.vAngle.z );
							}
							else 
								repeat.vAngle = rvector(0,0,0);

						}
					}
					pNewSkill->pRepeatList->push_back(repeat);
				}
			}


			if(find(pNewSkill->nID)==end())
				insert(value_type(pNewSkill->nID, pNewSkill));
			else {
				_ASSERT(false);
			}
		}
	}

#ifdef _DEBUG
	fp = fopen("zskillxml.cache", "wb");
	if (fp)
	{
		mlog("dump zskillxml.cache !\n");
		int iCount = (int)size();
		fwrite(&iCount, sizeof(int), 1, fp);
		ZSkillDesc* pDesc;
		for (iterator it=begin(); it!=end(); ++it)
		{
			pDesc = it->second;
			fwrite(pDesc, sizeof(ZSkillDesc), 1, fp);

			size_t numRepeat = pDesc->pRepeatList->size();
			fwrite(&numRepeat, sizeof(size_t), 1, fp);
			if (numRepeat > 0)
			{
				ZSkillRepeat* pArray = &*(pDesc->pRepeatList->begin());
				fwrite(pArray, sizeof(ZSkillRepeat), pDesc->pRepeatList->size(), fp);
			}
		}
		fclose(fp);
	}
#endif

	xmlDocument.Destroy();
	return true;

}

ZSkill::ZSkill()
{
	m_pDesc = NULL;
	m_pOwner = NULL;
	m_nUseNum = 0;
	m_uidTarget = MUID(0,0);
}

ZSkill::~ZSkill()
{
}

bool ZSkill::Init(int nID, ZObject *pOwner)
{
	ZSkillManager::iterator itr = ZGetApplication()->GetSkillManager()->find(nID);
	if(itr==ZGetApplication()->GetSkillManager()->end()) return false;

	m_pDesc = itr->second;
	m_pOwner = pOwner;


	m_nDelay		= m_pDesc->nDelay    + ( RandomNumber( 0,    (m_pDesc->nDelay / 10))       - (m_pDesc->nDelay / 20));
//	m_fVelocity		= m_pDesc->fVelocity + ( RandomNumber( 0.0f, (m_pDesc->fVelocity / 10.0f)) - (m_pDesc->fVelocity / 20.0f));

	InitStatus();
	return true;
}

void ZSkill::InitStatus()
{
	m_bEnable = false;
	m_fLastBeginTime = ZGetGame()->GetTime() - .001f * m_nDelay;		
	m_nUseNum = 0;
}

#define DAMAGE_DELAY	1.f			// ������ �ִ� ����

bool ZSkill::Update(float fElapsed)
{
	if(!m_bEnable) return false;

	if ((!m_pDesc->pRepeatList->empty()) && ((m_nUseNum-1) < (int)m_pDesc->pRepeatList->size()))
	{
		if (m_RepeatUTimer.Update(fElapsed))
		{
			Repeat();
		}
		return true;
	}

	if(ZGetGame()->GetTime()-m_fLastBeginTime > m_pDesc->nEffectTime*0.001f) {
		return false;
	}

	if(ZGetGame()->GetTime()>m_fNextDamageTime) {
		m_fNextDamageTime+=DAMAGE_DELAY;

		for(ZObjectManager::iterator i = ZGetObjectManager()->begin();i!=ZGetObjectManager()->end();i++) {
			ZObject *pTarget = i->second;
			if(pTarget->IsDie()) continue;	// ����npc �� ����
			float fDamage = m_pDesc->nModDoT;
			if(CheckRange(pTarget->GetPosition(),pTarget) && m_pDesc->CheckResist(pTarget,&fDamage)) {
				if(ZGetGame()->CanAttack(m_pOwner,pTarget)) {
					if(fDamage)
						pTarget->OnDamaged(m_pOwner,m_TargetPos,ZD_MELEE,MWT_KATANA,fDamage,m_pDesc->nModCriticalRate*.01f);
				}
			}
		}
	}
	return true;
} 

bool ZSkill::IsReady()
{
	return ZGetGame()->GetTime() - m_fLastBeginTime > 0.001f * m_pDesc->nDelay;
}

bool ZSkill::IsEnable()
{
	return m_bEnable;
}

// ����� ��ȿ�������� �ִ��� �Ǵ��Ѵ�
bool ZSkill::CheckRange(const rvector& center, ZObject *pCurrent)
{
	// �߻��ϴ� ����� ����� ������ ��ȿ�ϴ�.
	if(m_pDesc->bHitCheck)
	{
		if (MIsDerivedFromClass(ZActor, m_pOwner))
		{
			ZActor* pActor = (ZActor*)m_pOwner;
//			if (pActor->CanAttackRange(pCurrent)) return true;
			if (pActor->CanSee(pCurrent)) return true;
			else return false;
		}
	}


	switch(m_pDesc->nEffectType) {
		case ZSE_WHOLE_AREA : return true;
		case ZSE_ENEMY :
			if(ZGetGame()->CanAttack(m_pOwner,pCurrent)) return true;
			return false;
		case ZSE_ALLIED :
			if(m_pOwner->GetTeamID()==pCurrent->GetTeamID()) return true;
			//if(m_uidTarget==pCurrent->GetUID()) return true;
			return false;
		case ZSE_SLASH_AREA :
			{
				if (MIsDerivedFromClass(ZActor, m_pOwner))
				{
					ZActor* pActor = (ZActor*)m_pOwner;
					if (pActor->CanAttackMelee(pCurrent, m_pDesc))
					{
						if( ZGetGame()->CanAttack(m_pOwner,pCurrent) ) 
						{
							return true;
						}
					}
				}

				return false;
			}break;
		case ZSE_ALLIED_AREA :
		case ZSE_ENEMY_AREA : {
			if(Magnitude(pCurrent->GetPosition() - center)<m_pDesc->fEffectArea*100.f)
				return true;
			return false;
			}
		case ZSE_OWNER_AREA :
			if(Magnitude(pCurrent->GetPosition() - m_pOwner->GetPosition())<m_pDesc->fEffectArea*100.f)
				return true;
			return false;
		default :
 			_ASSERT(false);
			return false;
	}
}

void ZSkill::PreExecute(MUID& uidTarget, rvector& targetPos )
{
	RMeshPartsPosInfoType type = eq_parts_pos_info_etc;
	MUID uid = m_pOwner->GetUID();

	if(GetPartsTypePos(type,uid,uidTarget)==false)
		return;

	if(m_pDesc->szCastingPreEffect[0]) {
		ZGetEffectManager()->AddPartsPosType(m_pDesc->szCastingPreEffect, uid ,type,m_pDesc->nEffectTime);
	}
}

void ZSkill::LastExecute(MUID& uidTarget, rvector& targetPos )
{
	// ����� ������ ������ �� �͵�..
	for(ZObjectManager::iterator i = ZGetObjectManager()->begin();i!=ZGetObjectManager()->end();i++) {
		ZObject *pTarget = i->second;
		if(pTarget->IsDie()) continue;	// ����npc �� ����
		float fDamage = m_pDesc->nModLastDamage;
		if(CheckRange(pTarget->GetPosition(),pTarget) && m_pDesc->CheckResist(pTarget,&fDamage)) {
			if(ZGetGame()->CanAttack(m_pOwner,pTarget)) {
				if(fDamage)
					pTarget->OnDamaged(m_pOwner,m_TargetPos,ZD_MELEE,MWT_KATANA,fDamage,m_pDesc->nModCriticalRate*.01f);
			}
			else {
				if(m_pDesc->nModHeal)
					pTarget->OnHealing(m_pOwner,m_pDesc->nModHeal,m_pDesc->nModRepair);
			}
		}
	}
}

void ZSkill::Execute(MUID& uidTarget, rvector& targetPos )
{
	m_bEnable = true;
	m_TargetPos = targetPos;
	m_uidTarget = uidTarget;
	m_nUseNum = 0;

	// ��ų ���
	Use(uidTarget, targetPos);
}

void ZSkill::Use(MUID& uidTarget, rvector& targetPos)
{
	m_nUseNum++;
	m_fLastBeginTime = ZGetGame()->GetTime();
	m_fNextDamageTime = m_fLastBeginTime + DAMAGE_DELAY;
	ZObject *pTargetObject = ZGetObjectManager()->GetObject( uidTarget);
	if (pTargetObject == NULL)
		return;

	// �߻� �Ǵ� �����̴�
	if ( m_pDesc->bHitCheck)
	{
		rvector vMissilePos, vMissileDir;
		GetMissilePosDir( vMissileDir, vMissilePos, targetPos);

		ZGetGame()->m_WeaponManager.AddMagic( this, vMissilePos, vMissileDir, m_pOwner);

		int nRepeatIndex = m_nUseNum - 1;
		if ((m_pDesc->pRepeatList->empty()) || ((nRepeatIndex) >= (int)m_pDesc->pRepeatList->size()))
		{
			m_bEnable = false;
		}
		else
		{
			ZSkillRepeat repeat = (*m_pDesc->pRepeatList)[ nRepeatIndex];
			m_RepeatUTimer.Init( repeat.fDelay);
		}
	}
	
	// �߻�Ǵ� ���Ⱑ �ƴϴ�.
	else
	{
		// ī�޶� ȿ��
		if ( m_pDesc->bCameraShock)
		{
			ZCharacter *pTargetCharacter=ZGetGameInterface()->GetCombatInterface()->GetTargetCharacter();
			const float fDefaultPower = 500.0f;
			float fShockRange = m_pDesc->fCameraRange;
			float fDuration = m_pDesc->fCameraDuration;
			float fPower= ( fShockRange-Magnitude( pTargetCharacter->GetPosition() + rvector( 0, 0, 50) - targetPos)) / fShockRange;
			fPower *= m_pDesc->fCameraPower;
			
			if ( fPower > 0)
				ZGetGameInterface()->GetCamera()->Shock( fPower*fDefaultPower, fDuration, rvector( 0.0f, 0.0f, -1.0f));
		}

		for ( ZObjectManager::iterator i = ZGetObjectManager()->begin();  i!=ZGetObjectManager()->end();  i++)
		{
			ZObject *pObject = i->second;

			// ����npc �� ����
			if(pObject->IsDie())
				continue;

			float fDamage = m_pDesc->nModDamage;

			//if ( CheckRange( pObject->GetPosition(),pObject) && m_pDesc->CheckResist( pObject, &fDamage))
			if ( CheckRange( pTargetObject->GetPosition(), pObject) && m_pDesc->CheckResist( pObject, &fDamage))
			{
				
				// ���������ش�
				if ( ZGetGame()->CanAttack( m_pOwner, pObject))
				{
					if ( fDamage && (uidTarget == pObject->GetUID()))
						pObject->OnDamaged( m_pOwner, m_TargetPos, ZD_MELEE, MWT_KATANA, fDamage, m_pDesc->nModCriticalRate*.01f);
				}
				else
				{
					if ( m_pDesc->nModHeal)
						pObject->OnHealing( m_pOwner, m_pDesc->nModHeal, m_pDesc->nModRepair);
				}

				// root �Ӽ� (�������δ�)
				if ( m_pDesc->bModRoot)
				{
					if ( pObject)
					{
						float fDuration = m_pDesc->nEffectTime * 0.001f;
						pObject->OnStun( fDuration);
						ZModule_Movable *pMovableModule = (ZModule_Movable*)pObject->GetModule( ZMID_MOVABLE);
						if ( pMovableModule)
							pMovableModule->SetMoveSpeedRestrictRatio( 0, fDuration);
					}
				}

				// knockback
				if ( m_pDesc->fModKnockback != 0.0f)
				{
					if ( pObject)
					{
						rvector dir = pObject->GetPosition() - (targetPos + rvector( 0, 0, 80));
						Normalize( dir);
						pObject->AddVelocity( m_pDesc->fModKnockback * 7.f * -dir);
					}
				}
			}
		}
	}


	// slow �� �ӵ��� ���̴°�
	if ( m_pDesc->nModSpeed<100)
	{
		if ( pTargetObject)
		{
			ZModule_Movable *pMovableModule = (ZModule_Movable*)pTargetObject->GetModule( ZMID_MOVABLE);
			if ( pMovableModule)
				pMovableModule->SetMoveSpeedRestrictRatio( m_pDesc->nModSpeed*0.01f, m_pDesc->nEffectTime*0.001f);
		}
	}


	// ��ġ�� ������ �ش�..
	rvector vPos = m_pOwner->GetPosition();
	rvector vDir = m_pOwner->GetDirection();

	RMeshPartsPosInfoType type = eq_parts_pos_info_etc;

	GetPartsTypePos(pTargetObject, m_pDesc->nCastingEffectType, type,vPos,vDir);

	vPos = vPos + vDir* m_pDesc->vCastingEffectAddPos.x + rvector(0,0,m_pDesc->vCastingEffectAddPos.y);

	if(m_pDesc->szCastingEffect[0]) {

		if(type != eq_parts_pos_info_etc)
			ZGetEffectManager()->AddPartsPosType(m_pDesc->szCastingEffect, pTargetObject->GetUID(),type,m_pDesc->nEffectTime);
		else 
			ZGetEffectManager()->Add(m_pDesc->szCastingEffect, vPos,vDir,m_pOwner->GetUID(),m_pDesc->nEffectTime);
	}

	if(m_pDesc->szCastingEffectSp[0]) {

		ZGetEffectManager()->AddSp(m_pDesc->szCastingEffectSp,m_pDesc->nCastingEffectSpCount,
			vPos,vDir,m_pOwner->GetUID());
	}

	if(m_pDesc->szMessage[0]) {
		ZChatOutput( m_pDesc->szMessage, ZChat::CMT_SYSTEM );
	}

	if(m_pDesc->szEffectSound[0]) {
		bool bObserverTarget = uidTarget==ZGetCombatInterface()->GetTargetUID();
		char szSoundName[64];
		char *pszSoundName = szSoundName;
		if(bObserverTarget) {
			sprintf(szSoundName,"%s_2d",m_pDesc->szEffectSound);
			if(!ZGetSoundEngine()->isPlayAble(szSoundName))
				pszSoundName = m_pDesc->szEffectSound;
		}
		rvector pos = m_pOwner->GetPosition();
		ZGetSoundEngine()->PlaySound(pszSoundName,m_pOwner->GetPosition(),false);
	}

}

bool ZSkill::GetPartsTypePos(RMeshPartsPosInfoType& type,MUID& uid,MUID& uidTarget)
{
	if( m_pDesc->nCastingPreEffectType == ZSTP_TARGETHEAD ) {
		type = eq_parts_pos_info_HeadNub;
		uid = uidTarget;
	} 
	else if( m_pDesc->nCastingPreEffectType == ZSTP_EFFECT ) {
		type = eq_parts_pos_info_Effect;
	}
	else if( m_pDesc->nCastingPreEffectType == ZSTP_SPINE1 ) {
		type = eq_parts_pos_info_Spine1;
	}
	else if( m_pDesc->nCastingPreEffectType == ZSTP_LHAND ) {
		type = eq_parts_pos_info_LHand;
	}
	else if( m_pDesc->nCastingPreEffectType == ZSTP_RHAND ) {
		type = eq_parts_pos_info_RHand;
	}
	else if( m_pDesc->nCastingPreEffectType == ZSTP_LFOOT ) {
		type = eq_parts_pos_info_LFoot;
	}
	else if( m_pDesc->nCastingPreEffectType == ZSTP_RFOOT ) {
		type = eq_parts_pos_info_RFoot;
	}
	else {
		return false;
	}
	return true;
}

bool ZSkill::GetPartsTypePos(ZObject* pTargetObject, ZSKILLEFFECTTARGETPOSTYPE nSkillEffectPosType, RMeshPartsPosInfoType& type,rvector& vPos,rvector& vDir)
{
	if( pTargetObject && nSkillEffectPosType == ZSTP_TARGET ) {//���� ĳ���Ͱ� �ִ� ������ ��ġ

		rvector vNormal;
		rvector vOut;

		vPos = pTargetObject->GetPosition();
		vDir = pTargetObject->GetDirection();

		if( ZGetGame()->GetWorld()->GetBsp()->GetShadowPosition( vPos, rvector( 0, 0, -1), &vNormal, &vOut ))
		{
			vPos = vOut;
		}
	}
	else if(pTargetObject && nSkillEffectPosType == ZSTP_TARGETHEAD) {

		vPos = pTargetObject->GetPosition();
		vDir = pTargetObject->GetDirection();
		type = eq_parts_pos_info_Head;
	}
	else if( nSkillEffectPosType == ZSTP_EFFECT) {
		type = eq_parts_pos_info_Effect;
		vPos = pTargetObject->m_pVMesh->GetBipTypePosition(type);
		vDir = pTargetObject->GetDirection();
	}
	else if( nSkillEffectPosType == ZSTP_SPINE1) {
		type = eq_parts_pos_info_Spine1;
		vPos = pTargetObject->m_pVMesh->GetBipTypePosition(type);
		vDir = pTargetObject->GetDirection();
	}
	else if( nSkillEffectPosType == ZSTP_HEAD ) {
		type = eq_parts_pos_info_Head;
		vPos = pTargetObject->m_pVMesh->GetBipTypePosition(type);
		vDir = pTargetObject->GetDirection();
	}
	else if( nSkillEffectPosType == ZSTP_LHAND ) {
		type = eq_parts_pos_info_LHand;
		vPos = pTargetObject->m_pVMesh->GetBipTypePosition(type);
		vDir = pTargetObject->GetDirection();
	}
	else if( nSkillEffectPosType == ZSTP_RHAND ) {
		type = eq_parts_pos_info_RHand;
		vPos = pTargetObject->m_pVMesh->GetBipTypePosition(type);
		vDir = pTargetObject->GetDirection();
	}
	else if( nSkillEffectPosType == ZSTP_LFOOT ) {
		type = eq_parts_pos_info_LFoot;
		vPos = pTargetObject->m_pVMesh->GetBipTypePosition(type);
		vDir = pTargetObject->GetDirection();
	}
	else if( nSkillEffectPosType == ZSTP_RFOOT ) {
		type = eq_parts_pos_info_RFoot;
		vPos = pTargetObject->m_pVMesh->GetBipTypePosition(type);
		vDir = pTargetObject->GetDirection();
	}
	else 
		return false;

	return true;
}

bool ZSkill::IsUsable(ZObject *pTarget)	// �� ��ų�� ��󿡰� ��������
{
	if (m_pDesc->IsEffectiveTo(pTarget) && CheckRange(pTarget->GetPosition(),pTarget))
		return true;

	return false;
}


void ZSkill::Cancel(MUID& uidTarget)
{
	m_fLastBeginTime = ZGetGame()->GetTime();
}

void ZSkill::Repeat()
{
	rvector vMissilePos, vMissileDir;
	GetMissilePosDir(vMissileDir, vMissilePos, m_TargetPos);
	ZSkillRepeat repeat = (*m_pDesc->pRepeatList)[m_nUseNum-1];

	rmatrix rotmat;
	D3DXMatrixIdentity( &rotmat );
	D3DXMatrixRotationX(&rotmat, repeat.vAngle.x);
	D3DXMatrixRotationY(&rotmat, repeat.vAngle.y);
	D3DXMatrixRotationZ(&rotmat, repeat.vAngle.z);
	rvector dir = vMissileDir * rotmat;

	rvector tarpos = vMissilePos + (500.0f * dir);

	Use(m_uidTarget, tarpos);
}

void ZSkill::GetMissilePosDir(rvector& outDir, rvector& outPos, rvector& TargetPos)
{
	outPos = m_pOwner->GetPosition() + rvector(0,0,m_pOwner->GetCollHeight()/2.0f);
	outDir = m_pOwner->GetDirection();
	RMeshPartsPosInfoType type = eq_parts_pos_info_etc;
	GetPartsTypePos(m_pOwner, m_pDesc->nEffectStartPosType, type, outPos, outDir);
	outDir = TargetPos - outPos;

	if ( m_pDesc->bSurfaceMount)
	{
		outDir.z = 0;
		outPos.z = m_pOwner->GetPosition().z;
	}

	Normalize(outDir);
}