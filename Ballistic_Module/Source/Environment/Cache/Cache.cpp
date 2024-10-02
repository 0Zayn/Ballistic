#include "Cache.hpp"

using namespace Libraries;

__forceinline static int Cache__Invalidate(lua_State* L)
{
	luaL_stackcheck(L, 1, 1);

	luaL_checktype(L, 1, LUA_TUSERDATA);

	if (strcmp(luaL_typename(L, 1), xorstr_("Instance")) != LUA_OK)
	{
		luaL_typeerror(L, 1, xorstr_("Instance"));
		return 0;
	}

	const auto Instance = *(__int64*)lua_touserdata(L, 1);

	lua_pushlightuserdata(L, (void*)Update::PushInstance);

	lua_gettable(L, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L, (void*)Instance);
	lua_pushnil(L);

	lua_settable(L, -3);
	return 0;
}

__forceinline static int Cache__IsCached(lua_State* L)
{
	luaL_stackcheck(L, 1, 1);

	luaL_checktype(L, 1, LUA_TUSERDATA);

	if (strcmp(luaL_typename(L, 1), xorstr_("Instance")) != LUA_OK)
	{
		luaL_typeerror(L, 1, xorstr_("Instance"));
		return 0;
	}

	const auto Instance = *(__int64*)lua_touserdata(L, 1);

	lua_pushlightuserdata(L, (void*)Update::PushInstance);

	lua_gettable(L, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L, (void*)Instance);

	lua_gettable(L, -2);
	lua_pushboolean(L, !lua_isnil(L, -1));

	return 1;
}

__forceinline static int Cache__Replace(lua_State* L)
{
	luaL_stackcheck(L, 2, 2);

	luaL_checktype(L, 1, LUA_TUSERDATA);
	luaL_checktype(L, 2, LUA_TUSERDATA);

	if (strcmp(luaL_typename(L, 1), xorstr_("Instance")) != LUA_OK)
	{
		luaL_typeerror(L, 1, xorstr_("Instance"));
		return 0;
	}

	if (strcmp(luaL_typename(L, 2), xorstr_("Instance")) != LUA_OK)
	{
		luaL_typeerror(L, 2, xorstr_("Instance"));
		return 0;
	}

	const auto Instance = *(__int64*)lua_touserdata(L, 1);

	lua_pushlightuserdata(L, (void*)Update::PushInstance);

	lua_gettable(L, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L, (void*)Instance);
	lua_pushvalue(L, 2);
	lua_settable(L, -3);

	return 0;
}

__forceinline static int CloneRef(lua_State* L)
{
	luaL_stackcheck(L, 1, 1);

	luaL_checktype(L, 1, LUA_TUSERDATA);

	auto OriginalUserdata = lua_touserdata(L, 1);
	const auto ReturnedUserdata = *(uintptr_t*)OriginalUserdata;

	lua_pushlightuserdata(L, (void*)Update::PushInstance);

	lua_rawget(L, -10000);
	lua_pushlightuserdata(L, (void*)ReturnedUserdata);
	lua_rawget(L, -2);

	lua_pushlightuserdata(L, (void*)ReturnedUserdata);
	lua_pushnil(L);
	lua_rawset(L, -4);

	RBX::PushInstance(L, OriginalUserdata);

	lua_pushlightuserdata(L, (void*)ReturnedUserdata);
	lua_pushvalue(L, -3);
	lua_rawset(L, -5);

	return 1;
}

__forceinline static int CompareInstances(lua_State* L)
{
	luaL_stackcheck(L, 2, 2);

	luaL_checktype(L, 1, LUA_TUSERDATA);
	luaL_checktype(L, 2, LUA_TUSERDATA);

	if (strcmp(luaL_typename(L, 1), xorstr_("Instance")) != LUA_OK)
	{
		luaL_typeerror(L, 1, xorstr_("Instance"));
		return 0;
	}

	if (strcmp(luaL_typename(L, 2), xorstr_("Instance")) != LUA_OK)
	{
		luaL_typeerror(L, 2, xorstr_("Instance"));
		return 0;
	}

	lua_pushboolean(L, (*(__int64*)lua_touserdata(L, 1) == *(__int64*)lua_touserdata(L, 2)));
	return 1;
}

__forceinline static int SetClipboard(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TSTRING);

	size_t l = 0;
	auto s = luaL_checklstring(L, 1, &l);
	std::string content = std::string(s, l);

	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, l + 1);
	memcpy(GlobalLock(hMem), content.data(), l);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
	return 0;
}

__forceinline static int GetClipboard(lua_State* L)
{
	OpenClipboard(NULL);
	HANDLE hData = GetClipboardData(CF_TEXT);
	char* pszText = (char*)GlobalLock(hData);
	std::string pszTextCopy(pszText);

	GlobalUnlock(hData);
	CloseClipboard();
	lua_pushstring(L, pszTextCopy.c_str());
	return 1;
}

static const luaL_Reg LibraryFunctions[] = {
	{"invalidate", Cache__Invalidate}, {"Invalidate", Cache__Invalidate},

	{"iscached", Cache__IsCached}, {"is_cached", Cache__IsCached}, {"IsCached", Cache__IsCached},
	
	{"replace", Cache__Replace}, {"Replace", Cache__Replace},

	{nullptr, nullptr}
};

void Cache::Initialize(lua_State* L)
{
	lua_newtable(L);

	luaL_register(L, nullptr, LibraryFunctions);
	lua_setreadonly(L, -1, true);
	lua_setglobal(L, xorstr_("cache"));

	/* This is a little retarded */
	const char* CloneRefAliases[] = { "cloneref", "clone_ref", "CloneRef" };
	for (const auto& Alias : CloneRefAliases)
	{
		lua_pushcclosure(L, CloneRef, Alias, 0);
		lua_setglobal(L, Alias);
	}

	const char* CompareInstancesAliases[] = { "compareinstances", "compare_instances", "CompareInstances" };
	for (const auto& Alias : CompareInstancesAliases)
	{
		lua_pushcclosure(L, CompareInstances, Alias, 0);
		lua_setglobal(L, Alias);
	}

	const char* SetClipboardAliases[] = { "setclipboard", "set_clipboard", "SetClipboard", "toclipboard", "to_clipboard", "ToClipboard" };
	for (const auto& Alias : SetClipboardAliases)
	{
		lua_pushcclosure(L, SetClipboard, Alias, 0);
		lua_setglobal(L, Alias);
	}

	const char* GetClipboardAliases[] = { "getclipboard", "get_clipboard", "GetClipboard" };
	for (const auto& Alias : GetClipboardAliases)
	{
		lua_pushcclosure(L, GetClipboard, Alias, 0);
		lua_setglobal(L, Alias);
	}
}