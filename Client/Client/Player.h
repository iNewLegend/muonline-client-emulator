#pragma once
#include "3DMapObj.h"

class CRole : public C3DMapObj
{
public:
	CRole();
	~CRole();
public:
	enum ActionStateType { STAND,STARTWALK, WALK, ENDWALK, STARTRUN, RUN, ENDRUN, JUMP, HIT1, HIT2, HIT3, HIT4, SKILL, SPELL, DEATH };
	virtual int  GetObjType(){return MAP_ROLE;}
	virtual void OnFrameMove(float fElapsedTime);
	void setClass(int nID);
	void setSkeleton();
	void setEquip(const char* szPart, int nEquipID);
	void setHelm(int nID);
	void setArmor(int nID);
	void setGlove(int nID);
	void setPant(int nID);
	void setBoot(int nID);
	void setDir(unsigned char uDir);
	void setName(const wchar_t* wcsName);
	const wchar_t* getName();
	void setLevel(int nLevel);
	void drawName()const;
	virtual void render(int flag)const;
	std::vector<unsigned char>& getPath(){return m_setPath;}
	void walk(unsigned char uDir);
	void setCellPos(int x, int y);
private:
	std::wstring m_wstrName;
	int m_nLevel;
	int m_nClass;
	float m_fHeadRotate;
	float m_fCurrentHeadRotate;

	// Action
	std::vector<unsigned char> m_setPath;
	unsigned char m_uActionState;		//当前运动状态
	float m_fWalkLength;
	unsigned char m_uDir;
	float m_fOneWalkLength;

};