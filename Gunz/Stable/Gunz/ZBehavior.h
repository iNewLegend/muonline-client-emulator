#ifndef _ZBEHAVIOR_H
#define _ZBEHAVIOR_H


#include "ZStateMachine.h"

/// AI�� �ϴ� ��
enum ZBEHAVIOR_STATE
{
	ZBEHAVIOR_STATE_IDLE			=0,				///< ������ �ִ´�.
	ZBEHAVIOR_STATE_PATROL,							///< ����
	ZBEHAVIOR_STATE_ATTACK,							///< ����
	ZBEHAVIOR_STATE_RETREAT,						///< ����
	ZBEHAVIOR_STATE_SCRIPT,							///< ��ũ��Ʈ
	ZBEHAVIOR_STATE_END
};

enum ZBEHAVIOR_INPUT 
{
	ZBEHAVIOR_INPUT_NONE = 0,
	ZBEHAVIOR_INPUT_ATTACKED,						///< ���ݴ���

	ZBEHAVIOR_INPUT_END
};

// ���� Ư��
enum ZOFFENSETYPE
{
	ZOFFENSETYPE_MELEE		= 1,					// �ٰŸ� ������
	ZOFFENSETYPE_RANGE		= 2,					// ���Ÿ� ������
};


class ZBrain;

/// Behavior State �߻� Ŭ����
class ZBehaviorState : public ZState
{
protected:
	ZBrain*		m_pBrain;

	virtual void OnEnter() {}
	virtual void OnExit() {}
	virtual void OnRun(float fDelta) {}
public:
	ZBehaviorState(ZBrain* pBrain, int nStateID) : ZState(nStateID), m_pBrain(pBrain) { }
	virtual ~ZBehaviorState() { }
	void Run(float fDelta)	{ OnRun(fDelta); }
	void Enter()			{ OnEnter(); }
	void Exit()				{ OnExit(); }
};



class ZBehavior
{
private:
	ZStateMachine		m_FSM;
	ZBehaviorState*		m_pCurrState;
	ZBrain*				m_pBrain;
	ZOFFENSETYPE		m_nOffecseType;
	bool				m_bFriendly;
	void ChangeBehavior(ZBEHAVIOR_STATE nState);

public:
	ZBehavior();
	virtual ~ZBehavior();
	void Init(ZBrain* pBrain);
	void Run(float fDelta);
	bool Input(ZBEHAVIOR_INPUT nInput);
	void ForceState(ZBEHAVIOR_STATE nState);
	ZOFFENSETYPE GetOffenseType()						{ return m_nOffecseType;	}
	void SetOffenseType( ZOFFENSETYPE nType)			{ m_nOffecseType = nType;	}
	bool IsFriendly()									{ return m_bFriendly;		}
	void SetFriendly( bool bFriendly)					{ m_bFriendly = bFriendly;	}
};




#endif