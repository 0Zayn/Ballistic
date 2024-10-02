#include "IdentityManager.hpp"

using namespace RBX;

int CIdentityManager::GetIdentity()
{
	return *(__int64*)((__int64)(((lua_State*)TaskScheduler->GetLuaState())->userdata) + Update::Userdata::Identity);
}

void CIdentityManager::SetIdentity(int Identity)
{
	const auto Userdata = (__int64)(((lua_State*)TaskScheduler->GetLuaState())->userdata);
	*(__int64*)(Userdata + Update::Userdata::Identity) = Identity;
	*(__int64*)(Userdata + Update::Userdata::Capabilities) = 0x3FFFFF00 | 0x3F; /* Locked to WebService capabilities */
}