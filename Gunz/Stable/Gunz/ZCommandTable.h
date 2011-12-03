#ifndef _ZCOMMANDTABLE_H
#define _ZCOMMANDTABLE_H

/// Ŭ���̾�Ʈ Ŀ�´������Ͱ� ����ϴ� Ŀ�ǵ�
void ZAddCommandTable(MCommandManager* pCommandManager);


// �ܼ� or �ܼ��� alias ���� ��ɾ� ///////////////////////////////////////////
#define ZC_CON_CLEAR		41000	///< Ŭ���̾�Ʈ �ܼ� ȭ���� �����.
#define ZC_CON_SIZE			41001	///< Ŭ���̾�Ʈ �ܼ� â ũ�⸦ �����Ѵ�.
#define ZC_CON_HIDE			41002
#define ZC_CON_CONNECT		41003
#define ZC_CON_DISCONNECT	41004
#define ZC_CON_SAY			41005

#define ZC_TEST_INFO		42000	// �������˾Ƴ��� ���� �׽�Ʈ ��ɾ�
#define ZC_BEGIN_PROFILE	42001	// �������ϸ��� �����Ѵ�.
#define ZC_END_PROFILE		42002	// �������ϸ��� �����Ѵ�.

// �����ڿ� ///////////////////////////////////////////////////////////////////


// test code //////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	#define ZC_TEST_SETCLIENT1		61000
	#define ZC_TEST_SETCLIENT2		61001
	#define ZC_TEST_SETCLIENT3		61002
	#define ZC_TEST_SETCLIENTALL	61003
#endif

#define ZC_TEST_BIRD1			61004	// ������ �׽�Ʈ





// ���ӿ��� ����ϴ� ��ɾ� ///////////////////////////////////////////////////
#define ZC_CHANGESKIN						51000	///< �������̽� ��Ų�� �����Ѵ�.
#define ZC_REPORT_119						51001	///< 119
#define ZC_MESSAGE							51002	///< �޽���

#define ZC_EVENT_OPTAIN_SPECIAL_WORLDITEM	52001	///< Ư���� ����������� �Ծ��� ��� ó��

// ����Ʈ ����

#define MC_QUEST_NPC_LOCAL_SPAWN			53000	///< Ư���� ����������� �Ծ��� ��� ó��


#endif