#ifndef MMASTER_H
#define MMASTER_H

#include "MCommandCommunicator.h"
#include "MServer.h"
#include "MUID.h"

/// ��� ������ ����/�����ϴ� ������ ��Ʈ�ѷ�
/// - UID�� �߱�/�����Ѵ�.
class MMaster : public MServer{
private:
	static MMaster*	m_pInstance;		///< ���� �ν��Ͻ�
protected:
	MUIDRefMap	m_GlobalUIDRefMap;	///< �������� Global UID Reference Map
public:
protected:
	/// ���ο� UID ����
	virtual MUID UseUID(void);

	virtual int OnAccept(MCommObject* pCommObj);
	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	bool OnCommand(MCommand* pCommand);

	/// Master�� ���� ������ Target���� ����
	void ResponseInfo(MUID& Target);
public:
	MMaster(void);
	virtual ~MMaster(void);

	/// ���� �ν��Ͻ� ���
	static MMaster* GetInstance(void);

	bool Create(int nPort);
	void Destroy(void);
};

#define MASTER_UID	MUID(0, 1)	///< �������� ���� UID(�Һ�)


#endif