#ifndef _MMATCHFRIENDINFO_H
#define _MMATCHFRIENDINFO_H

//#pragma once


#include <list>
using namespace std;
#include "MMatchGlobal.h"


#define MAX_FRIEND_COUNT	20


// ģ������
struct MMatchFriendNode {
	unsigned long	nFriendCID;
	unsigned short	nFavorite;
	char			szName[MATCHOBJECT_NAME_LENGTH];

	unsigned char	nState;										// ��ġ����(�κ�, ������ ���)
	char			szDescription[MATCH_SIMPLE_DESC_LENGTH];	// ��ġ����(�κ��̸� ���)
};
class MMatchFriendList : public list<MMatchFriendNode*> {};


class MMatchFriendInfo {
private:
	MCriticalSection	m_csFriendListLock;
public:
	MMatchFriendList	m_FriendList;
public:
	MMatchFriendInfo();
	virtual ~MMatchFriendInfo();
	bool Add(unsigned long nFriendCID, unsigned short nFavorite, const char* pszName);
	void Remove(const char* pszName);
	MMatchFriendNode* Find(unsigned long nFriendCID);
	MMatchFriendNode* Find(const char* pszName);
	void UpdateDesc();
};

#pragma pack(1)
struct MFRIENDLISTNODE {
	unsigned char	nState;
	char			szName[MATCHOBJECT_NAME_LENGTH];
	char			szDescription[MATCH_SIMPLE_DESC_LENGTH];
};
#pragma pack()


#endif