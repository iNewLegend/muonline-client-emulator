#ifndef _ZBASEQUEST_H
#define _ZBASEQUEST_H

#include "ZGlobal.h"
#include "MBaseQuest.h"
#include "ZQuestMap.h"
#include "ZQuestGameInfo.h"
#include "ZMyItemList.h"
#include "ZNPCInfoFromServer.h"



#include <set>

enum ZQuestCheetType
{
	ZQUEST_CHEET_GOD			= 0,		// �������
	ZQUEST_CHEET_WEAKNPCS		= 1,		// �������� 1
	ZQUEST_CHEET_AMMO			= 2,		// �Ѿ˸���
	ZQUEST_CHEET_MAX
};

// ����Ʈ�� �����̹��� ���� �������̽�

// ���� ����Ʈ���� ������ �����̹���带 �߰��ϸ鼭, ����Ʈ�� Ŀ�ǵ峪 npc�ڵ� ���� ������ �� �������� �̰͵���
// ZGetQuest()�� �ʹ� ��������� ����ϰ� �־ �����̹��� �߰��Ϸ��� ū �Ը��� �����丵�� �Ͼ �� �ۿ� ����.
// �׷��� �̹� �� ���� �ǰ� �ִ� ����Ʈ �ڵ带 �����丵�ϴ� ���� ���� �߻� ���� ������ �ſ� �δ㽺����.
// �׷��� �����丵 ��� �����̹������� ����Ʈ �ڵ带 �ѹ� �� �����ؼ� �װ��� �����̹��� �°� �����ϱ�� �Ͽ���
// ���� �ߺ� �ڵ尡 ��������� ������ �� �����ϴٰ� �Ǵ���..
// ������,
// Ŭ���̾�Ʈ �ڵ��� ���� �κ��� ZGetQuest()�� ����ϰ� ������ ��ġ �̰��� �̱����̳� ���������� ���� ���ۿ��� ���Ҵ�.
// ����Ʈ �ڵ尡 �ִ� ���� ������ �ǵ帮�� �������� �Ͽ����� ������ ZGetQuest()����ϴ� ���� ��� ������ ������ �� ���
// ::ZGetQuest()�� ����Ÿ���� ZSurvival�ϼ��� �ֵ��� �ϱ� ���ؼ� ���� ZQuest���� ������ �Ʒ��� �������̽��� �߰��Ͽ���.
// ����Ʈ �ڵ带 ������ �ǵ帮�� �ʱ� ���ؼ� ����Ʈ�� �����̹��� �������� ���� �� �ִ� �ڵ尡 �ִ��� ����� �ű��� �ʾƼ�
// ��� �Լ��� ���������Լ��̴�.

class ZBaseQuest : public MBaseQuest
{
#ifdef _QUEST_ITEM
    virtual bool OnRewardQuest( MCommand* pCmd ) = 0;
	virtual bool OnNewMonsterInfo( MCommand* pCmd ) = 0;

	virtual void GetMyObtainQuestItemList( int nRewardXP, int nRewardBP, void* pMyObtainQuestItemListBlob, void* pMyObtainZItemListBlob ) = 0;

public :
	virtual int GetRewardXP( void) = 0;
	virtual int GetRewardBP( void) = 0;
	virtual bool IsQuestComplete( void) = 0;
	virtual bool IsRoundClear( void) = 0;
	virtual DWORD GetRemainedTime( void) = 0;

	virtual MQuestCombatState GetQuestState() = 0;

	virtual ZNPCInfoFromServerManager& GetNPCInfoFromServerMgr() = 0;

#endif

	virtual bool OnNPCSpawn(MCommand* pCommand) = 0;
	virtual bool OnNPCDead(MCommand* pCommand) = 0;
	virtual bool OnPeerNPCDead(MCommand* pCommand) = 0;
	virtual bool OnEntrustNPCControl(MCommand* pCommand) = 0;
	virtual bool OnPeerNPCBasicInfo(MCommand* pCommand) = 0;
	virtual bool OnPeerNPCHPInfo(MCommand* pCommand) = 0;
	virtual bool OnPeerNPCAttackMelee(MCommand* pCommand) = 0;
	virtual bool OnPeerNPCAttackRange(MCommand* pCommand) = 0;
	virtual bool OnPeerNPCSkillStart(MCommand* pCommand) = 0;
	virtual bool OnPeerNPCSkillExecute(MCommand* pCommand) = 0;
	virtual bool OnPeerNPCBossHpAp(MCommand* pCommand) = 0;
	virtual bool OnRefreshPlayerStatus(MCommand* pCommand) = 0;
	virtual bool OnClearAllNPC(MCommand* pCommand) = 0;
	virtual bool OnQuestRoundStart(MCommand* pCommand) = 0;
	virtual bool OnQuestPlayerDead(MCommand* pCommand) = 0;
	virtual bool OnQuestGameInfo(MCommand* pCommand) = 0;
	virtual bool OnQuestCombatState(MCommand* pCommand) = 0;
	virtual bool OnMovetoPortal(MCommand* pCommand) = 0;
	virtual bool OnReadyToNewSector(MCommand* pCommand) = 0;
	virtual bool OnSectorStart(MCommand* pCommand) = 0;
	virtual bool OnObtainQuestItem(MCommand* pCommand) = 0;
	virtual bool OnObtainZItem(MCommand* pCommand) = 0;
	virtual bool OnSectorBonus(MCommand* pCommand) = 0;
	virtual bool OnQuestCompleted(MCommand* pCommand) = 0;
	virtual bool OnQuestFailed(MCommand* pCommand) = 0;
	virtual bool OnQuestPing(MCommand* pCommand) = 0;


	//ZQuestMap			m_Map;
	virtual void LoadNPCMeshes() = 0;
	virtual void LoadNPCSounds() = 0;
	virtual void MoveToNextSector() = 0;
	virtual void UpdateNavMeshWeight(float fDelta) = 0;
protected:
	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual bool OnCreateOnce() = 0;
	virtual void OnDestroyOnce() = 0;
public:
	ZBaseQuest() {}
	virtual ~ZBaseQuest() {}
public:
	virtual void OnGameCreate() = 0;
	virtual void OnGameDestroy() = 0;
	virtual void OnGameUpdate(float fElapsed) = 0;
	virtual bool OnCommand(MCommand* pCommand) = 0;				///< ���� �̿ܿ� ������� Ŀ�ǵ� ó��
	virtual bool OnGameCommand(MCommand* pCommand) = 0;			///< ������ ������� Ŀ�ǵ� ó��

	virtual void SetCheet(ZQuestCheetType nCheetType, bool bValue) = 0;
	virtual bool GetCheet(ZQuestCheetType nCheetType) = 0;

	virtual void Reload() = 0;
	virtual bool Load() = 0;

	
	// interface
	virtual ZQuestGameInfo* GetGameInfo() = 0;

	// ���¿� ������� ���ɼ� �ִ� ����Ʈ ���õ� Ŀ�ǵ�.
	virtual bool OnSetMonsterBibleInfo( MCommand* pCmd ) = 0;


	virtual bool OnPrePeerNPCAttackMelee(MCommand* pCommand) = 0;	// ������ ó���ϴ°� ��Ÿ�̹� �ʴ�
	
};


#endif