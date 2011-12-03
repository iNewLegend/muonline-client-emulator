#ifndef MERRORTABLE_H
#define MERRORTABLE_H

#define MOK				0	///< ���� ����

#define MERR_UNKNOWN	-1	///< ���� ���ϴ� ����

#define MERR_COMMUNICATOR_HAS_NOT_UID						100		///< Ŀ�´������Ͱ� UID�� ������ ���� ���, ���� �����ͳ� Ȥ�� UID�� �߱޹��� �� �ִ� Ŀ�´������Ϳ� �������� �ʾ��� ����.
#define MERR_COMMUNICATOR_HAS_NOT_UID_SPACE					101		///< Ŀ�´������Ͱ� �߱��� UID ������ ���� �ʴ�. UID�� �������� ���ɼ��� ����.
#define MERR_COMMUNICATOR_INVALID_DIRECT_CONNECTION_POINTER	110		///< �ϳ��� ���μ������� ���� Ŀ�´������Ͱ� ���ÿ� ������, DirectConnectionPointer�� ��ȿ���� ���� ���
#define MERR_COMMAND_HAS_INVALID_RECEIVER_UID				200		///< Ŀ�ǵ忡 ������ Receiver UID�� ��ȿ���� ������

#define MERR_ZONESERVER_NOT_CONNECTED_TO_MASTER				300		///< Zone-Server�� Master-Controller�� ������� �ʾ�����
#define MERR_ZONESERVER_TRY_TO_INVALID_MASTER				301		///< Zone-Server�� Master-Controller�� �ƴ� Ŀ�´������Ϳ��� Master-Controller�� �����ϰ� ������ �õ��Ҷ�

#define MERR_OBJECT_INVALID									1000	///< ��ȿ���� ���� ������Ʈ
//#define MERR_OBJECT_INVALID_MODE							1001	///< �������� ���� ���

#define MERR_MAP_CANNOT_OPEN_FILE							2000	///< ���� ���� ���ϴ� ���


// ����, �α��� ����
#define MERR_CLIENT_WRONG_PASSWORD							10000	///< �н����尡 �߸���
#define MERR_CLIENT_CONNECTED_ALREADY						10001	///< �̹� �������̴�.
#define MERR_COMMAND_INVALID_VERSION						10002	///< ������ Ŭ���̾�Ʈ�� ������ �ٸ���.
#define MERR_CLIENT_FULL_PLAYERS							10003	///< ���� �ο��� ��á��
#define MERR_CLIENT_MMUG_BLOCKED							10004	///< �����ߴ�.
#define MERR_FAILED_AUTHENTICATION							10005	///< ����� ������ ������ �߻��Ͽ����ϴ�.
#define MERR_MULTIPLE_LOGIN									10006	///< �ٸ� PC���� ���� ID�� �����Ͽ��� ������ ������ ������ �����մϴ�.
#define MERR_FAILED_LOGIN_RETRY								10007	///< �α��� �������� ������ �����. ��� ��, �ٽ� �õ����ּ���.
#define MERR_FAILED_GETACCOUNTINFO							10008	///< ���� ������ ������ �� ��� �α����� �� �� ����.
#define MERR_FAILED_MD5_NOT_MATCH							10009   ///< MD5�� �߸��Ǿ����ϴ�.
#define MERR_FAILED_TOO_FASTCONNECTION						10010   ///< �ʹ� ���� ���� �õ��Դϴ�.
#define MERR_FAILED_BLOCK_IP								10011   ///< ���� IP�Դϴ� - Country IP Block
#define MERR_FAILED_NHNUSA_INVALID_AUTHSTR					10012	///< [NHNUSA] ����Ű�� ��ȿ���� �ʴ�.(����Ű �ð� ������ ��찡 ����. Ŭ�� ������ϵ��� �ȳ�)
#define MERR_FAILED_NETMARBLE_KOR_CANT_GET_ACCOUNTINFO		10013	///< �ݸ��� �α��� ������ �������µ� �����߽��ϴ�. ������ ���ӵǸ� ���ǹٶ��ϴ�.

// ĳ���� ���� ����
#define MERR_CLIENT_EXIST_CHARNAME							10100	///< ������ �̹� ĳ���� ���̵� ����
#define MERR_WRONG_WORD_NAME								10101	///< ĳ�����̸��� ����� �� ���� ���ڰ� �ִ�.
#define MERR_TOO_SHORT_NAME									10102	///< ĳ�����̸� ���̰� �ʹ� ª��.
#define MERR_TOO_LONG_NAME									10103	///< ĳ���� �̸��� �ʹ� ���
#define MERR_PLZ_INPUT_CHARNAME								10104	///< ĳ���� �̸��� �Է����ּ���
#define MERR_CHAR_NOT_EXIST									10110	///< �������� �ʴ� ĳ�����̸�
#define MERR_CANNOT_DELETE_CHAR								10111	///< ĳ���͸� ������ �� �����ϴ�.

// ������ ����
#define MERR_TOO_EXPENSIVE_BOUNTY							20001	///< �ٿ�Ƽ�� �����ϴ�
#define MERR_CANNOT_BUY_ITEM								20002	///< �������� ��� ����.
#define MERR_CANNOT_EQUIP_ITEM								20003	///< �������� ����� �� ����.
#define MERR_CANNOT_TAKEOFF_ITEM							20004	///< ���� ������ ������ �� ����
#define MERR_CANNOT_SELL_ITEM								20005	///< �������� �� �� ����.
#define MERR_CANNOT_SELL_EQUIPED_ITEM						20006	///< ����� �������� �� �� ����.
#define MERR_CANNOT_SELL_NONE_ITEM							20007	///< �ش� �������� ����.
#define MERR_TOO_MANY_ITEM									20008	///< ���� �ִ� �������� �ʹ� ����
#define MERR_NO_SELITEM										20009	///< ������ �������� ����
#define MERR_TOO_HEAVY										20010	///< �������� ���԰� �ʹ� ���̴�
#define MERR_LOW_LEVEL										20011	///< ������ �ʹ� ����
#define MERR_CANNOT_EQUIP_EQUAL_ITEM						20012	///< �罽�Կ� ���� ������ ����Ұ�
#define MERR_CANNOT_TAKEOFF_ITEM_BY_WEIGHT					20013	///< ���Զ����� ���� �� ����
#define MERR_CANNOT_SELL_CASHITEM							20014	///< ĳ���������� �� �� �����ϴ�.
#define MERR_BRING_ACCOUNTITEM_BECAUSEOF_SEX				20015	///< ������ �����ʾ� �������� ������ �� ����.
#define MERR_BRING_CONFIRM_ACCOUNTITEM_BECAUSEOF_LEVEL		20016	///< ������ �����ʾ� �������� ������ �� ����. �׷��� �������ڴ°�?
#define MERR_BRING_BACK_ACCOUNTITEM							20017	///< �߾����࿡ ���� �� �����ϴ�.
#define MERR_BRING_BACK_ACCOUNTITEM_FOR_CASHITEM			20018	///< ĳ�������۸� �߾����࿡ ���� �� �ֽ��ϴ�.
#define MERR_BRING_BACK_ACCOUNTITEM_NOT_SPENDABLE			20019	///< ĳ�������۸� �߾����࿡ ���� �� �ֽ��ϴ�.
#define MERR_TAKEOFF_ITEM_BY_LEVELDOWN						20020	///< �����ٿ����� �������� ���������ϴ�.
#define MERR_CANNOT_CAHNGE_THIS_ITEM						20021	///< �����ٿ����� �������� ���������ϴ�.
#define MERR_CANNOT_SELL_TOO_MANY_ITEMCOUNT					20022	///< �ش� �������� ������ �̻��ϴ�.
#define MERR_CANNOT_SELL_NOT_SPENDABLEITEM					20023	///< �ߺ� �������� �ƴѵ�, ������ �����Ǿ� �ִ�.

#define MERR_CANNOT_JOIN_STAGE								30000	///< ���������� ������ �� ����.
#define MERR_CANNOT_JOIN_STAGE_BY_MAXPLAYERS				30001	///< �������� �ο��� �� á��.
#define MERR_CANNOT_JOIN_STAGE_BY_PASSWORD					30002	///< ��й�ȣ�� Ʋ�Ƚ��ϴ�.
#define MERR_CANNOT_JOIN_STAGE_BY_LEVEL						30003	///< ������ ���� �ʽ��ϴ�.
#define MERR_CANNOT_JOIN_STAGE_BY_FORCEDENTRY				30004	///< ������ �� �����ϴ�.
#define MERR_CANNOT_JOIN_STAGE_BY_BAN						30005	///< ������ �����Ǿ����ϴ�.
#define MERR_STAGE_NOT_EXIST								30006	///< �������� �ʴ� ���������Դϴ�
#define MERR_STAGE_ALREADY_EXIST							30007	///< ���������� �̹� �����մϴ�
#define MERR_CANNOT_CREATE_STAGE							30008	///< ���� ���� �� �����ϴ�.
#define MERR_CANNOT_NO_STAGE								30009	///< ���� �����ϴ�.

#define MERR_CANNOT_JOIN_CHANNEL							30020	///< ä�ο� ������ �� ����
#define MERR_CANNOT_JOIN_CHANNEL_BY_MAXPLAYERS				30021	///< ä�� �ο��� �� á��
#define MERR_CANNOT_JOIN_CHANNEL_BY_LEVEL					30022	///< ������ ���� �ʴ�
#define MERR_CANNOT_JOIN_CHANNEL_BY_NEWBIE					30023	///< ������ �ִ� ĳ������ ������ ���� ���̻� �Թ�ä�ο��� �� �� �����ϴ�.

// ����� ����.
#define MERR_CANNOT_JOIN_NEED_TICKET						30024	///< ������� ���� �� �� �����ϴ�.
#define MERR_CANNOT_START_NEED_TICKET						30025	///< ������� ���� ������ �־� ������ ������ �� �����ϴ�.
#define MERR_NO_TARGET										30026	///< ����� �����ϴ�.

// Ŭ������
#define MERR_CLAN_CANNOT_CREATE								30030	///< Ŭ���� ���� �� �����ϴ�.
#define MERR_CLAN_CANNOT_CLOSE								30031	///< Ŭ���� ����� �� �����ϴ�.
#define MERR_EXIST_CLAN										30032	///< Ŭ���� �̹� �����մϴ�.
#define MERR_CLAN_NO_SPONSOR								30033	///< Ŭ����������� �����մϴ�.
#define MERR_CLAN_SPONSOR_JOINED_OTHERCLAN					30034	///< Ŭ����������� �̹� �ٸ� Ŭ���� ���ԵǾ� �ֽ��ϴ�.
#define MERR_CLAN_SPONSOR_NOT_LOBBY							30035	///< Ŭ����������� �κ� �����ϴ�.
#define MERR_CLAN_WRONG_CLANNAME							30036	///< Ŭ���̸��� �߸��Ǿ����ϴ�.
#define MERR_CLAN_NOT_MASTER								30037	///< Ŭ�������Ͱ� �ƴմϴ�.
#define MERR_CLAN_NOT_MASTER_OR_ADMIN						30038	///< Ŭ�������ͳ� ��ڰ� �ƴմϴ�.
#define MERR_CLAN_JOINER_JOINED_ALREADY						30039	///< �����ڰ� �̹� �ٸ� Ŭ���� ���ԵǾ� �ֽ��ϴ�.
#define MERR_CLAN_DONT_JOINED								30040	///< ����ó������ �ʾҽ��ϴ�.
#define MERR_CLAN_JOINED_ALREADY							30041	///< �̹� Ŭ���� ���ԵǾ� �ֽ��ϴ�.
#define MERR_CLAN_JOINER_NOT_LOBBY							30042	///< �����ڰ� �κ� �����ϴ�.
#define MERR_CLAN_NOT_JOINED								30043	///< Ŭ���� ���ԵǾ� ���� �ʽ��ϴ�.
#define MERR_CLAN_CANNOT_LEAVE								30044	///< Ŭ������ Ż���� �� �����ϴ�.
#define MERR_CLAN_MEMBER_FULL								30045	///< ���̻� Ŭ������ �߰��� �� �����ϴ�.
#define MERR_CLAN_OFFLINE_TARGET							30046	///< ����� ���������� �ʽ��ϴ�.
#define MERR_CLAN_OTHER_CLAN								30047	///< ���� Ŭ���� �ƴմϴ�.
#define MERR_CLAN_CANNOT_CHANGE_GRADE						30048	///< ������ ������ �� �����ϴ�.
#define MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER				30049	///< �ش� Ŭ������ �����ϴ�.
#define MERR_CLAN_CREATING_LESS_BOUNTY						30050	///< Ŭ���� �����ϴµ� �ʿ��� �ٿ�Ƽ�� �����մϴ�.
#define MERR_CLAN_CREATING_LESS_LEVEL						30051	///< 10���� �̻� Ŭ���� ������ �� �ֽ��ϴ�.
#define MERR_CLAN_ANNOUNCE_DELETE							30052	///< Ŭ�� ����û ������ �˷���.

// ���ǰ���

// ��������
#define MERR_LADDER_CANNOT_CHALLENGE						30070	///< Ŭ������ ��û�� �� �����ϴ�.
#define MERR_LADDER_WRONG_TEAM_MEMBER						30071	///< �� ����� �ٸ��ϴ�.
#define MERR_LADDER_NO_TEAM_MEMBER							30072	///< �� ����� �����ϴ�.
#define MERR_LADDER_EXIST_CANNOT_CHALLENGE_MEMBER			30073	///< �����ӿ� ��û�� �� ���� ����� �ֽ��ϴ�.
#define MERR_LADDER_NOT_SERVICE_TIME						30074	///< ���� Ŭ���� ���� �ð��� �ƴմϴ�. ���� 10�ÿ� ���µ˴ϴ�.

// Ŭ��������
#define MERR_CB_WRONG_TEAM_MEMBER							30080	///< ���� Ŭ������ �ƴմϴ�.
#define MERR_CB_NO_TEAM_MEMBER								30081	///< ����� �����ϴ�.
#define MERR_CB_EXIST_CANNOT_CHALLENGE_MEMBER				30082	///< Ŭ������ ��û�� �� ���� ����� �ֽ��ϴ�.


// Ŭ���̾�Ʈ
#define MERR_NOT_SUPPORT									30103	///< �������� �ʽ��ϴ�.
#define MERR_CANNOT_ABUSE									30104	///< �ùٸ� ǥ���� �ƴմϴ�.
#define MERR_CANNOT_INPUT_SAME_CHAT_MSG						30105	///< ���� ��ȭ ���� 3���̻� �����ؼ� �� �� ����
#define MERR_CHAT_PENALTY_FOR_ONE_MINUTE					30106	///< 1�а� ä���� �����մϴ�.

#define MERR_CLIENT_CONNECT_FAILED							90000	///< Server�� ��������Ҷ�
#define MERR_CLIENT_DISCONNECTED							90002	///< Server�� ������ ����������



// �޼��� �����
#define MRESULT_CLAN_JOINED									100000	///< Ŭ���� ���ԵǾ����ϴ�.
#define MRESULT_CLAN_CREATED								100001	///< Ŭ���� �����Ǿ����ϴ�.

// ���󰡱� ����
#define MERR_CANNOT_FOLLOW									110000	///< ���� �����ϴ�.
#define MERR_CANNOT_FOLLOW_BY_PASSWORD						110001	///< ��й�ȣ�� �ʿ��� ���̶� ���� ���� ����.
// #define MERR_CANNOT_FOLLOW_BY_CHANNER						110002	///< �ٸ� ä�ο� �־� ���� ���� ����.

// ��ǥ����.
#define MERR_CANNOT_CALL_VOTE								120000 // ��ǥ�� ��û�Ҽ� �����ϴ�.


// Disconnect message
#define MERR_FIND_HACKER									130001	/// ��ŷ ����.
#define MERR_BLOCK_HACKER									130002	/// ��ŷ �����ڷ� ��.
#define MERR_BLOCK_BADUSER									130003	/// �ҷ� ������ ��.
#define MERR_FIND_INVALIDFILE								130004	/// �߸��� ������ ���� �Ǿ����ϴ�.
#define MERR_FIND_FLOODING									130005	/// �÷��� ���� ����.
#define MERR_BLOCK_BY_ADMIN									130006	/// ��ڿ����� ��. 1�ð� ��
#define MERR_BLOCK_SLEEP_ACCOUNT							130007	/// �޸� �������� �з��� ȸ���� ���, ���� ���̴�.
#define MERR_CLEANING_CLIENT								130008	/// ������ �ð� �̻�, �ƹ��� ������ ������ ���� ��..


// Duel Tournament ����.. Added By ȫ����(2009-09-25)
#define MERR_DT_WRONG_CHANNEL								140000	///< ��� ��ʸ�Ʈ ä���� �ƴ϶� ��û ����!
#define MERR_DT_ALREADY_JOIN								140001	///< �̹� ��� ��ʸ�Ʈ ���� ��û�� �߽��ϴ�.
#define MERR_DT_CANNOT_CHALLENGE							140002	///< ���� ��û�� �����߽��ϴ�.
#define MERR_DT_CHALLENGE_TIMEOUT							140003	///< ���� ��û ���� �ð��� �ʰ��Ǿ����ϴ�.
#define MERR_DT_CANNOT_MAKE_STAGE							140004	///< Stage�� �������� ���߽��ϴ�.
#define MERR_DT_CANNOT_FIND_STAGE							140005	///< Stage�� ã�� ���߽��ϴ�.
#define MERR_DT_CANNOT_WRONG_STAGE_SETTING					140006	///< Stage ������ �̻��մϴ�.
#define MERR_DT_CANNOT_START_STAGE							140007	///< Stage�� ������ �� �����ϴ�.
#define MERR_DT_CANNOT_ACCESS_DB							140008	///< DB���� ����� ��Ȱ���� �ʽ��ϴ�.
#define MERR_DT_NOT_SERVICE_TIME							140009	///< ���� �����ʸ�Ʈ ���� �ð��� �ƴմϴ�.(���½ð� %d��00�� ~ %d��59��)

// Admin ���� ��ɾ�.. Added By ȫ����(2010-08-11)
#define MERR_ADMIN_NO_TARGET								150000	///< ó���� ����� �����ϴ�.
#define MERR_ADNIN_CANNOT_PENALTY_ON_DB						150001	///< DB���� ����� ��Ȱ���� �ʽ��ϴ�.

// ���� ���� �޽���
#define MERR_CANNOT_VOTE									200000 // ��ǥ �Ұ���.
#define MERR_CANNOT_VOTE_LADERGAME							200001 // ���������� ��ǥ�� ������ �ʽ��ϴ�.
#define MERR_VOTE_ALREADY_START								200002 // �̹� ��ǥ�� �������Դϴ�.
#define MERR_VOTE_FAILED									200003 // ��ǥ�� �����߽��ϴ�.
#define MERR_TIME_10REMAINING								200004 // �ð����� 10�� ���ҽ��ϴ�.
#define MERR_TIME_30REMAINING								200005 // �ð����� 30�� ���ҽ��ϴ�.
#define MERR_TIME_60REMAINING								200006 // �ð����� 60�� ���ҽ��ϴ�.
#define MERR_PERSONNEL_TOO_MUCH								200007 // �ο��� �ʹ� �����ϴ�.
#define MERR_HE_IS_NOT_READY								200008 //  '%s'���� �غ� �ȵǾ����ϴ�.


// ���� ���� �޽���
#define MERR_CANNOT_APPLY_BUFF_INFO							300000 // ������ ������ �� �����ϴ�.

// MC_MATCH_ANNOUNCE���� ���ڿ� ���ڿ� ���� ���ڿ��� �ִ´�� ����ID�� �־���� ���� ���ξ�
#define MTOK_ANNOUNCE_PARAMSTR "ERRMSG:"

#endif