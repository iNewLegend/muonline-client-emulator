//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* Role.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <deque> 
#include "SkeletonNode.h"
#include "Pos2D.h"
#include "..\..\Protocol\prodef.h"
#include "..\..\Protocol\GameProtocol.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PLAYER_BMD_PATCH	"Data\\Player\\player.bmd"
// ----
#define DIR_COUNT			8
#define CLASS_TEMP_SIZE		256
#define DIR_LENGTH(a)		sqrt((float)DX[a] * DX[a] + (float)DY[a] * DY[a])
#define DIR_NORMALIZE(a)	Vec3D((float)DX[a], 0.0f,(float)DY[a]).normalize()
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



struct RoleCmd 
{
	enum /* WeaponStateType */
	{
		STAND,
		MOVE,
		DIR,
		POSE,
		ATTACT,
		ACTION,
		DEAD,
	};
	int nType;
	int nParam1;
	int nParam2;
	int nParam3;
};
class CRole : public CSkeletonNode
{
public:
	CRole();
	~CRole();
	// ----
	void			damage			(int nDamage, unsigned char uDamageType, int nShieldDamage);
	// ----
	void			setSkeleton		();
	void			setSet			(const unsigned char* charSet);
	void			setEquip		(int nType, int nEquipID);
	void			setEquipSkin	(int nType, int nSkinID);
	// ----
	void			setCellPos		(int x, int y);
	int				getCellPosX		(){return m_vPos.x;}
	int				getCellPosY		(){return m_vPos.z;}
	// ----
	void			drawName		() const;
	// ----
	virtual void	updateAction	();
	virtual void	setSafeState	(unsigned char uState)		{ m_uSafeState=uState; updateAction();}
	virtual void	setActionState	(unsigned char uState);
	virtual void	setWeaponState	(unsigned char uState)		{ m_uWeaponState=uState; updateAction();}
	UCHAR			getActionState	()const						{ return m_uActionState; };
	UCHAR			getWeaponState	()const						{ return m_uWeaponState; };
	virtual void	playWalkSound	();
	// ----
	virtual bool	isDynamic		()							{ return true; }
	virtual void	frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	frameMoveRole	(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render			(int nRenderType)const;
	virtual void	animate			(const Matrix& mWorld, double fTime, float fElapsedTime);
	// ----
	void			setRoleID			(ULONG uID)				{ m_uRoleID = uID; };
	void			setLevel		(int nLevel)				{ m_nLevel=nLevel; };
	void			setClass		(int nID)					{ m_nClass=nID; };
	void			setRoleName		(const wchar_t* wcsName)	{ m_wstrName = wcsName; };
	void			setDir			(UCHAR uDir)				{ m_uDir = uDir; };
	// ----
	ULONG			getRoleID		()							{ return m_uRoleID; };
	const wchar_t*	getName			()							{ return m_wstrName.c_str(); };

	std::deque<char>& getPath		()							{ return m_Path; };
	// ----
	void			moveStep();
	void			startRoleCmd(const RoleCmd& cmd);
	void			doRoleCmd(const RoleCmd& cmd, double fTime, float fElapsedTime);
	void			nextRoleCmd();
	void			clearRoleCmd(){m_RoleCmds.clear();}
	void			addRoleCmd(const RoleCmd& cmd){m_RoleCmds.push_back(cmd);}
	const RoleCmd&	getCurRoleCmd()const{return m_CurRoleCmd;}
	
	CHARACTER_DATA&	getCharacterData(){return m_CharacterData;}
	CHARACTER_DATA*	getCharacterDataForLua(){return &m_CharacterData;}
	void			setCharacterData(const CHARACTER_DATA& data);
	virtual void	release		()						{delete this;}
	GSET_VAR(float,m_f,RoleHeight);

	enum /* ActionStateType */
	{
		STAND,
		WALK,
		RUN,
		HIT,
		SIT,
		RELY,
		DIE,
		DEATH,
		SKILL,
		SPELL,
	};
	enum /* WeaponStateType */
	{
		WS_EMPTY,
		WS_SINGLE,
		WS_DSINGLE,
		WS_DOUBLE,
		WS_DOUBLE_ROD,
		WS_BOW,
		WS_CROSSBOW
	};
private:
	char			m_szTemp[CLASS_TEMP_SIZE];
protected:
	// ----
	unsigned long	m_uRoleID;
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
	std::deque<RoleCmd>m_RoleCmds;
	RoleCmd			m_CurRoleCmd;

	// ----
	unsigned char	m_uActionState;
	unsigned char	m_uWeaponState;
	unsigned char	m_uSafeState;
	// ----
	float			m_fWalkSpeed;
	float			m_fAttackSpeed;
	Vec3D			m_vNextPos;
	// ----
	unsigned char	m_uDir;
	unsigned char	m_uTargetDir;
	// ----
	float			m_fRoleHeight;
	// ----
	CHARACTER_DATA	m_CharacterData;
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern const int DX[];
extern const int DY[];
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned char GetDir(const Vec3D & vSrcDir, const Vec3D & vDestDir);
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------