#include "Metatable.hpp"

using namespace Libraries;

__forceinline static int GetRawMetatable(lua_State* L)
{
	luaL_stackcheck(L, 1, 1);

	const auto ObjectType = lua_type(L, 1);
	if (ObjectType != LUA_TTABLE && ObjectType != LUA_TUSERDATA)
	{
		luaL_typeerror(L, 1, xorstr_("table or userdata"));
		return 0;
	}

	if (!lua_getmetatable(L, 1))
		lua_pushnil(L);

	return 1;
}

__forceinline static int SetRawMetatable(lua_State* L)
{
	luaL_stackcheck(L, 2, 2);

	const auto ObjectType = lua_type(L, 1);
	if (ObjectType != LUA_TTABLE && ObjectType != LUA_TUSERDATA)
	{
		luaL_typeerror(L, 1, xorstr_("table or userdata"));
		return 0;
	}

	const auto MetatableType = lua_type(L, 2);
	if (MetatableType != LUA_TTABLE && MetatableType != LUA_TNIL)
	{
		luaL_typeerror(L, 2, xorstr_("nil or table"));
		return 0;
	}

	lua_settop(L, 2);
	lua_pushboolean(L, lua_setmetatable(L, 1));
	return 1;
}

__forceinline static int SetReadonly(lua_State* L)
{
	luaL_stackcheck(L, 2, 2, luaL_checktype(L, 1, LUA_TTABLE););
	luaL_checktype(L, 2, LUA_TBOOLEAN);

	((Table*)lua_topointer(L, 1))->readonly = lua_toboolean(L, 2);
	return 0;
}

__forceinline static int IsReadonly(lua_State* L)
{
	luaL_stackcheck(L, 1, 1, luaL_checktype(L, 1, LUA_TTABLE););

	lua_pushboolean(L, ((Table*)lua_topointer(L, 1))->readonly);
	return 1;
}

__forceinline static int GetNamecallMethod(lua_State* L)
{
	luaL_stackcheck(L, 0, 0);

	if (L->namecall)
	{
		lua_pushstring(L, L->namecall->data);
		return 1;
	}
	return 0;
}

__forceinline static int SetNamecallMethod(lua_State* L)
{
	luaL_stackcheck(L, 1, 1, luaL_checktype(L, 1, LUA_TSTRING););
	L->namecall = tsvalue(luaA_toobject(L, 1));
	return 0;
}

__forceinline static int HookMetamethod(lua_State* L)
{
	luaL_checkany(L, 1);

	const auto MetatableName = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TFUNCTION);

	lua_pushvalue(L, 1);
	lua_getmetatable(L, -1);
	if (lua_getfield(L, -1, MetatableName) == LUA_TNIL)
	{
		luaL_argerrorL(
			L, 2, std::format("'{}' is not a valid member of the given object's metatable.", MetatableName).c_str()
		);
	}
	lua_setreadonly(L, -2, false);

	lua_getglobal(L, xorstr_("hookfunction"));
	lua_pushvalue(L, -2);
	lua_pushvalue(L, 3);
	lua_call(L, 2, 1);
	lua_remove(L, -2);
	lua_setreadonly(L, -2, true);

	return 1;
}

static const luaL_Reg LibraryFunctions[] = {
	{"getrawmetatable", GetRawMetatable}, {"get_raw_metatable", GetRawMetatable}, {"GetRawMetatable", GetRawMetatable},

	{"setrawmetatable", SetRawMetatable}, {"set_raw_metatable", SetRawMetatable}, {"SetRawMetatable", SetRawMetatable},

	{"setreadonly", SetReadonly}, {"set_readonly", SetReadonly}, {"SetReadonly", SetReadonly},

	{"isreadonly", IsReadonly}, {"is_readonly", IsReadonly}, {"IsReadonly", IsReadonly},

	{"getnamecallmethod", GetNamecallMethod}, {"get_namecall_method", GetNamecallMethod}, {"GetNamecallMethod", GetNamecallMethod},
	
	{"setnamecallmethod", SetNamecallMethod}, {"set_namecall_method", SetNamecallMethod}, {"SetNamecallMethod", SetNamecallMethod},

	{"hookmetamethod", HookMetamethod}, {"hook_metamethod", HookMetamethod}, {"HookMetamethod", HookMetamethod},

	{nullptr, nullptr}
};

void Metatable::Initialize(lua_State* L)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, LibraryFunctions);
	lua_pop(L, 1);
}