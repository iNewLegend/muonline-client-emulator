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
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void InitLua(lua_State * L)
{
	lua_tinker::def(L,"ConnectServer",	CSConnectServer);
	lua_tinker::def(L,"LoginServer",	CSLoginServer);
	lua_tinker::def(L,"RoleCreate",		CSRoleCreate);
	lua_tinker::def(L,"EnterWorld",		CSEnterWorld);
	lua_tinker::def(L,"GameClose",		GameClose);
	lua_tinker::def(L,"SendChat",		CSChat);

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
	lua_tinker::class_<CRole>(L, "CRole")
		.inh<CRenderNode>()
		.con(lua_tinker::constructor<void>())
		.def("setID",			& CPlayerMe::setID)
		.def("setCellPos",		& CPlayerMe::setCellPos)
		.def("getCellPosX",		& CPlayerMe::getCellPosX)
		.def("getCellPosY",		& CPlayerMe::getCellPosY)
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
	lua_tinker::class_<CUIDisplayRoleChoose>(L, "CUIDisplayRoleChoose")
		.inh<CUIControl>()
		.con(lua_tinker::constructor<void>())
		.def("GetSelectIndex",	& CUIDisplayRoleChoose::getSelectIndex)
		.def("LoadModel",		& CUIDisplayRoleChoose::LoadModel)
		.mem("m_vEye",			& CUIDisplayRoleChoose::m_vEye)
		.mem("m_vLookAt",		& CUIDisplayRoleChoose::m_vLookAt);
	lua_tinker::set(L, "g_UIDisplayCharList", &CUIDisplayRoleChoose::getInstance());

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