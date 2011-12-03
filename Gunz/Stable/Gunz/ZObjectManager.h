#ifndef _ZOBJECTMANAGER_H
#define _ZOBJECTMANAGER_H

#include "ZPrerequisites.h"
#include "MRTTI.h"
#include "MUID.h"
#include "RVisualMeshMgr.h"
#include "ZObject.h"
#include "ZCharacter.h"
#include <map>
#include <list>
using namespace std;

_USING_NAMESPACE_REALSPACE2

class ZActor;

/// NPC ������
class ZNPCObjectMap : public map<MUID, ZObject*>
{
private:
	bool m_bForceInvisibleNewNpc;
public:
	ZNPCObjectMap() : m_bForceInvisibleNewNpc(false) {}

	void Insert(const MUID& uidNPC, ZObject* pObject)
	{
		insert(value_type(uidNPC, pObject));
		if (m_bForceInvisibleNewNpc)
			pObject->SetVisible(false);
	};
	void Delete(const MUID& uidNPC)
	{
		iterator itorNPC = find(uidNPC);
		if (itorNPC != end())
		{
			delete (*itorNPC).second;
			erase(itorNPC);
		}
	};
	ZObject* Find(const MUID& uidNPC)
	{
		iterator itor = find(uidNPC);
		if (itor == end()) return NULL;

		return ((*itor).second);
	};
	void SetVisibleAll(bool b)
	{
		for (iterator it=begin(); it!=end(); ++it)
			it->second->SetVisible(b);
	}
	void ForceInvisibleNewNpc(bool b) { m_bForceInvisibleNewNpc = b; }
};

class ZObjectManager : public map<MUID, ZObject*>
{
private:
	int									m_nOnDrawCnt;
	int									m_nRenderedCnt;
	unsigned long int					m_nGenerate;
	ZNPCObjectMap						m_NPCObjectMap;			// NPC id�� ���⼭ �����Ѵ�.

//	unsigned long int UseID()	{	m_nGenerate++;	return m_nGenerate;	}
	void Insert(ZObject* pObject);
	bool DrawObject(ZObject* pObject);
public:
	ZObjectManager();
	virtual ~ZObjectManager();
	void Add(ZObject *pObject);
	void Delete(ZObject* pObject);
	void Clear();
	void ClearNPC();
	void Update(float fDelta);
	void Draw();

	ZObject* Pick(ZObject* pMyChar,rvector& pos,rvector& dir, RPickInfo* pInfo = NULL);
	ZObject* Pick(int x,int y,RPickInfo* pInfo=NULL);
	ZObject* Pick(rvector& pos,rvector& dir, RPickInfo* pInfo = NULL);
	ZObject* Pick( rvector& pos, float Radius );

	// debug code
	int GetDrawCount()		{ return m_nOnDrawCnt; }
	int GetRenderedCount()	{ return m_nRenderedCnt; }
	ZObject* GetObject(const MUID& uid);
	ZActor* GetNPCObject(const MUID& uidNPC);
	ZNPCObjectMap* GetNPCObjectMap() { return &m_NPCObjectMap; }
};




#endif 