#include "Script.hpp"

using namespace Libraries;

__forceinline static int GetGc(lua_State* L)
{
	auto IncludeTables = lua_gettop(L) ? luaL_optboolean(L, 1, 0) : true;

	lua_newtable(L);

	struct CGarbageCollector
	{
		lua_State* L;
		int IncludeTables;
		int Count;
	} Gct{ L, IncludeTables, 0 };

	luaM_visitgco(L, &Gct, [](void* Context, lua_Page* Page, GCObject* Gco) {
		auto Gct = static_cast<CGarbageCollector*>(Context);

		if (!((Gco->gch.marked ^ WHITEBITS) & otherwhite(Gct->L->global)))
			return false;

		auto tt = Gco->gch.tt;
		if (tt == LUA_TFUNCTION || tt == LUA_TUSERDATA || (Gct->IncludeTables && tt == LUA_TTABLE))
		{
			Gct->L->top->value.gc = Gco;
			Gct->L->top->tt = Gco->gch.tt;
			Gct->L->top++;

			lua_rawseti(Gct->L, -2, ++Gct->Count);
		}
		return false;
		});

	return 1;
}

__forceinline static int GetReg(lua_State* L)
{
	lua_pushvalue(L, LUA_REGISTRYINDEX);
	return 1;
}

__forceinline static int GetGenv(lua_State* L)
{
	lua_pushvalue(L, LUA_ENVIRONINDEX);
	return 1;
}

__forceinline static int SetThreadIdentity(lua_State* L)
{
	luaL_stackcheck(L, 1, 1);
	luaL_checktype(L, 1, LUA_TNUMBER);

	RBX::IdentityManager->SetIdentity(lua_tonumber(L, 1));
	return 0;
}

__forceinline static int GetThreadIdentity(lua_State* L)
{
	luaL_stackcheck(L, 0, 0);
	lua_pushnumber(L, RBX::IdentityManager->GetIdentity());
	return 1;
}

__forceinline static int IdentifyExecutor(lua_State* L)
{
	luaL_stackcheck(L, 0, 0);

	lua_pushstring(L, Configuration::Name);
	lua_pushstring(L, Configuration::Version);
	return 2;
}

__forceinline static int GetExecutorName(lua_State* L)
{
	luaL_stackcheck(L, 0, 0);

	lua_pushstring(L, Configuration::Name);
	return 1;
}

__forceinline static int GetHui(lua_State* L)
{
	luaL_stackcheck(L, 0, 0);

	lua_getglobal(L, xorstr_("game"));
	lua_getfield(L, -1, xorstr_("GetService"));

	lua_pushvalue(L, -2);
	lua_pushstring(L, xorstr_("CoreGui"));

	lua_pcall(L, 2, 1, 0);
	return 1;
}

__forceinline static int GetCallbackValue(lua_State* L)
{
	luaL_stackcheck(L, 2, 2);

	luaL_checktype(L, 1, LUA_TUSERDATA);
	luaL_checktype(L, 2, LUA_TSTRING);

	auto Instance = *(uintptr_t*)lua_touserdata(L, 1);

	int Atom;
	auto Property = lua_tostringatom(L, 2, &Atom);

	const auto KTable = (uintptr_t*)Update::KTable;
	auto Descriptor = KTable[Atom];
	if (!Descriptor)
		return 0;

	auto ClassDescriptor = *(uintptr_t*)(Instance + 0x18);
	const auto Prop = RBX::GetProperty(ClassDescriptor + 0x3b0, &Descriptor);
	if (!Prop)
		return 0;

	int Index = *(uint32_t*)(*(uintptr_t*)(*(uintptr_t*)(*(uintptr_t*)((Instance + *(uintptr_t*)(*Prop + 0x78)) + 0x18) + 0x38) + 0x28) + 0x14);
	lua_getref(L, Index);

	if (lua_isfunction(L, -1))
		return 1;

	lua_pop(L, 1);

	return 0;
}

__forceinline static int IsScriptable(lua_State* L)
{
	const auto PropName = luaL_checkstring(L, 2);

	for (const auto Instance = *(RBX::Instance**)lua_touserdata(L, 1);
		const auto & Prop: Instance->classDescriptor->propertyDescriptors.descriptors) 
	{
		if (Prop->name == PropName)
		{
			lua_pushboolean(L, Prop->IsScriptable());
			return 1;
		}
	}

	if (lua_type(L, -1) != lua_Type::LUA_TBOOLEAN)
		lua_pushnil(L);

	return 1;
}

__forceinline static int SetScriptable(lua_State* L)
{
	const auto PropName = luaL_checkstring(L, 2);
	const bool NewScriptable = luaL_checkboolean(L, 3);

	for (const auto Instance = *(RBX::Instance**)lua_touserdata(L, 1);
		const auto & Prop: Instance->classDescriptor->propertyDescriptors.descriptors)
	{
		if (Prop->name == PropName)
		{
			lua_pushboolean(L, Prop->IsScriptable());
			Prop->SetScriptable(NewScriptable);
			return 1;
		}
	}
	return 0;
}

__forceinline static int Require__Handler(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TUSERDATA);

	const auto TargetModule = *(RBX::ModuleScript**)lua_touserdata(L, 1);
	*(__int64*)(TargetModule + 0x1A8) = 1;

	return 0;
}

__forceinline static int GetInstances(lua_State* L)
{
	luaL_stackcheck(L, 0, 0);

	struct CInstanceContext 
	{
		lua_State* L;
		__int64 n;
	} Context = { L, 0 };

	lua_createtable(L, 0, 0);
	for (lua_Page* Page = L->global->allgcopages; Page;)
	{
		lua_Page* Next{ Page->listnext }; /* Block visit might destroy the page */

		luaM_visitpage(Page, &Context, [](void* Context, lua_Page* Page, GCObject* Gco) -> bool {
			auto GcContext = (CInstanceContext*)Context;
			const auto Type= Gco->gch.tt;

			if (Type == LUA_TUSERDATA)
			{
				TValue* top = GcContext->L->top;
				top->value.p = reinterpret_cast<void*>(Gco);
				top->tt = Type;
				GcContext->L->top++;

				if (!strcmp(luaL_typename(GcContext->L, -1), xorstr_("Instance")))
				{
					GcContext->n++;
					lua_rawseti(GcContext->L, -2, GcContext->n);
				}
				else
					lua_pop(GcContext->L, 1);
			}

			return true;
			}
		);

		Page = Next;
	}

	return 1;
}

__forceinline static int GetNilInstances(lua_State* L)
{
	luaL_stackcheck(L, 0, 0);

	struct CInstanceContext
	{
		lua_State* L;
		__int64 n;
	} Context = { L, 0 };

	lua_createtable(L, 0, 0);
	for (lua_Page* Page = L->global->allgcopages; Page;)
	{
		lua_Page* Next{ Page->listnext }; /* Block visit might destroy the page */

		luaM_visitpage(Page, &Context, [](void* Context, lua_Page* Page, GCObject* Gco) -> bool {
			auto GcContext = (CInstanceContext*)Context;
			const auto Type = Gco->gch.tt;

			if (Type == LUA_TUSERDATA)
			{
				TValue* top = GcContext->L->top;
				top->value.p = reinterpret_cast<void*>(Gco);
				top->tt = Type;
				GcContext->L->top++;

				if (!strcmp(luaL_typename(GcContext->L, -1), xorstr_("Instance")))
				{
					lua_getfield(GcContext->L, -1, xorstr_("Parent"));

					const auto NullParent = lua_isnoneornil(GcContext->L, -1);
					if (NullParent)
					{
						lua_pop(GcContext->L, 1);
						GcContext->n++;
						lua_rawseti(GcContext->L, -2, GcContext->n);
					}
					else
						lua_pop(GcContext->L, 2);
				}
				else
					lua_pop(GcContext->L, 1);
			}

			return true;
			}
		);

		Page = Next;
	}
	return 1;
}

static const luaL_Reg LibraryFunctions[] = {
	{"getgc", GetGc}, {"get_gc", GetGc}, {"GetGc", GetGc},

	{"getreg", GetReg}, {"get_reg", GetReg}, {"GetReg", GetReg},

	{"getgenv", GetGenv}, {"getgenv", GetGenv}, {"getgenv", GetGenv},

	{"setthreadidentity", SetThreadIdentity}, {"set_thread_identity", SetThreadIdentity}, {"SetThreadIdentity", SetThreadIdentity},
	{"setidentity", SetThreadIdentity}, {"set_identity", SetThreadIdentity}, {"SetIdentity", SetThreadIdentity},
	{"setthreadcontext", SetThreadIdentity}, {"set_thread_context", SetThreadIdentity}, {"SetThreadContext", SetThreadIdentity},

	{"getthreadidentity", GetThreadIdentity}, {"get_thread_identity", GetThreadIdentity}, {"GetThreadIdentity", GetThreadIdentity},
	{"getidentity", GetThreadIdentity}, {"get_identity", GetThreadIdentity}, {"GetIdentity", GetThreadIdentity},
	{"getthreadcontext", GetThreadIdentity}, {"get_thread_context", GetThreadIdentity}, {"GetThreadContext", GetThreadIdentity},

	{"identifyexecutor", IdentifyExecutor}, {"identify_executor", IdentifyExecutor}, {"IdentifyExecutor", IdentifyExecutor},

	{"getexecutorname", GetExecutorName}, {"get_executor_name", GetExecutorName}, {"GetExecutorName", GetExecutorName},

	{"gethui", GetHui}, {"get_hui", GetHui}, {"GetHui", GetHui},

	{"getinstances", GetInstances}, {"get_instances", GetInstances}, {"GetInstances", GetInstances},

	{"getnilinstances", GetNilInstances}, {"get_nil_instances", GetNilInstances}, {"GetNilInstances", GetNilInstances},

	/* {"getcallbackvalue", GetCallbackValue}, {"get_callback_value", GetCallbackValue}, {"GetCallbackValue", GetCallbackValue}, */

	{"isscriptable", IsScriptable}, {"is_scriptable", IsScriptable}, {"IsScriptable", IsScriptable},

	{"setscriptable", SetScriptable}, {"set_scriptable", SetScriptable}, {"SetScriptable", SetScriptable},

	{"Require_Handler", Require__Handler},

	{nullptr, nullptr}
};

void Script::Initialize(lua_State* L)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, LibraryFunctions);
	lua_pop(L, 1);
}