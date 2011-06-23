//---------------------------------------------------------------------------
#ifndef GAMESERVER_CMANG_H
#define GAMESERVER_CMANG_H
//---------------------------------------------------------------------------

#include "User.h"
//---------------------------------------------------------------------------

#define MAX_MSG_LEN				500
#define MAX_DATA				200

#define DATA_TYPE_ONCONNECT		0
#define DATA_TYPE_ONLEVELUP		1
#define DATA_TYPE_ONMAPMOVE		2
//---------------------------------------------------------------------------

struct _DATA_ONCONNECT
{
	bool	m_IsData;
	int		m_Level;
	int		m_IsMove;
	int		m_Map;
	int		m_X;
	int		m_Y;
	char	m_Msg[MAX_MSG_LEN];
};
//---------------------------------------------------------------------------

struct _DATA_ONLEVELUP
{
	bool	m_IsData;
	int		m_Level;
	int		m_IsMove;
	int		m_Map;
	int		m_X;
	int		m_Y;
	int		m_BonusPoints;
	char	m_Msg[MAX_MSG_LEN];
};
//---------------------------------------------------------------------------

struct _DATA_ONMAPMOVE
{
	bool	m_IsData;
	int		m_MapID;
	char	m_Msg[MAX_MSG_LEN];
};
//---------------------------------------------------------------------------

class cMang
{
private:
		int					m_DataOnConnectCount;
		int					m_DataOnLevelUpCount;
		int					m_DataOnMapMoveCount;
		// ----
		_DATA_ONCONNECT		m_DataOnConnect[MAX_DATA];
		_DATA_ONLEVELUP		m_DataOnLevelUP[MAX_DATA];
		_DATA_ONMAPMOVE		m_DataOnMapMove[MAX_DATA];
public:
	cMang();
	// ----
	void			Init();
	bool			LoadFile(char * szFileName);
	bool			MsgSend(int aIndex ,int MsgIndex ,char* Message , ...);
	bool			MailAdd(int aIndex,int MsgIndex,char* subject);
	bool			ReadMsgReq(int aIndex,int MsgIndex);
	// ----
	bool			InsertData(int Level, int IsMove, int Map, int X, int Y, char * Msg);
	bool			InsertData(int Level, int IsMove, int Map, int X, int Y, int BonusPoints, char * Msg);
	bool			InsertData(int MapID, char * Msg);
	// ----
	bool			OnConnect(int aIndex);
	bool 			OnLevelUp(int aIndex);
	bool 			OnMoveMap(int aIndex,int MapTarget);
};

//---------------------------------------------------------------------------

extern cMang g_Manger;
//---------------------------------------------------------------------------
#endif /* GAMESERVER_CMANG_H */
//---------------------------------------------------------------------------