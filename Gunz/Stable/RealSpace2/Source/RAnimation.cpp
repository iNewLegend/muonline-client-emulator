#include "stdafx.h"

#include "RAnimation.h"

#include "RealSpace2.h"

#include "RMesh.h"

_USING_NAMESPACE_REALSPACE2

_NAMESPACE_REALSPACE2_BEGIN

/////////////////////////////////////////////////////////////////////////

RAnimation::RAnimation() 
{
	m_pAniData		= NULL;

	m_sound_name[0] = NULL;

	m_NameID		 = -1;

	m_weapon_motion_type = -1;

	m_ani_loop_type = RAniLoopType_Loop;

	m_bIsHaveSound = false;
	m_bSoundRelatedToMap = false;

	m_isConnected = false;

	m_isLoadDone = false;
}

RAnimation::~RAnimation() 
{
//	DestroyEventFile();
}
/*
//�ִϸ��̼� ���� ���� �߰��� �κ�....
void RAnimation::SetAniEventInfo(char * eventtype, char * filename, int beginframe, int ID)
{
	RAniEventInfo * pAniEventInfo = new RAniEventInfo();
	pAniEventInfo->SetBeginFrame(beginframe);
	pAniEventInfo->SetEventType(eventtype);
	pAniEventInfo->SetFileName(filename);
	pAniEventInfo->SetID(ID);

	m_vecAniEventInfo.push_back(pAniEventInfo);
}

bool RAnimation::isHaveEventInfo()
{
	if(m_vecAniEventInfo.empty())
		return false;
	return true;
}

void RAnimation::DestroyEventFile()
{
	while(isHaveEventInfo())
	{
		SAFE_DELETE(m_vecAniEventInfo.back());
		m_vecAniEventInfo.pop_back();
	}
}
*/
/////////////////////////////////////////////////////////////////////

AnimationType RAnimation::GetAnimationType() 
{
	if(!m_pAniData)	return RAniType_TransForm;
	return m_pAniData->m_ani_type;
}

int RAnimation::GetMaxFrame() 
{
	if(!m_pAniData)	return 0;
	return m_pAniData->m_max_frame;
}

int RAnimation::GetAniNodeCount() 
{
	if(!m_pAniData)	return 0;
	return m_pAniData->m_ani_node_cnt;
}

RAnimationNode* RAnimation::GetAniNode(int i) 
{
	if(!m_pAniData)	return 0;
	return m_pAniData->m_ani_node[i];
}

RAnimationNode* RAnimation::GetBipRootNode() 
{
	if(!m_pAniData)	return 0;
	return m_pAniData->m_pBipRootNode;
}

void RAnimation::SetWeaponMotionType(int wtype) 
{
	m_weapon_motion_type = wtype;
}

int  RAnimation::GetWeaponMotionType() 
{
	return m_weapon_motion_type;
}

RAnimationNode* RAnimation::GetNode(char* name)
{
	if(m_pAniData==NULL) 
		return NULL;

	return m_pAniData->GetNode(name);
}

void RAnimation::SetFileName(char* name)
{
	if(!name[0]) return;
	strcpy(m_filename,name);
}

char* RAnimation::GetFileName()
{
	return m_filename;
}

char* RAnimation::GetSoundFileName() 
{
	return m_sound_name;
}

bool RAnimation::IsHaveSoundFile() 
{
	return m_bIsHaveSound;
}

void RAnimation::SetLoadDone(bool b) 
{
	m_isLoadDone = b;
}

bool RAnimation::IsLoadDone() 
{
	return m_isLoadDone;
}

void RAnimation::ClearSoundFile(void)
{
	m_sound_name[0] = NULL;
	m_bIsHaveSound = false;
}

bool RAnimation::SetSoundFileName(char* pSoundName)
{
	if(!pSoundName[0])
		return false;
	strcpy(m_sound_name,pSoundName);
	m_bIsHaveSound = true;
	return true;
}

bool RAnimation::CheckWeaponMotionType(int wtype) {

	if(wtype == -1) // üũ�� �ʿ� ���� ���
		return true;

	if(m_weapon_motion_type == wtype)
		return true;
	return false;
}

bool RAnimation::LoadAni(char* filename)
{
	RAnimationFile* pAnimationFile = RGetAnimationFileMgr()->Add(filename);

	if(pAnimationFile==NULL ) 
		return false;

	m_pAniData = pAnimationFile;

	return true;
}

AnimationLoopType RAnimation::GetAnimationLoopType()
{
	if(RMesh::m_bToolMesh)
		return RAniLoopType_Loop;
	return m_ani_loop_type;
}

void RAnimation::SetAnimationLoopType(AnimationLoopType type)
{
	m_ani_loop_type = type;
}

/////////////////////////////////////////////////////////////////////


_NAMESPACE_REALSPACE2_END
