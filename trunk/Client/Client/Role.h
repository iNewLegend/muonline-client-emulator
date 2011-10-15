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
	std::deque<char>m_Path;
	// ----
	unsigned char	m_uActionState;
	// ----
	float			m_fWalkSpeed;
	Vec3D			m_vNextPos;
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
	void			setEquip		(int nType, int nEquipID);
	void			setEquipLevel	(int nType, int nLevel);
	// ----
	void			setCellPos		(int x, int y);
	void			setTargetCellPos(int x, int y);
	// ----
	void			drawName		() const;
	// ----
	virtual void	setActionState	(unsigned char uActionState);
	virtual unsigned char	getActionState	();
	virtual void	playWalkSound	();
	// ----
	virtual int		GetObjType		()							{ return MAP_ROLE; }
	virtual bool	isDynamic		()							{ return true; }
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	frameMoveRole	(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	// ----
	void			setID			(ULONG uID)					{ m_uID = uID; };
	void			setLevel		(int nLevel)				{ m_nLevel=nLevel; };
	void			setClass		(int nID)					{ m_nClass=nID; };
	void			setRoleName		(const wchar_t* wcsName)	{ m_wstrName = wcsName; };
	void			setDir			(UCHAR uDir)				{ m_uDir = uDir; };
	void			setTargetDir	(UCHAR uDir)				{ m_uTargetDir = uDir; };
	// ----
	ULONG			getID			()							{ return m_uID; };
	const wchar_t*	getName			()							{ return m_wstrName.c_str(); };
	UCHAR			getActionState	()const						{ return m_uActionState; };
	std::deque<char>& getPath		()							{ return m_Path; };
	// ----
	void			moveStep();
	enum /* ActionStateType */
	{
		STAND,
		WALK,
		RUN,
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