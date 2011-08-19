#include "ClientNetWork.h"
#include "GlobalFunction.h"
#include "lua_tinker.h"
#include "Audio.h"
#include "UIDisplayModel.h"
#include "MainRoot.h"
#include "UIDisplayRoleChoose.h"
#include "protocol.h"
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
	lua_tinker::class_<CUIDisplayModel>(L, "CDisplayModel")
		.inh<CUIControl>()
		.con(lua_tinker::constructor<void>())
		.def("LoadModel",	& CUIDisplayModel::LoadModel)
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
	// ----
	lua_tinker::class_<CPlayerMe>(L, "CPlayerMe")
		.con(lua_tinker::constructor<void>())
		.def("setCellPos",		& CPlayerMe::setCellPos);
	lua_tinker::set(L, "g_PlayerMe", &CPlayerMe::getInstance());
	// ----
	lua_tinker::class_<CWorld>(L, "CWorld")
		.con(lua_tinker::constructor<void>())
		.def("create",		& CWorld::create)
		.def("addRole",		& CWorld::addRole);
	lua_tinker::set(L, "g_World", &CWorld::getInstance());
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