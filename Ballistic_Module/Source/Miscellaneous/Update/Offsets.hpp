/* Updated for version-b591875ddfbc4294 */

#pragma once
#include <Windows.h>
#include <iostream>

#define REBASE(x) x + (uintptr_t)GetModuleHandle(nullptr)

#define RBX_PRINT_NORMAL 0
#define RBX_PRINT_INFO 1
#define RBX_PRINT_WARN 2
#define RBX_PRINT_ERROR 3

struct lua_State;

namespace Update
{
	const uintptr_t Print = REBASE(0x11FBE70); /* "Current identity is %d" */
	const uintptr_t GetTaskScheduler = REBASE(0x2BC4A10); /* "initialThermalStatus" */
	const uintptr_t RawTaskScheduler = REBASE(0x5BD5BA9); /* "Flags must be loaded before task scheduler can be used." */

	const uintptr_t LuaVM__Load = REBASE(0xA82390); /* "%s: bytecode version mismatch (expected [%d..%d], got %d)" */
	const uintptr_t Task__Defer = REBASE(0xDEF730); /* "Maximum re-entrancy depth (%i) exceeded calling task.defer" */

	const uintptr_t GetGlobalStateForInstance = REBASE(0xC97180); /* "Script Start" */
	const uintptr_t DecryptLuaState = REBASE(0xA7FE00); /* "Script Start" */

	const uintptr_t PushInstance = REBASE(0xC74B40);
	const uintptr_t LuaO_NilObject = REBASE(0x43D8E28); /* "%s: bytecode version mismatch (expected [%d..%d], got %d)" */
	const uintptr_t Luau_Execute = REBASE(0x2522210); /* "C stack overflow" */
	const uintptr_t LuaH_DummyNode = REBASE(0x43D8628);

	/* GetCallbackValue */
	const uintptr_t KTable = REBASE(0x51E1460);
	const uintptr_t GetProperty = REBASE(0x9D15A0);

	namespace TaskScheduler
	{
		/* Hooking these may be useful */
		const uintptr_t JobStart = REBASE(0x2B5B3C0); /* "[FLog::TaskSchedulerRun] JobStart %s" */
		const uintptr_t JobEnd = REBASE(0x2B5B4E0); /* "[FLog::TaskSchedulerRun] JobStop %s" */

		const uintptr_t JobsStart = 0x198; /* "TaskScheduler::Job:" */
		const uintptr_t JobsEnd = 0x1A0; /* "TaskScheduler::Job:" */
		const uintptr_t JobName = 0x90; /* "TaskScheduler::Job:" */

		const uintptr_t ScriptContextJob = 0x1F8; /* "WaitingHybridScriptsJob" */
		const uintptr_t ScriptContextName = 0x50;
		const uintptr_t ScriptContextState = 0x110; /* "Script Start" */

		const uintptr_t DecryptState = 0x88; /* "Script Start" */
	}

	namespace Userdata /* These members rarely change. */
	{
		const uintptr_t Identity = 0x30;
		const uintptr_t Capabilities = 0x48;
	}

	namespace Proto /* These members rarely change. */
	{
		const uintptr_t Capabilities = 0x78;
	}

	namespace DataModel
	{
		const uintptr_t PlaceId = 0x168;

		namespace ScriptContext
		{
			const uintptr_t LuaState = 0x1C8;
		}

		namespace Bytecode
		{
			const uintptr_t ModuleScript = 0x160;
			const uintptr_t LocalScript = 0x1C0;
		}
	}
}

namespace RBX
{
	using _Print = int(__fastcall*)(int, const char*, ...);
	inline auto Print = (_Print)Update::Print;

	using _GetTaskScheduler = uintptr_t(__fastcall*)();
	inline auto GetTaskScheduler = (_GetTaskScheduler)Update::GetTaskScheduler;

	using _LuaVM__Load = int(__fastcall*)(lua_State*, std::string*, const char*, OPTIONAL int);
	inline auto LuaVM__Load = (_LuaVM__Load)Update::LuaVM__Load;

	using _Task__Defer = uintptr_t(__fastcall*)(lua_State*);
	inline auto Task__Defer = (_Task__Defer)Update::Task__Defer;

	using _GetGlobalStateForInstance = uintptr_t(__fastcall*)(uintptr_t, uintptr_t*, uintptr_t*);
	inline auto GetGlobalStateForInstance = (_GetGlobalStateForInstance)Update::GetGlobalStateForInstance;

	using _DecryptLuaState = uintptr_t(__fastcall*)(uintptr_t);
	inline auto DecryptLuaState = (_DecryptLuaState)Update::DecryptLuaState;

	using _Luau_Execute = void(__fastcall*)(lua_State*);
	inline auto Luau_Execute = (_Luau_Execute)Update::Luau_Execute;

	using _PushInstance = __int64(__fastcall*)(lua_State*, void*);
	inline auto PushInstance = (_PushInstance)Update::PushInstance;

	using _GetProperty = uintptr_t * (__thiscall*)(uintptr_t, uintptr_t*);
	inline auto GetProperty = (_GetProperty)Update::GetProperty;
}