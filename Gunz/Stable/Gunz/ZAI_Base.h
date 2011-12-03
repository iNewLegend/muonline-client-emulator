#ifndef _ZAI_BASE_H
#define _ZAI_BASE_H


/// AI �ൿ ��Ʈ
enum AI_BEHAVIOR_STATE {
	AI_BEHAVIOR_NONE = 0,		///< N/A
	AI_BEHAVIOR_IDLE,			///< ������ ����
	AI_BEHAVIOR_PATROL,			///< ����
	AI_BEHAVIOR_ATTACK,			///< ����
	AI_BEHAVIOR_RETREAT,		///< ����
	AI_BEHAVIOR_SCRIPT,			///< ��ũ��Ʈ
	AI_BEHAVIOR_DEAD,			///< ����

	AI_BEHAVIOR_END
};


/// AI �׼� ��Ʈ
enum AI_ACTIVITY_SET {
	AI_ACTIVITY_NONE = 0,
	AI_ACTIVITY_IDLE,
	AI_ACTIVITY_MOVE_DIR,		// �ѹ������� �����̵�
	AI_ACTIVITY_MOVE_POS,		// ��ã��� �̵�
	AI_ACTIVITY_ATTACK,
	AI_ACTIVITY_DAMAGED,
//		AI_ACTIVITY_DEAD,
	AI_ACTIVITY_DELAY,

	AI_ACTIVITY_END
};


/// AI ����
enum COND_SET
{
	AI_COND_NONE = 0,
	
	AI_COND_SCHEDULE_DONE,
	AI_COND_TASK_FAILED,

	AI_COND_END
};


/// FSM�� �ʿ��� �Է�
enum AI_INPUT_SET 
{
	AI_INPUT_NONE		= 0,
	AI_INPUT_PLAYER_ATTACKS,
	AI_INPUT_PLAYER_UPPERCUT,

	AI_INPUT_END
};

#endif