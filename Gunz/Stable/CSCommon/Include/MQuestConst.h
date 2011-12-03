#ifndef _MQUESTCONST_H
#define _MQUESTCONST_H


/// MAX QL
#define MAX_QL			5			

/// �ִ� �ʼ� ����	
#define MAX_MAPSET		6


/// scenario ���� /////////////////////////////////////////////////////////

/// �ó��������� �ʿ�� �ϴ� ��� ������ �ְ� ����
#define MAX_SCENARIO_SACRI_ITEM			2

/// �ó��������� �ִ� Ư�� ������ ���� �ְ� ����
#define MAX_SCENARIO_REWARD_ITEM		3		

/// �ó������� �����ϴ� �ֻ��� ��ü��
#define SCENARIO_STANDARD_DICE_SIDES	6


/// Map ���� //////////////////////////////////////////////////////////////

/// ������ ��ũ�� �ִ� 10������ ���� �� ����
#define MAX_SECTOR_LINK		10		

/// ���������� ����Ÿ�Ժ� 20������ ���� �� �ִ�.
#define MAX_SPAWN_COUNT		20


/// NPC�� ���� Ÿ��
enum MQuestNPCSpawnType
{
	MNST_MELEE = 0,
	MNST_RANGE,
	MNST_BOSS,
	MNST_END
};

/// �Ȱ��� ������������ NPC�� �ѹ� ������Ų �� �ּ��� 2�ʰ� ������ ������ų �� �ִ�.
#define SAME_SPAWN_DELAY_TIME			2000

/// ����Ʈ���� �������� ����������� 10�ʰ� ������ �ڵ������� �Ҹ��Ѵ�.
#define QUEST_DYNAMIC_WORLDITEM_LIFETIME	10000

/// �����۹ڽ� ��������� ID�� 51�̴�. - �����Ϸ��� worlditem.xml�� ID�� �Բ� �����ؾ� �Ѵ�.
#define QUEST_WORLDITEM_ITEMBOX_ID			51

/// NPC ���� //////////////////////////////////////////////////////////////

/// NPC�� ���� �� �ִ� �ִ� ��ų��
#define MAX_SKILL	4



/// Rule ���� /////////////////////////////////////////////////////////////

/// ��Ż�� ������ ���� ��Ż�� �̵��� �� �ִ� �ð�
#define PORTAL_MOVING_TIME				30000		// (30 ��)

/// ���ο� ���� �����ϰ��� NPC�� ó�� ���ö������� ���� �ð�
#define QUEST_COMBAT_PLAY_START_DELAY	7000		// (7 ��)

/// ����Ʈ Complete�ϰ� ���� ������ ���� �� �ִ� �����ð�
#define QUEST_COMPLETE_DELAY	10000				// (10 ��)

/// ����Ʈ�� ���� ����
enum MQuestCombatState
{
	MQUEST_COMBAT_NONE		= 0,
	MQUEST_COMBAT_PREPARE	= 1,	// �����
	MQUEST_COMBAT_PLAY		= 2,	// ���� ����
	MQUEST_COMBAT_COMPLETED	= 3		// �������� - ���������� �̵�
};



enum MQuestItemType
{
	MMQIT_PAGE = 1,
	MMQIT_SKULL,
	MMQIT_FRESH,
	MMQIT_RING,
	MMQIT_NECKLACE,
	MMQIT_DOLL,
	MMQIT_BOOK,
	MMQIT_OBJECT,
	MMQIT_SWORD,
	MMQIT_GUN,
	MMQIT_LETTER,
	MMQIT_ITEMBOX,
	MMQIT_MONBIBLE,
	

	MMQIT_END,
};


#define MIN_QUEST_ITEM_ID		200001	// ����Ʈ ������ ���� ��.

//#define MAX_UNIQUE_ITEM_COUNT	3
//#define START_QUEST_ITEM_COUNT	2
#define MIN_QUEST_DB_ITEM_COUNT	1
#define MAX_QUEST_DB_ITEM_COUNT	100 // �ϳ��� ���������� ������ ������ �ִ� �ִ� ������ ��. (�ִ� ������ �ִ� ���� + MIN_QUEST_ITEM_COUNT)
									// ī��Ʈ: 0�� �ѹ��� ȹ���� ���� �����鼭 ���� ������ 0.
									//		 : 1�� ��� �ѹ��� ȹ���� ���� ������ ���� ������ 0��.
									//		 : 2�� ������ ī��Ʈ�� ���۵Ǵ� ����. 2�� ���������δ� 1���� ��Ÿ��.

// �ϳ��� ������ ���� ĳ���ʹ� 0~99������ ����Ʈ �������� ���� �� �ִ�.
#define MIN_QUEST_ITEM_COUNT	0	
#define MAX_QUEST_ITEM_COUNT	99

//#define REAL_MAX_QUEST_ITEM_COUNT (MAX_QUEST_ITEM_COUNT - MIN_QUEST_ITEM_COUNT)

// zquestitem.xml�� ����Ǵ� ������ ID�� ����.
#define MINID_QITEM_LIMIT		200001	// Qeust item.
#define MAXID_QITEM_LIMIT		200256
#define MINID_MONBIBLE_LIMIT	210001	// Monster bible.
#define MAXID_MONBIBLE_LIMIT	220000

#define	MAX_PLAY_COUNT				0					// �ִ� DB������Ʈ ���� �Ҽ��ִ� ����Ʈ �Ϸ��. ����Ʈ �ϷḦ ������ ������ ������Ʈ �Ѵ�.
#define	MAX_SHOP_TRADE_COUNT		20					// �ִ� DB������Ʈ ���� ������ �ŷ��� �Ҽ� �ִ¼�.
#define	MAX_ELAPSE_TIME				(1000 * 60 * 30)	// �ִ� DB������Ʈ�� ������ų�� �ִ� �ð�(/ms).
#define MAX_REWARD_COUNT			10					// �ִ� DB������Ʈ ���� ��������� �ִ� ��. (�������� ������ ����. ex. �����:2, �Ұ��:3 �ϰ�� +2�� �ȴ�.)
#define MAX_QUSET_PLAYER_COUNT		4

#define MAX_SACRIFICE_SLOT_COUNT 2	// ��� �������� �÷������� �ִ� ������ �ִ� ��.

// Quest�� ���õ� DB ��� ��.
#define MAX_DB_QUEST_ITEM_SIZE		256
#define MAX_DB_MONSTERBIBLE_SIZE	32
#define MAX_DB_QUEST_BINARY_SIZE	292	// �� ���̳ʸ� �ʵ� ������� 292( CRC32 4Byte + MAX_DB_QUEST_ITEM_SIZE + MAX_DB_MONSTERBIBLE_SIZE ).
#define QUEST_DATA					293	// MAX_DB_QUEST_BINARY_SIZE + 1(\0)

// ������ ������� Ŀ�ǵ� ���� ��� ��� ��.
#define ITEM_TYPE_NOT_SACRIFICE			-2		// �������� Ÿ���� Sacrifice�� �ƴ϶� ���Կ� �÷� ������ ����.
#define NEED_MORE_QUEST_ITEM			-4		// �������� ������ ������.
#define SACRIFICE_ITEM_DUPLICATED		-5		// ������ �ߺ�. ������ �÷����ִ� �����۰� ���� �������� ���Կ� �ø��� �Ͽ�����.
#define ERR_SACRIFICE_ITEM_INFO			-255	// ��û�� ��������� ������ ���������� ����.


// ���Կ� �÷����� ������ �˻� ��� ��� ��.
#define MSQITRES_NOR  1	// �Ϲݽó������� ���� ��� �����۸� �ִ� ��Ȳ.
#define MSQITRES_SPC  2	// Ư���ó������� �ش��ϴ� ����������� ����.
#define MSQITRES_INV  3	// �ش� QL������ ��������� ���� ���̺��� ����. �̰��� ���� �ʴ� ��� �������� �÷��� �������.
#define MSQITRES_DUP  4 // ���� ���Կ� ���� �������� �÷��� ����.
#define MSQITRES_EMP  5 // ���� ������ ��� ��� ����. �� ���´� QL����1�� ����� ��.
#define MSQITRES_ERR -1	// ����... ���̺��� �ش� QL�� ã���� ����. QL = 0 or QL���� ���� ������ MAX QL���� Ŭ���.


// ���� ���� ����. 
#define MSMB_UNKNOW		0	// �ش� ���Ϳ� ���� ������ �ƴ°� ����.
#define MSMB_KNOW		1	// �ش� ���Ϳ� ���� ������ �̹� �˰� ����.
#define MSMB_NEWINFO	2	// ���Ӱ� ȹ��. �� ������ ������ ����� ��Ű�� MSMB_KNOW�� �ٲ���� ��.



#endif