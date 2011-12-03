#ifndef MZONESERVER_H
#define MZONESERVER_H

#include "winsock2.h"
#include "MCharacterDesc.h"
#include "MServer.h"
#include "MMap.h"
#include "MSpawnManager.h"
#include "RTypes.h"
#include "MObjectCharacter.h"
#include <vector>
using namespace std;

class MObject;
class MObjectCharacter;
class MObjectItem;

//class MCharacterList : public map<MUID, MObjectCharacter*>{};
//class MItemList : public map<MUID, MObjectItem*>{};
class MObjectList : public map<MUID, MObject*>{};

/*!
	Zone-Server:
	- �� ������ ���� ������Ʈ ���� ����
	- ���� ����
		-# MZoneServer::Create()
		-# MZoneServer::ConnectToMaster()
		-# MZoneServer::do_something()
		-# MZoneServer::DisconnectToMaster()
		-# MZoneServer::Destroy()
*/
class MZoneServer : public MServer{
private:
	static MZoneServer*	m_pInstance;		///< ���� �ν��Ͻ�
protected:
	MUID			m_Master;				///< ������ ��Ʈ�ѷ��� UID

	MMapManager		m_Maps;					///< �ϳ��� Zone�ȿ� Map ����Ʈ
	MSpawnManager	m_SpawnManager;			///< Spawn ����

	bool			m_bRequestUIDToMaster;	///< UID �Ҵ��� ������ ��Ʈ�ѷ����� ��û�ߴ°�?

	//MCharacterList	m_Characters;
	//MItemList		m_Items;
	MObjectList		m_Objects;
	//MGridMap<MObjectCharacter*>	m_MapRefCharacter;
	//MGridMap<MObjectItem*>	m_MapRefItem;

	float			m_fSharedVisionRadius;	///< �þ� �����Ǵ� ����

	MUIDRANGE		m_CommAllocRange;		///< Ŀ�´������ͷ� �Ҵ��Ҽ� �ִ� ����
	MUIDRANGE		m_NextCommAllocRange;	///< Ŀ�´������ͷ� ������ �Ҵ��Ҽ� �ִ� ����

protected:
	/// Create()ȣ��ÿ� �Ҹ��� �Լ�
	virtual bool OnCreate(void);
	/// Destroy()ȣ��ÿ� �Ҹ��� �Լ�
	virtual void OnDestroy(void);
	/// ����� Ŀ�ǵ� ���
	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	/// ����� Ŀ�ǵ� ó��
	virtual bool OnCommand(MCommand* pCommand);
	/// ����� ����
	virtual void OnRun(void);

	/// �ʱ�ȭ
	void OnInitialize();
	/// �α��εǾ�����
	void OnLogin(MUID CommUID, MUID PlayerUID);
	/// Ŭ���̾�Ʈ���� Ŭ���� ���߰� �Ѵ�.
	void SetClientClockSynchronize(MUID& CommUID);
	void AssignPlayerCharacter(MUID& CommUID, MUID& PlayerUID);
	void ResponsePlayerCharacterAssign(MUID& CommUID, MObject* pObject);
	void ReleasePlayerCharacter(MUID& uid);
		
	/// Object�� ������ ��ȣ�ν� ������Ʈ
	void ResponseObjectUpdate(MUID& TargetUID, MObject* pObject);
	void ResponseCharacterBasicAttr(MUID& TargetUID, MUID& RequestUID);

	/// Object�� Skill��� ����
	void ResponseCharacterSkillList(const MUID& CommUID, const MUID& CharUID);

	/// Zone-Server�� ���� ������ Target���� ����
	void ResponseInfo(MUID& Target);
	void ResponseMapList(MUID& Target);

	/// DB���� ĳ���� ������ �� �ε�
	MObject* LoadCharacter(MUID& uid);
	/// ĳ���� ������ ��ε�. DB�� ����
	void UnloadCharacter(MUID& uid);

	/// UID�� ĳ���͸� ��ȯ, ������ NULL�� ��ȯ�Ѵ�.
	MObjectCharacter* FindCharacter(const MUID& uidChr);

	/// ĳ���ͻ����� ActionPipe ����
	void BuildActionPipe(MObjectCharacter* pCharacter);

	/// ��ȭ �޽���
	int MessageSay(MUID& uid, char* pszSay);

	/// ������Ʈ ����
	int ObjectAdd(MUID* pAllocUID, int nObjID, int nMapID, int nNodeID, rvector& Pos);
	/// ������Ʈ ����
	int ObjectRemove(const MUID& uid, MObjectList::iterator* pNextItor);

	/// ������Ʈ ���º���
	int ObjectMoveMode(const MUID& uid, int nMoveMode);
	int ObjectMode(const MUID& uid, int nMode);
	int ObjectState(const MUID& uid, int nState);

	/// ������Ʈ �̵�
	//int ObjectMove(MUID& uid, unsigned long int nStartTime, unsigned long int nEndTime, rvector& StartPos, rvector& EndPos);
	/// �н� ����Ʈ�� ������Ʈ �̵�
	int ObjectMove(MUID& uid, void* pPathArray);
	/// ������Ʈ ����
	int ObjectAttack(MUID& uid, MUID& uidTarget);
	/// ������Ʈ ��ų
	int ObjectSkill(const MUID& uid, const MUID& uidTarget, int nSkillID);
	/// Map�� Cell�ȿ� �ִ� Object List ����
	void ResponseObjectList(MUID& Target, int nMapID, int nNodeID);
	/// Map�� Object List ����
	void ResponseObjectList(MUID& Target, int nMapID);

	/// ������Ʈ�� ���� �ִ� �������� ���� or ����
	int ObjectEquipItem(MUID& TargetUID, MUID& ChrUID, MUID& ItemUID, MEquipmentSlot slot);

	/// Item ����
	MObjectItem* CreateObjectItem(int nItemDescID);
	MObjectItem* FindItem(MUID& ItemUID);		///< UID�� �̿��Ͽ� Item�� ã�´�.

	void ResponseEquipItem(MUID& TargetUID, MUID& ItemUID, MEquipmentSlot slot);

	/// uid���� �������� ������ش�.
	int AdminCreateItem(MUID uid, unsigned int nItemIndex);
	/// ĳ���� ������ �˷��ش�.
	void ResponseAdminCharacterInfo(MUID& TargetUID, MUID& RequestUID);

	/// ���ο� UID ����
	virtual MUID UseUID(void);
public:
	MZoneServer(void);
	virtual ~MZoneServer(void);

	/// ���� �ν��Ͻ� ���
	static MZoneServer* GetInstance(void);

	/// ���� Ŭ�� ����
	unsigned long int GetGlobalClockCount(void);

	/// �ʱ�ȭ
	bool Create(int nPort);
	/// ����
	void Destroy(void);

	/// Ŀ�ؼ��� �޾Ƶ��� ���
	virtual int OnAccept(MCommObject* pCommObj);
	/// Ŀ�ؼ��� �̷���� ���
	virtual int OnConnected(MUID* pTargetUID, MUID* pAllocUID, MCommObject* pCommObj);
	/// Ŀ�ؼ��� ����� ���
	virtual int OnDisconnect(MCommObject* pCommObj);

	/// Master�� �����Ѵ�.
	int ConnectToMaster(MCommObject* pCommObj);
	/// Master�� ������ �����Ѵ�.
	void DisconnectToMaster(void);

	/// Master���� UID�Ҵ��� ��û�Ѵ�.
	void RequestUIDToMaster(int nSize);

	void DebugTest();

public:
	/// UID�� ������Ʈ ����
	MObject* GetObject(const MUID& uid);
	/// UID�� ĳ���� ������Ʈ ����
	MObjectCharacter* GetObjectCharacter(const MUID& uid);
	/// UID�� ������ ������Ʈ ����
	MObjectItem* GetObjectItem(const MUID& uid);
	/// CommUID�� ĳ���� ������Ʈ ����
	MObjectCharacter* GetPlayerByCommUID(const MUID& uid);

    /// Object�� ������ ��ȣ�ν� ������Ʈ
	void UpdateObjectRecognition(MObject* pObject);

	/// Command�� Object�� Listener���� ����
	void RouteToListener(MObject* pObject, MCommand* pCommand);
	/// Command�� ��ü Ŭ���̾�Ʈ�� ����
	void RouteToAllClient(MCommand* pCommand);
	/// Command�� Sender�� ������ ��ü Ŭ���̾�Ʈ�� ����
	void RouteToAllExcludeSender(MUID& uidSender, MCommand* pCommand);
	/// Command�� �������� ����
	void RouteToRegion(int nMapID, const rvector& Pos, float fRange, MCommand* pCommand);

	int SpawnObject(int nSpawnID, int nObjID, int nMapID, int nNodeID, rvector& Pos);
	
};

#define DEFAULT_REQUEST_UID_SIZE	10000	///< UID �Ҵ� ��û �⺻ ����


/// �� �Ѱ��� ����
struct MMAPDESCRIPTION{
	char	szName[MMAP_NAME_LENGTH];
};

/// �� ����Ʈ
struct MMAPLIST{
	int					nMapCount;	///< �� ����
	MMAPDESCRIPTION*	pMaps;		///< ��
};

/// Local Clock�� Global Clock���� ��ȯ
unsigned long int ConvertLocalClockToGlobalClock(unsigned long int nLocalClock, unsigned long int nLocalClockDistance);
/// Global Clock�� Local Clock���� ��ȯ
unsigned long int ConvertGlobalClockToLocalClock(unsigned long int nGlobalClock, unsigned long int nLocalClockDistance);


#endif