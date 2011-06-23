//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* Role.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "3DMapObj.h" 
#include "..\..\Protocol\prodef.h"
#include "..\..\Protocol\GameProtocol.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PLAYER_BMD_PATCH	"Data\\Player\\player.bmd"
#define ITEM_CVS_PATCH		"Data\\CSV\\Item.csv"
#define EQUIP_CVS_PATCH		"Data\\CSV\\Equip.csv"
// ----
#define DIR_COUNT			8
#define CLASS_TEMP_SIZE		256
#define CONSTLPWCHAR		const wchar_t*
#define STDVECTIORCHAR		std::vector<unsigned char>
#define DIR_LENGTH(a)		sqrt((float)DX[a] * DX[a] + (float)DY[a] * DY[a])
#define DIR_NORMALIZE(a)	Vec3D((float)DX[a], 0.0f,(float)DY[a]).normalize()
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CRole : public C3DMapObj
{
private:
	char			m_szTemp[CLASS_TEMP_SIZE];
protected:
	unsigned long	m_uID;
	// ----
	std::wstring	m_wstrName;
	// ----
	int				m_nLevel;
	int				m_nClass;
	// ----
	float			m_fHeadRotate;
	float			m_fCurrentHeadRotate;
	// ----
	STDVECTIORCHAR	m_setPath;
	// ----
	unsigned char	m_uActionState;
	// ----
	float			m_fWalkLength;
	float			m_fOneWalkLength;
	// ----
	unsigned char	m_uDir;
	unsigned char	m_uTargetDir;

public:
	CRole();
	~CRole();
	// ----
	void			walk			(unsigned char uDir);
	void			damage			(int nDamage, unsigned char uDamageType, int nShieldDamage);
	// ----
	void			setSkeleton		();
	void			setSet			(const CHARSET & charSet);
	void			setEquip		(const char * szPart, int nType, int nEquipID);
	// ----
	void			setCellPos		(int x, int y);
	void			setTargetCellPos(int x, int y);
	// ----
	void			drawName		() const;
	// ----
	virtual void	render			(int flag) const;
	virtual void	setActionState	(unsigned char uActionState);
	virtual void	playWalkSound	();
	// ----
	virtual int		GetObjType		()							{ return MAP_ROLE; }
	virtual bool	isDynamic		()							{ return true; }
	virtual void	OnFrameMove		(float fElapsedTime);
	// ----
	void			setID			(ULONG uID)					{ m_uID = uID; };
	void			setLevel		(int nLevel)				{ m_nLevel=nLevel; };
	void			setClass		(int nID)					{ m_nClass=nID; };
	void			setHelm			(int nID)					{ setEquip("helm",	7,	nID); };
	void			setArmor		(int nID)					{ setEquip("armor",	8,	nID); };
	void			setGlove		(int nID)					{ setEquip("glove",	10,	nID); };
	void			setPant			(int nID)					{ setEquip("pant",	9,	nID); };
	void			setBoot			(int nID)					{ setEquip("boot",	11,	nID); };
	void			setName			(const wchar_t* wcsName)	{ m_wstrName = wcsName; };
	void			setDir			(UCHAR uDir)				{ m_uDir = uDir; };
	void			setTargetDir	(UCHAR uDir)				{ m_uTargetDir = uDir; };
	// ----
	ULONG			getID			()							{ return m_uID; };
	CONSTLPWCHAR	getName			()							{ return m_wstrName.c_str(); };
	UCHAR			getActionState	()const						{ return m_uActionState; };
	STDVECTIORCHAR& getPath			()							{ return m_setPath; };
	// ----
	enum /* ActionStateType */
	{
		STAND,STARTWALK,
		WALK,
		ENDWALK,
		STARTRUN,
		RUN,
		ENDRUN,
		JUMP,
		HIT1,
		HIT2,
		HIT3,
		HIT4,
		SKILL,
		SPELL,
		DIE,
		DEATH
	};
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern const int DX[];
extern const int DY[];
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned char GetDir(const Vec3D & vSrcDir, const Vec3D & vDestDir);
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------