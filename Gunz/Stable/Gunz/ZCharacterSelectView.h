#ifndef _ZCHARACTERSELECTVIEW_H
#define _ZCHARACTERSELECTVIEW_H

#include "ZMeshView.h"
#include "MUID.h"
#include "MMatchTransDataType.h"
#include "RCharCloth.h"
#include "ZShadow.h"

class ZInterfaceBackground;

// ���⼭ ���̴� index���� ĳ���� ������ index

struct ZSelectCharacterInfo
{
	MTD_CharInfo			m_CharInfo;
	MTD_AccountCharInfo		m_AccountCharInfo;
	bool					m_bLoaded;				// �ش� ������ ĳ���� ������ �޾ƿԴ��� ����
	bool					m_bRequested;			// �ش� ������ ĳ���� ������ ��û�ߴ��� ����
};


/// ĳ���� ������ �� �ʿ��� �͵��� ��� ���� Ŭ����. 
/// ĳ���� ������ �� Ŭ������ ������ �ְ�, �׸��� �͵� �� Ŭ������ ��
class ZCharacterSelectView
{
public:
	enum ZSelectViewState
	{
		ZCSVS_SELECT = 0,
		ZCSVS_CREATE
	};
private:
	static int				m_nNumOfCharacter;			// ������ �ִ� ĳ���� ��
	static int				m_nSelectedCharacter;		// ������ ĳ���� �ε���
	static char				m_szLastChar[MATCHOBJECT_NAME_LENGTH];

	bool					m_bReserveSelectChar;
	int						m_nSelCharIndex;
	unsigned long int		m_nReservedSelectTime;
	RVisualMesh*			m_pVisualMesh;
	RMesh*					m_pMesh;		// �ӽø޽�
	ZShadow*				m_pShadow;
	ZInterfaceBackground*	m_pBackground;
	ZSelectViewState		m_nState;
	float					m_fCRot;

	void DrawCharacterLight(rvector& vCharPos);
	static void ClearInterfaces();
public:
	static ZSelectCharacterInfo		m_CharInfo[MAX_CHAR_COUNT];		// �� ĳ���� ����
	static void ClearCharInfo();
	static void OnReceivedAccountCharInfo(void* pCharListBlob);
	static void OnReceivedCharInfo(int nCharNum, void* pCharInfoBlob);
public:
	ZCharacterSelectView();
	~ZCharacterSelectView();
	void Draw(void);
	void SetBackground(ZInterfaceBackground* pBackground) { m_pBackground = pBackground; }
	bool SelectMyCharacter();
	void SelectChar(int nSelectIndex);
	void SetState(ZSelectViewState nState);
	void OnChangedCharCostume();
	void OnChangedCharInfo(int sex,int index);
	bool IsEmpty(int nIndex);
	void OnInvalidate();
	void OnRestore();
	void UpdateInterface(int nSelIndex);
	inline void CharacterLeft(float add_degree=1.0f);
	inline void CharacterRight(float add_degree=1.0f);
	static int GetNumOfCharacter() { return m_nNumOfCharacter; }
	static int GetSelectedCharacter() { return m_nSelectedCharacter; }
	static void SetSelectedCharacter(int nCharIndex) { m_nSelectedCharacter = nCharIndex; }
	static void SetSelectedCharacterFromLastChar();

	static void LoadLastChar();
	static void SetLastChar(char* szName);
	static const char* GetLastChar() { return m_szLastChar; }
};





// inline func /////////////////////////////////////////////////////////////////////////////
inline void ZCharacterSelectView::CharacterLeft(float add_degree)
{
	m_fCRot += add_degree;
}

inline void ZCharacterSelectView::CharacterRight(float add_degree)
{
	m_fCRot -= add_degree;
}





#endif