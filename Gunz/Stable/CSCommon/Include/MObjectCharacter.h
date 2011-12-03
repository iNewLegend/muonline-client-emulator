#ifndef MOBJECTCHARACTER_H
#define MOBJECTCHARACTER_H

#include "MCharacterDesc.h"
#include "MObject.h"
#include "MObjectItem.h"
#include "MFormulaTable.h"

#include <list>
#include <map>
#include <algorithm>
using namespace std;


class MAction;
class MActionPipe;
class MActParam;




/// ĳ���� ������Ʈ
class MObjectCharacter : public MObject{
protected:
	char					m_szName[32];
	MCharacterRace			m_nRace;
	MCharacterClass			m_nClass;
	MCharacterType			m_nType;
	int						m_nLevel;		///< ����
	unsigned long int		m_nEXP;			///< ����ġ
	int						m_nMaxHP;
	int						m_nMaxEN;
	float					m_fHP;
	float					m_fEN;
	MCharacterBasicAttr		m_BasicAttr;
	MCharacterMoveMode		m_nMoveMode;
	MCharacterMode			m_nMode;
	MCharacterState			m_nState;
	MCharacterAbility		m_AbilityFlags;
	MCharacterRepeatInfo	m_RepeatInfo;

	MObjectItem*			m_pEquipmentItems[MES_END];	///< �����ϰ� �ִ� �����ۿ� ���� ������
	MObjectItemMap			m_HasItems;					///< ������ �ִ� �����۵�

	list<MActionPipe*>		m_ActionPipeList;
	list<MAction*>			m_ActionList;
	MActParam*				m_pActParam;

	int						m_nLastHPRegenTime;
	int						m_nLastENRegenTime;
	int						m_nDiedTime;
	int						m_nLastCacheTime;

	int CalcMaxHP();					///< BasicAttr���� ������ �ִ� HP ���
	int CalcMaxEN();					///< BasicAttr���� ������ �ִ� EN ���

	void RegenHP(unsigned long int nTime);		///< HP�� ����
	void RegenEN(unsigned long int nTime);		///< EN�� ����
public:
	MObjectCharacter(MUID& uid, MMap* pMap, rvector& Pos);
	virtual ~MObjectCharacter(void);

	/// ƽ ó��
	virtual void Tick(unsigned long int nTime);
	void LoadInitialData();

	// Item ����
	int ObtainItem(MObjectItem* pObjectItem);		///< �������� ȹ���Ѵ�.
	void ClearItems();
	bool EquipItem(MObjectItem* pObjectItem, MEquipmentSlot TargetSlot);	///< ������ ����
	bool EquipItem(MUID& ItemUID, MEquipmentSlot TargetSlot);				///< ������ ����
	MObjectItem*	FindItem(MUID& uidItem);	///< UID�� ������ �ִ� �������� ã�´�.
	void ClearEquipmentItem();

	// property
	int GetHP() { return (int)m_fHP; }
	int GetEN() { return (int)m_fEN; }
	int GetSTR() { return m_BasicAttr.nSTR; }
	int GetCON() { return m_BasicAttr.nCON; }
	int GetDEX() { return m_BasicAttr.nDEX; }
	int GetAGI() { return m_BasicAttr.nAGI; }
	int GetINT() { return m_BasicAttr.nINT; }
	int GetCHA() { return m_BasicAttr.nCHA; }
	float UpdateHP(float fVal);
	float UpdateEN(float fVal);
	const MCharacterBasicAttr* GetBasicAttr()	{ return &m_BasicAttr; }
	MCharacterType		GetType()				{ return m_nType; }
	MCharacterMoveMode	GetMoveMode()			{ return m_nMoveMode; }
	void SetMoveMode(MCharacterMoveMode nMode);
	MCharacterMode		GetMode()				{ return m_nMode; }
	void SetMode(MCharacterMode nMode);
	MCharacterState		GetState()				{ return m_nState; }
	void SetState(MCharacterState nState);

	MObjectItem*	GetEquipmentItem(MEquipmentSlot nSlot) { return m_pEquipmentItems[nSlot]; }
	void Resurrect();
	virtual bool CheckDestroy(int nTime);
	void CacheUpdate(unsigned long int nTime);


	// Action ����
	void AddAction(MAction* pAction);
	void DelAction(int nActID);
	MAction* FindAction(int nActID);
	void AddActionPipe(MActionPipe* pActionPipe);
	void DelActionPipe(int nActPipeID);
	MActionPipe* FindActionPipe(int nActPipeID);
	int GetActionCount()	{ return (int)m_ActionList.size(); }
	list<MAction*>::iterator GetActionBeginItor()	{ return m_ActionList.begin(); }
	list<MAction*>::iterator GetActionEndItor()		{ return m_ActionList.end(); }
	void UpdateActParam(MActParam* pActParam);
	MActParam* GetActParam()	{ return m_pActParam; }
	void UnloadActions();
	bool InvokeActionPipe(int nPipeID, int nTime, MActParam* pActParam);

	bool Attack(MObjectCharacter* pTarget);
	void StopAttack();
	bool Skill(int nSkillID, MObjectCharacter* pTarget);
	bool Warning(MObjectCharacter* pAttacker);
	bool Damage(int nDamage, const MUID& uidAttacker);

	void RouteRepeatInfo(int nTime);

	DECLARE_RTTI()
};


struct MBLOB_BASICINFO_UPDATE {
	MUID	uid;
	float	fHP;
	float	fEN;
};


#endif