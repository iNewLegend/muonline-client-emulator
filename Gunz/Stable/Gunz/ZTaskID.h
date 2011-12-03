#ifndef _ZTASKID_H
#define _ZTASKID_H

/// AI�� �ϴ� ��
enum ZTASK_ID
{
	ZTID_NONE = 0,
	ZTID_MOVE_TO_TARGET,			///< ���� ������� �̵�
	ZTID_MOVE_TO_POS,				///< ���� ��ġ�� �̵�
	ZTID_MOVE_TO_DIR,				///< ���� �������� �̵�(��ã����Ѵ�.)
	ZTID_ROTATE_TO_DIR,				///< ���� �������� ȸ��
	ZTID_ATTACK_MELEE,				///< ���� ����
	ZTID_ATTACK_RANGE,				///< ���� ����
	ZTID_DELAY,						///< ���
	ZTID_SKILL,						///< ���� �� ���
	ZTID_END
};


#endif