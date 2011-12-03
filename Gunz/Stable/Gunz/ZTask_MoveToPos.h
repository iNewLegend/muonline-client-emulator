#ifndef _ZTASK_MOVE_TO_POS_H
#define _ZTASK_MOVE_TO_POS_H

#include "ZTask.h"

class ZTask_MoveToPos : public ZTask
{
private:
	// data
	rvector		m_TargetPos;		// ������
	bool		m_bRotated;
	bool				m_bChained;			///< ���� Task�� ����Ǿ� �ִ��� ����.
protected:
	virtual void OnStart();
	virtual ZTaskResult OnRun(float fDelta);
	virtual void OnComplete();
	virtual bool OnCancel();
public:
	DECLARE_TASK_ID(ZTID_MOVE_TO_POS);

	ZTask_MoveToPos(ZActor* pParent, rvector& vTarPos, bool bChained=false);
	virtual ~ZTask_MoveToPos();
	virtual const char* GetTaskName() { return "MoveToPos"; }
};







#endif