#ifndef _ZTASK_H
#define _ZTASK_H

#include <list>
#include <queue>
using namespace std;

#include "ZTaskID.h"

/// task ���
enum ZTaskResult
{
	ZTR_RUNNING,
	ZTR_COMPLETED,
	ZTR_CANCELED
};

class ZBrain;

/// Task�� �߻�Ŭ���� - Task�� command ������ ����Ѵ�.
class ZTask
{
protected:
	ZActor*				m_pParent;
	queue<int>			m_Params;
	bool				m_bCancelEnable;


	virtual void OnStart() = 0;				///< �½�ũ ���۽�
	virtual ZTaskResult OnRun(float fDelta) = 0;		///< �½�ũ ����
	virtual void OnComplete() = 0;			///< �׽�ũ �Ϸ��
	virtual bool OnCancel() = 0;			///< �½�ũ ��ҽ�
public:
	ZTask(ZActor* pParent) : m_pParent(pParent), m_bCancelEnable(true) {}
	virtual ~ZTask() {}

	void ClearParams();
	void PushParam(int nValue);
	void PushParam(float fValue);
	float PopParamFloat();
	int PopParamInt();

	void Start();
	ZTaskResult Run(float fDelta);
	void Complete();
	bool Cancel();
	ZActor* GetParent() { return m_pParent; }
	void SetCancelEnable(bool bEnable) { m_bCancelEnable = bEnable; }

	virtual ZTASK_ID GetID() = 0;
	virtual const char* GetTaskName() = 0;
};


#define DECLARE_TASK_ID(_ID)	ZTASK_ID GetID() { return _ID; }





#endif