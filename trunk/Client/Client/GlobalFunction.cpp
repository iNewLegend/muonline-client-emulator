#include "ClientNetWork.h"
#include "GlobalFunction.h"
#include "lua_tinker.h"
#include "Audio.h"
#include "protocol.h"
#include "MainRoot.h"
#include "Monster.h"
#include "UIGeneralDialog.h"
#include "UIDisplayModel.h"
#include "UIChatList.h"
#include "UIIcon.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool g_bLocal = false;

void InitLua(lua_State * L)
{
	// Local
	lua_tinker::set(L, "g_bLocal", &g_bLocal);
	// Func
	lua_tinker::def(L,"ConnectServer",	CSConnectServer);
	lua_tinker::def(L,"LoginServer",	CSLoginServer);
	lua_tinker::def(L,"CharCreate",		CSCharCreate);
	lua_tinker::def(L,"CharDelete",		CharDelete);
	lua_tinker::def(L,"EnterWorld",		CSEnterWorld);
	lua_tinker::def(L,"GameClose",		GameClose);
	lua_tinker::def(L,"SendChat",		CSChat);
	lua_tinker::def(L, "getControlFocus",		CUIControl::getControlFocus);
	lua_tinker::def(L, "getControlPressed",		CUIControl::getControlPressed);
	lua_tinker::def(L, "getControlMouseOver",	CUIControl::getControlMouseOver);
	lua_tinker::def(L, "getControlEvent",		CUIControl::getControlEvent);
	// ----
	// Audio
	lua_tinker::class_<CAudio>(L,"Audio")
		.con(lua_tinker::constructor<void>())
		.def("PlayMusic",	& CAudio::PlayBGM)
		.def("ReplayMusic",	& CAudio::ReplayMusic)
		.def("PausedMusic",	& CAudio::PausedMusic)
		.def("StopMusic",	& CAudio::StopMusic)
		.def("PlaySound",	& CAudio::playSound)
		.def("SetBGMVolume",& CAudio::SetBGMVolume)
		.def("SetSeVolume",	& CAudio::SetSeVolume);
	lua_tinker::set(L, "g_Audio", & GetAudio());
	// ----
	// Vec3D
	lua_tinker::class_<Vec3D>(L, "Vec3D")
		.con(lua_tinker::constructor<float,float,float>())
		.mem("x",	& Vec3D::x)
		.mem("y",	& Vec3D::y)
		.mem("z",	& Vec3D::z);
	// ----
	// Render Node
	lua_tinker::class_<CRenderNode>(L, "CRenderNode")
		.con(lua_tinker::constructor<void>())
		.def("setName",& CRenderNode::setName)
		.def("setFilename",	& CRenderNode::setFilename);
	// ----
	// SkeletonNode Node
	lua_tinker::class_<CSkeletonNode>(L, "CSkeletonNode")
		.inh<CRenderNode>()
		.con(lua_tinker::constructor<void>());
	// ----
	// Character Data
	lua_tinker::class_<CHARACTER_DATA>(L, "CHARACTER_DATA")
		.con(lua_tinker::constructor<void>())
		.mem("MapX",			& CHARACTER_DATA::MapX)
		.mem("MapY",			& CHARACTER_DATA::MapY)
		.mem("MapNumber",		& CHARACTER_DATA::MapNumber)
		.mem("Exp",				& CHARACTER_DATA::Exp)
		.mem("NextExp",			& CHARACTER_DATA::NextExp)
		.mem("LevelUpPoint",	& CHARACTER_DATA::LevelUpPoint)
		.mem("Str",				& CHARACTER_DATA::Str)
		.mem("Dex",				& CHARACTER_DATA::Dex)
		.mem("Vit",				& CHARACTER_DATA::Vit)
		.mem("Energy",			& CHARACTER_DATA::Energy)
		.mem("Life",			& CHARACTER_DATA::Life)
		.mem("MaxLife",			& CHARACTER_DATA::MaxLife)
		.mem("Mana",			& CHARACTER_DATA::Mana)
		.mem("MaxMana",			& CHARACTER_DATA::MaxMana)
		.mem("wShield",			& CHARACTER_DATA::wShield)
		.mem("wMaxShield",		& CHARACTER_DATA::wMaxShield)
		.mem("wMaxShield",		& CHARACTER_DATA::wMaxShield)
		.mem("wMaxShield",		& CHARACTER_DATA::wMaxShield)
		.mem("BP",				& CHARACTER_DATA::BP)
		.mem("MaxBP",			& CHARACTER_DATA::MaxBP)
		.mem("Money",			& CHARACTER_DATA::Money)
		.mem("PkLevel",			& CHARACTER_DATA::PkLevel)
		.mem("CtlCode",			& CHARACTER_DATA::CtlCode)
		.mem("AddPoint",		& CHARACTER_DATA::AddPoint)
		.mem("MaxAddPoint",		& CHARACTER_DATA::MaxAddPoint)
		.mem("Leadership",		& CHARACTER_DATA::Leadership)
		.mem("wMinusPoint",		& CHARACTER_DATA::wMinusPoint)
		.mem("wMaxMinusPoint",	& CHARACTER_DATA::wMaxMinusPoint);
	// ----
	// Character
	lua_tinker::class_<CRole>(L, "CRole")
		.inh<CRenderNode>()
		.con(lua_tinker::constructor<void>())
		.def("setRoleID",		& CPlayerMe::setRoleID)
		.def("setCellPos",		& CPlayerMe::setCellPos)
		.def("getCellPosX",		& CPlayerMe::getCellPosX)
		.def("getCellPosY",		& CPlayerMe::getCellPosY)
		.def("getCharacterData",& CRole::getCharacterDataForLua)
		.def("setRoleName",		& CPlayerMe::setRoleName)
		.def("setDir",			& CPlayerMe::setDir)
		.def("setActionState",	& CPlayerMe::setActionState)
		.def("setAnim",			& CPlayerMe::setAnim)
		.def("setClass",		& CPlayerMe::setClass)
		.def("setSkeleton",		& CPlayerMe::setSkeleton)
		.def("setEquip",		& CPlayerMe::setEquip)
		.def("setEquipSkin",	& CPlayerMe::setEquipSkin);
	// ----
	// Monster
	lua_tinker::class_<CMonster>(L, "CMonster")
		.inh<CRole>()
		.con(lua_tinker::constructor<void>())
		.def("setType",			& CMonster::setType);
	// ----
	// PlayerMe
	lua_tinker::class_<CPlayerMe>(L, "CPlayerMe")
		.inh<CRole>()
		.con(lua_tinker::constructor<void>());
	lua_tinker::set(L, "g_PlayerMe", &CPlayerMe::getInstance());
	// ----
	// Camera
	lua_tinker::class_<CCamera>(L,"CCamera")
		.con(lua_tinker::constructor<void>())
		.def("setRadius",			&CCamera::setRadius)
		.def("getRadius",			&CCamera::getRadius)

		.def("setDefaultRadius",	&CCamera::setDefaultRadius)
		.def("getDefaultRadius",	&CCamera::getDefaultRadius)

		.def("setMinRadius",		&CCamera::setMinRadius)
		.def("getMinRadius",		&CCamera::getMinRadius)

		.def("setMaxRadius",		&CCamera::setMaxRadius)
		.def("getMaxRadius",		&CCamera::getMaxRadius)

		.def("setYawAngle",			&CCamera::setYawAngle)
		.def("getYawAngle",			&CCamera::getYawAngle)

		.def("setPitchAngle",		&CCamera::setPitchAngle)
		.def("getPitchAngle",		&CCamera::getPitchAngle)

		.def("setMinPitchAngle",	&CCamera::setMinPitchAngle)
		.def("getMinPitchAngle",	&CCamera::getMinPitchAngle)

		.def("setMaxPitchAngle",	&CCamera::setMaxPitchAngle)
		.def("getMaxPitchAngle",	&CCamera::getMaxPitchAngle);
	lua_tinker::set(L, "g_Camera", &CMainRoot::getInstance().getMainDialog().getDisplay().getCamera());
	// ----
	// World
	lua_tinker::class_<CWorld>(L, "CWorld")
		.con(lua_tinker::constructor<void>())
		.def("create",		& CWorld::create)
		.def("addChild",	& CWorld::addChild)
		.def("addRole",		& CWorld::addRole);
	lua_tinker::set(L, "g_World", &CWorld::getInstance());
	// ----
	// Item Data
	lua_tinker::class_<ItemData>(L, "ItemData")
		.con(lua_tinker::constructor<void>())
		.mem("cType",		& ItemData::cType)
		.mem("cIndex",		& ItemData::cIndex)
		.mem("level",		& ItemData::level);
	// ----
	// Icon
	lua_tinker::class_<CUIIcon>(L, "CUIIcon")
		.inh<CUIControl>()
		.con(lua_tinker::constructor<void>())
		.def("setItemData",	& CUIIcon::setItemData);
	// ----
	// Display Model
	lua_tinker::class_<CUIDisplayModel>(L, "CDisplayModel")
		.inh<CUIControl>()
		.con(lua_tinker::constructor<void>())
		.def("LoadModel",	& CUIDisplayModel::LoadModel)
		.def("setRenderNode",	& CUIDisplayModel::setRenderNode)
		.def("getRenderNode",	& CUIDisplayModel::getRenderNode)
		.mem("m_vEye",		& CUIDisplayModel::m_vEye)
		.mem("m_vLookAt",	& CUIDisplayModel::m_vLookAt);
	// ----
	// Chat List
	lua_tinker::class_<CUIChatList>(L, "CUIChatList")
		.inh<CUIListBox>()
		.con(lua_tinker::constructor<void>())
		.def("addMessage",	& CUIChatList::addMessage);
	lua_tinker::set(L, "g_UIChatList", &CUIChatList::getInstance());
	// ----
	// General Dialog
	lua_tinker::class_<CUIGeneralDialog>(L, "CUIGeneralDialog")
		.inh<CUIDialog>()
		.con(lua_tinker::constructor<void>());
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LUA_MessageBox(const wchar_t* wcsText, int nType)
{
	lua_tinker::call<void>(CUICombo::m_pLuaState, "MessageBox",wcsText,nType);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GameClose()
{
	//DXUTShutdown();
	//DXUTToggleREF();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------