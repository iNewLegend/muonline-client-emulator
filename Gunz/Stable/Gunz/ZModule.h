#ifndef _ZMODULE_H
#define _ZMODULE_H

#include <map>
#include "MRTTI.h"

using namespace std;

class ZModuleContainer;

class ZModule {
	friend ZModuleContainer;
protected:
	ZModuleContainer*	m_pContainer;	// �̸���� ����ִ� container

public:
	ZModule()				{}
	virtual ~ZModule()		{}

	void Active(bool bActive = true);	// (��)Ȱ��ȭ ��Ų��

	virtual int GetID() = 0;
	
	virtual bool Update(float fElapsed)	{ return true; }
	virtual void Destroy()	{}

	virtual void InitStatus() {}

protected:
	virtual void OnAdd()		{}
	virtual void OnRemove()		{}
	virtual void OnActivate()	{}
	virtual void OnDeactivate()	{}
};

typedef map <int, ZModule *> ZMODULEMAP;

class ZModuleContainer {
	friend ZModule;

	MDeclareRootRTTI;
	ZMODULEMAP m_Modules;
	ZMODULEMAP m_ActiveModules;

public:
	virtual ~ZModuleContainer();

	// bInitialActive : Ȱ��ȭ �ʱ�ġ
	void AddModule(ZModule *pModule,bool bInitialActive = false);
	void RemoveModule(ZModule *pModule);

	// ��ϵ� ����� nID ����� ã�´�
	ZModule* GetModule(int nID);

	void ActiveModule(int nID, bool bActive = true);	// Ư�� ����� (��)Ȱ��ȭ ��Ų��
	bool IsActiveModule(int nID);

	virtual void UpdateModules(float fElapsed);
	virtual void InitModuleStatus(void);
};

#define DECLARE_ID(_ID)		int GetID() { return _ID; }

// ������ ����
class ZModule_HPAP;
class ZModule_Movable;
class ZModule_Resistance;
class ZModule_ColdDamage;
class ZModule_FireDamage;
class ZModule_PoisonDamage;
class ZModule_LightningDamage;
class ZModule_HealOverTime;
class ZModule_Skills;

#endif	// of _ZOBJECTMODULE_H