#ifndef _ZCHARACTERMANAGER_H
#define _ZCHARACTERMANAGER_H

#include "MUID.h"
#include "RVisualMeshMgr.h"
#include "ZObject.h"
#include "ZCharacter.h"
#include "MObjectTypes.h"

_USING_NAMESPACE_REALSPACE2

class ZCharacterManager : public map<MUID, ZCharacter*>
{
private:
	int		m_nLiveCount;
	//void DrawMyCharacter();
public:
//	int		m_nOnDrawCnt;
//	int		m_nRenderedCnt;

	ZCharacterManager();
	virtual ~ZCharacterManager();
//	void Draw();
	

	void Add(ZCharacter *pCharacter);
	ZCharacter* Add(MUID uid, rvector pos,bool bMyCharacter=false);

	void Delete(MUID uid);
	void Clear();
//	void Update(float fElapsed);

//	ZCharacter* Pick(ZCharacter* pMyChar,rvector& pos,rvector& dir, RPickInfo* pInfo = NULL);
//	ZCharacter* Pick(int x,int y,RPickInfo* pInfo=NULL);
//	ZCharacter* Pick(rvector& pos,rvector& dir, RPickInfo* pInfo = NULL);
	ZCharacter* Find(MUID uid);
//	ZCharacter* Pick( rvector& pos, float Radius );

	/// �ش� �÷��̾��� ������ ���Ѵ�. - �����˻���. �÷��̾ ������ -1 ��ȯ
	/// @param uid					ã�� �÷��̾� UID
	/// @param bIncludeHiddenChar	admin hidden ĳ���� ���Կ���
	int GetCharacterIndex(MUID& uid, bool bIncludeHiddenChar=true);		

	void OutputDebugString_CharacterState();

	int GetLiveCount();
	int GetCount() { return (int)size(); }
	ZCharacter* Get(int index);

	bool ToggleClothSimulation();

	void OnInvalidate();
	void OnRestore();

	void InitRound();
};




#endif 