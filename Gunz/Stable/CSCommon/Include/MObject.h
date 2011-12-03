#ifndef MOBJECT_H
#define MOBJECT_H

#include "MUID.h"
//#include "RTypes.h"
#include "MGridMap.h"
#include "MObjectTypes.h"

using namespace std;
#include <algorithm>

class MMap;
class MObject;

typedef MGridMap<MObject*>	MObjectGridMap;


/// ������ �̿��� RTTI ����. Class Declaration
#define DECLARE_RTTI()								public: static const char* _m_RTTI_szClassName; virtual const char* GetClassName(void){ return _m_RTTI_szClassName; }
/// ������ �̿��� RTTI ����. Class Implementation
#define IMPLEMENT_RTTI(_Class)						const char* _Class::_m_RTTI_szClassName = #_Class;
/// ���� Ŭ����Ÿ���ΰ�?
#define ISEQUALCLASS(_Class, _InstancePointer)		(_Class::_m_RTTI_szClassName==(_InstancePointer)->GetClassName())	// ������ ��

//#define SAFECAST(_Class, _InstancePointer)			_InstancePointer-
/*
/// �̵��� ���� ����
struct MOBJECTPATH{
	unsigned long int	nTime;		///< �ش� �ð�
	rvector				Pos;		///< �ش� ��ġ
	int					nPathNode;	///< Path Node ID
};
#define MOBJECTMOVE	MOBJECTPATH	// �ӽ�, ���߿� �̸��� �� �ٲ��ش�.
*/
class MObject;
enum OBJECTCACHESTATE {	OBJECTCACHESTATE_KEEP, OBJECTCACHESTATE_NEW, OBJECTCACHESTATE_EXPIRE };
class MObjectCacheNode {
public:
	MObject*			m_pObject;
	OBJECTCACHESTATE	m_CacheState;
};
class MObjectCache : public list<MObjectCacheNode*> {
	int		m_nUpdateCount;	// New, Expire�� ī��Ʈ
public:
	int GetUpdateCount() { return m_nUpdateCount; }
	MObjectCacheNode* FindCacheNode(MObject* pObj);
	void Invalidate();
	void Update(MObject* pObject);
	void RemoveExpired();
};


/// �������� �����ϴ� ��� ������Ʈ�� Abstract Class
class MObject{
protected:
	MUID			m_UID;

	MObjectType		m_ObjectType;		///< �켱 �̰��� ������ PC, NPC�� �Ǻ��Ѵ�.
private:
	//rvector			m_Pos;		///< ��ġ
	//rvector				m_StartPos, m_EndPos, m_Pos;
	//unsigned long int	m_nStartTime, m_nEndTime;

//	vector<MOBJECTPATH>	m_Path;		///< �н�
//	rvector				m_Pos;		///< ���� ��ġ
//	int					m_nSpawnID;

//protected:
//	MMap*					m_pMap;		///< ���� �����ִ� ��
//	MObjectGridMap::HREF	m_hRef;		///< �ʿ����� ��ġ �ڵ�


public:
	MObjectCache	m_ObjectCache;
	list<MUID>		m_CommListener;

	/*
protected:
	void UpdateObjectCache(void);
	*/

protected:
	/// �ð��� ���� ��ġ ����
//	rvector GetPos(unsigned long int nTime);

public:
	MObject();
//	MObject(MUID& uid, MMap* pMap, rvector& Pos);	// Map Navication Object
	MObject(const MUID& uid);	// No Map Object
	virtual ~MObject(void)	{};

	inline const MUID GetUID(void) const { return m_UID; }

	/// ���� ��ġ ����
//	void SetPos(rvector& Pos){ SetPos(0, 0, Pos, Pos); }
	/// �ð��� ���� ��ġ ����
//	void SetPos(unsigned long int nStartTime, unsigned long int nEndTime, rvector& StartPos, rvector& EndPos);
	/// �н��� ����
//	void SetPath(MOBJECTPATH* p, int nCount);
	/// ���� �ð��� ������Ʈ ��ġ ���
//	const rvector GetPos(void){ return m_Pos; }
	/// SpawnID ��� (Spawn Object �ƴϸ� 0)
//	int GetSpawnID() { return m_nSpawnID; }
//	void SetSpawnID(int nSpawnID) { m_nSpawnID = nSpawnID; }

	void SetObjectType(MObjectType type) { m_ObjectType = type; }
	MObjectType GetObjectType()	{ return m_ObjectType; }

	virtual bool CheckDestroy(int nTime) { return false; }

	/// ƽ ó��
	virtual void Tick(unsigned long int nTime)	{}

	/// ���� �� ����
//	void SetMap(MMap* pMap){ m_pMap = pMap; }
	/// ���� �����ִ� �� ���
//	MMap* GetMap(void){ return m_pMap; }
	

	void AddCommListener(MUID ListenerUID);
	void RemoveCommListener(MUID ListenerUID);
	bool IsCommListener(MUID ListenerUID);
	bool HasCommListener() { if (m_CommListener.size() > 0) return true; else return false; }
	const MUID GetCommListener() { 
		if (HasCommListener())
			return *m_CommListener.begin(); 
		else
			return MUID(0,0);
	}

	void InvalidateObjectCache() { m_ObjectCache.Invalidate(); } 
	void UpdateObjectCache(MObject* pObject) { m_ObjectCache.Update(pObject); }
	void ExpireObjectCache(MObject* pObject) { 
		MObjectCacheNode* pNode = m_ObjectCache.FindCacheNode(pObject);
		if (pNode == NULL) return;
		pNode->m_CacheState = OBJECTCACHESTATE_EXPIRE;
	}
	void RemoveObjectCacheExpired() { m_ObjectCache.RemoveExpired(); }
	int GetObjectCacheUpdateCount() { return m_ObjectCache.GetUpdateCount(); }

	DECLARE_RTTI()
};

/*
/// ���̴� ��� ������Ʈ. �ʿ� ��ġ��ų�� �ִ� ��ġ�� ũ�⸦ ������.
class MObjectVisible : public MObject{
public:
	MObjectVisible(MUID& uid) : MObject(uid) {}
	virtual ~MObjectVisible(void){}

	DECLARE_RTTI()
};
*/


/*
/// ������Ʈ�� �⺻ ����
struct MOBJECTBASICINFO{
	MUID	uid;
	float	x, y, z;
};

/// ������Ʈ�� ĳ�� ����
struct MOBJECTCACHEINFO{
	MUID				uid;
	float				x, y, z;

	MObjectType		nObjectType;		// ������Ʈ Ÿ�� - Character, Item
//	union {								// ������Ʈ Ÿ�Կ� ���� �޶��� �� �ִ�.
//		MCharacterType	nChrType;			// ĳ���� Ÿ�� - Player, NPC
//	};

	OBJECTCACHESTATE	nState;
};

/// ĳ���� �Ӽ� �⺻ ����
struct MCharacterAttrBasicInfo{
	MUID		uid;
	int			nHP;
	int			nEN;
};


/// �ɾ�µ� �ɸ��� �ð� ���(���߿� ĳ���Ϳ� ���� ������ �Ķ���ͷ� ����.)
unsigned long int MGetTimeToWalk(rvector& Distance);
/// ���ϴµ� �ɸ��� �ð� ���
unsigned long int MGetTimeToTurn(rvector& Dir1, rvector& Dir2);
*/
#endif