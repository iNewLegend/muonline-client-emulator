#include "ClientNetWork.h"
#include "GlobalFunction.h"
#include "lua_tinker.h"
#include "Audio.h"
#include "UIDisplayModel.h"
#include "MainRoot.h"
#include "UIDisplayRoleChoose.h"
#include "UIIcon.h"
#include "protocol.h"
#include "Monster.h"
#include "UIGeneralDialog.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void InitLua(lua_State * L)
{
	lua_tinker::def(L,"ConnectServer",	CSConnectServer);
	lua_tinker::def(L,"LoginServer",	CSLoginServer);
	lua_tinker::def(L,"RoleCreate",		CSRoleCreate);
	lua_tinker::def(L,"EnterWorld",		CSEnterWorld);
	lua_tinker::def(L,"GameClose",		GameClose);
	lua_tinker::def(L,"SendChat",		CSChat);
	lua_tinker::def(L, "getControlFocus",		CUIControl::getControlFocus);
	lua_tinker::def(L, "getControlPressed",		CUIControl::getControlPressed);
	lua_tinker::def(L, "getControlMouseOver",	CUIControl::getControlMouseOver);
	lua_tinker::def(L, "getControlEvent",		CUIControl::getControlEvent);



	// ----
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
	lua_tinker::class_<Vec3D>(L, "Vec3D")
		.con(lua_tinker::constructor<float,float,float>())
		.mem("x",	& Vec3D::x)
		.mem("y",	& Vec3D::y)
		.mem("z",	& Vec3D::z);
	// ----
	lua_tinker::class_<CRenderNode>(L, "CRenderNode")
		.con(lua_tinker::constructor<void>());

	// ----
	lua_tinker::class_<CHARACTER_DATA>(L, "CHARACTER_DATA")
		.con(lua_tinker::constructor<void>())
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
	lua_tinker::class_<CRole>(L, "CRole")
		.inh<CRenderNode>()
		.con(lua_tinker::constructor<void>())
		.def("setID",			& CPlayerMe::setID)
		.def("setCellPos",		& CPlayerMe::setCellPos)
		.def("getCellPosX",		& CPlayerMe::getCellPosX)
		.def("getCellPosY",		& CPlayerMe::getCellPosY)
		.def("getCharacterData",& CRole::getCharacterData)
		.def("setRoleName",		& CPlayerMe::setRoleName)
		.def("setDir",			& CPlayerMe::setDir)
		.def("setActionState",	& CPlayerMe::setActionState)
		.def("setAnim",			& CPlayerMe::setAnim)
		.def("setClass",		& CPlayerMe::setClass)
		.def("setSkeleton",		& CPlayerMe::setSkeleton)
		.def("setEquip",		& CPlayerMe::setEquip)
		.def("setEquipSkin",	& CPlayerMe::setEquipSkin);
	// ----
	lua_tinker::class_<CMonster>(L, "CMonster")
		.inh<CRole>()
		.con(lua_tinker::constructor<void>())
		.def("setType",			& CMonster::setType);
	// ----
	lua_tinker::class_<CPlayerMe>(L, "CPlayerMe")
		.inh<CRole>()
		.con(lua_tinker::constructor<void>());
	lua_tinker::set(L, "g_PlayerMe", &CPlayerMe::getInstance());
	// ----
	lua_tinker::class_<CWorld>(L, "CWorld")
		.con(lua_tinker::constructor<void>())
		.def("create",		& CWorld::create)
		.def("addRole",		& CWorld::addRole);
	lua_tinker::set(L, "g_World", &CWorld::getInstance());
	// ----
	lua_tinker::class_<ItemData>(L, "ItemData")
		.con(lua_tinker::constructor<void>())
		.mem("cType",		& ItemData::cType)
		.mem("cIndex",		& ItemData::cIndex)
		.mem("level",		& ItemData::level);
	// ----
	lua_tinker::class_<CUIIcon>(L, "CUIIcon")
		.inh<CUIControl>()
		.con(lua_tinker::constructor<void>())
		.def("setItemData",	& CUIIcon::setItemData);
	// ----
	lua_tinker::class_<CUIDisplayModel>(L, "CDisplayModel")
		.inh<CUIControl>()
		.con(lua_tinker::constructor<void>())
		.def("LoadModel",	& CUIDisplayModel::LoadModel)
		.def("setRenderNode",	& CUIDisplayModel::setRenderNode)
		.def("getRenderNode",	& CUIDisplayModel::getRenderNode)
		.mem("m_vEye",		& CUIDisplayModel::m_vEye)
		.mem("m_vLookAt",	& CUIDisplayModel::m_vLookAt);
	// ----
	lua_tinker::class_<CUIDisplayRoleList>(L, "CUIDisplayRoleChoose")
		.inh<CUIControl>()
		.con(lua_tinker::constructor<void>())
		.def("GetSelectIndex",	& CUIDisplayRoleList::getSelectIndex)
		.def("LoadModel",		& CUIDisplayRoleList::LoadModel)
		.mem("m_vEye",			& CUIDisplayRoleList::m_vEye)
		.mem("m_vLookAt",		& CUIDisplayRoleList::m_vLookAt);
	lua_tinker::set(L, "g_UIDisplayCharList", &CUIDisplayRoleList::getInstance());
	// ----
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