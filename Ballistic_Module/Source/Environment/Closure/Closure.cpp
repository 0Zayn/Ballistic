#include "Closure.hpp"

using namespace Libraries;

std::unordered_map<Closure*, __int64> CClosures;

__forceinline static int NewCClosureStub(lua_State* L)
{
	const auto Nargs = lua_gettop(L);
	int LClosure = CClosures.find(clvalue(L->ci->func))->second;
	if (!LClosure)
		return 0;

	lua_getref(L, LClosure);
	lua_insert(L, 1);

	const char* Error;
	const auto Res = lua_pcall(L, Nargs, LUA_MULTRET, 0);

	if (Res && Res != LUA_YIELD && (Error = lua_tostring(L, -1), !strcmp(Error, xorstr_("attempt to yield across metamethod/C-call boundary"))))
		return lua_yield(L, 0);

	if (Res != LUA_OK)
	{
		luaL_error(L, "%s", lua_tostring(L, -1));
		return 0;
	}
	return lua_gettop(L);
}

__forceinline static int NewCClosure(lua_State* L)
{
	luaL_stackcheck(L, 1, 1, luaL_checktype(L, 1, LUA_TFUNCTION););

	if (lua_iscfunction(L, 1))
	{
		lua_pushvalue(L, 1);
		return 1;
	}

	lua_pushcclosure(L, NewCClosureStub, 0, 0);
	CClosures[&luaA_toobject(L, -1)->value.gc->cl] = lua_ref(L, 1);
	return 1;
};

__forceinline static int IsLClosure(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_pushboolean(L, lua_isLfunction(L, 1));
	return 1;
}

__forceinline static int IsCClosure(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_pushboolean(L, lua_iscfunction(L, 1));
	return 1;
}

__forceinline static int CheckCaller(lua_State* L)
{
	if (lua_gettop(L) > 0)
		return 0;

	const auto ContextLevel = *(uintptr_t*)((uintptr_t)L->userdata + 0x20); /* Acting as extra space */
	lua_pushboolean(L, ContextLevel > 5);
	return 1;
}

__forceinline static int CheckClosure(lua_State* L)
{
	luaL_stackcheck(L, 1, 1, luaL_checktype(L, 1, LUA_TFUNCTION););

	const auto ClosureList = GetClosureList();
	lua_pushboolean(L, (ClosureList.find((Closure*)lua_topointer(L, 1)) != ClosureList.end()));

	return 1;
}

__forceinline static int CloneFunction(lua_State* L)
{
	luaL_stackcheck(L, 1, 1, checkType(L, 1, LUA_TFUNCTION));

	if (lua_iscfunction(L, 1)) {
		Closure* cloneFunc = (Closure*)lua_topointer(L, 1);

		if (CClosures.find(cloneFunc) != CClosures.end()) {
			std::intptr_t closureRef = CClosures.find(cloneFunc)->second;
			lua_pushcclosure(L, NewCClosureStub, 0, 0);
			CClosures[&luaA_toobject(L, -1)->value.gc->cl] = closureRef;

			return 1;
		}
	}

	lua_clonefunction(L, 1);
	return 1;
}

__forceinline static int HookFunction(lua_State* L)
{
	luaL_stackcheck(L, 2, 2, luaL_checktype(L, 1, LUA_TFUNCTION););
	luaL_checktype(L, 2, LUA_TFUNCTION);

	auto Function = lua_toclosure(L, 1);
	auto Hook = lua_toclosure(L, 2);

	if (Function->isC)
	{
		const auto HookRef = lua_ref(L, 2);

		if (!Hook->isC)
		{
			lua_pushcclosure(L, NewCClosureStub, 0, 0);
			CClosures[&luaA_toobject(L, -1)->value.gc->cl] = HookRef;
			Hook = lua_toclosure(L, -1);
			lua_ref(L, -1);
			lua_pop(L, 1);
		}

		lua_CFunction Func1 = Hook->c.f;
		lua_clonefunction(L, 1);
		Function->c.f = [](lua_State* L) -> int { return 0; };

		for (auto i = 0; i < Hook->nupvalues; i++)
		{
			auto OldTValue = &Function->c.upvals[i];
			auto HookTValue = &Hook->c.upvals[i];

			OldTValue->value = HookTValue->value;
			OldTValue->tt = HookTValue->tt;
		}

		auto ClosureRef = CClosures.find(Function)->second;
		if (ClosureRef != 0)
		{
			CClosures[Function] = HookRef;
			CClosures[clvalue(luaA_toobject(L, -1))] = ClosureRef;
		}

		Function->nupvalues = Hook->nupvalues;
		Function->c.f = Func1;

		return 1;
	}
	else
	{
		if (Hook->isC)
		{
			lua_newtable(L);
			lua_newtable(L);

			lua_pushvalue(L, LUA_GLOBALSINDEX);
			lua_setfield(L, -2, xorstr_("__index"));
			lua_setreadonly(L, -1, true);

			lua_setmetatable(L, -2);

			lua_pushvalue(L, 2);
			lua_setfield(L, -2, xorstr_("cFuncCall"));

			static auto Encoder = CBytecodeEncoder();
			const auto Bytecode = Luau::compile(xorstr_("return cFuncCall(...)"), {}, {}, &Encoder);
			luau_load(L, "", Bytecode.c_str(), Bytecode.size(), -1);
			Hook = lua_toclosure(L, -1);
		}

		Proto* nProto = Hook->l.p;
		lua_clonefunction(L, 1);

		Function->env = Hook->env;

		Function->stacksize = Hook->stacksize;
		Function->preload = Hook->preload;

		for (auto i = 0; i < Hook->nupvalues; ++i)
			setobj2n(L, &Function->l.uprefs[i], &Hook->l.uprefs[i]);

		Function->nupvalues = Hook->nupvalues;
		Function->l.p = nProto;

		return 1;
	}
	return 0;
}

__forceinline static std::string GetScriptBytecode__Handler(lua_State* L, bool Decrypt = true, bool UseOffset = false)
{
	const auto DecompressBytecode = [&](std::string_view CompressedInput) -> std::string {
		const uint8_t BytecodeSignature[4] = { 'R', 'S', 'B', '1' }; /* Roblox Secure Bytecode Version 1 */
		const int BytecodeHashMultiplier = 41;
		const int BytecodeHashSeed = 42;

		if (CompressedInput.size() < 8)
			return xorstr_("Compressed data too short");

		std::vector<uint8_t> CompressedData(CompressedInput.begin(), CompressedInput.end());
		std::vector<uint8_t> HeaderBuffer(4);

		for (size_t i = 0; i < 4; ++i)
		{
			HeaderBuffer[i] = CompressedData[i] ^ BytecodeSignature[i];
			HeaderBuffer[i] = (HeaderBuffer[i] - i * BytecodeHashMultiplier) % 256;
		}

		for (size_t i = 0; i < CompressedData.size(); ++i)
			CompressedData[i] ^= (HeaderBuffer[i % 4] + i * BytecodeHashMultiplier) % 256;

		uint32_t HashValue = 0;
		for (size_t i = 0; i < 4; ++i)
			HashValue |= HeaderBuffer[i] << (i * 8);

		uint32_t Rehash = XXH32(CompressedData.data(), CompressedData.size(), BytecodeHashSeed);
		if (Rehash != HashValue)
			return xorstr_("Hash mismatch during decompression");

		uint32_t DecompressedSize = 0;
		for (size_t i = 4; i < 8; ++i)
			DecompressedSize |= CompressedData[i] << ((i - 4) * 8);

		CompressedData = std::vector<uint8_t>(CompressedData.begin() + 8, CompressedData.end());
		std::vector<uint8_t> Result(DecompressedSize);

		size_t const ActualDecompressdSize = ZSTD_decompress(Result.data(), DecompressedSize, CompressedData.data(), CompressedData.size());
		if (ZSTD_isError(ActualDecompressdSize))
			return xorstr_("ZSTD decompression error: ") + std::string(ZSTD_getErrorName(ActualDecompressdSize));

		Result.resize(ActualDecompressdSize);
		return std::string(Result.begin(), Result.end());
	};

	const auto Script = *(uintptr_t*)lua_touserdata(L, 1);
	const auto Instance = *(RBX::Instance**)lua_touserdata(L, 1);
	std::string ClassName = Instance->classDescriptor->name;

	std::string Bytecode = "";

	if (ClassName == xorstr_("LocalScript"))
	{
		const auto BytecodeStrPtr = *(uintptr_t*)(Script + Update::DataModel::Bytecode::LocalScript);
		Bytecode = *(std::string*)(BytecodeStrPtr + 0x10);

		if (Bytecode.size() <= 8)
		{
			Bytecode = "";
			Decrypt = false;
		}
	}
	else if (ClassName == xorstr_("ModuleScript"))
	{
		const auto BytecodeStrPtr = *(uintptr_t*)(Script + Update::DataModel::Bytecode::ModuleScript);
		Bytecode = *(std::string*)(BytecodeStrPtr + 0x10);

		if (Bytecode.size() <= 8)
		{
			Bytecode = "";
			Decrypt = false;
		}
	}
	else if (ClassName == xorstr_("Script"))
	{
		Bytecode = "";
		Decrypt = false;
	}

	if (Decrypt)
		Bytecode = DecompressBytecode(Bytecode);

	return Bytecode;
}

__forceinline static int GetScriptBytecode(lua_State* L)
{
	luaL_stackcheck(L, 1, 1);

	const auto Bytecode = GetScriptBytecode__Handler(L);
	if (Bytecode.empty())
	{
		luaL_error(L, xorstr_("invalid bytecode format"));
		return 0;
	}

	lua_pushlstring(L, Bytecode.c_str(), Bytecode.size());
	return 1;
}

__forceinline static int Loadstring(lua_State* L)
{
	std::string	Source = lua_tostring(L, 1);
	const std::string Chunk = luaL_optstring(L, 2, "=");

	if (Source.empty() || lua_type(L, 1) != LUA_TSTRING)
		return 0;

	static auto Encoder = CBytecodeEncoder();
	const std::string& Bytecode = Luau::compile(Source, { 2, 1, 2 }, { true, true }, &Encoder);
	if (Bytecode[0] != '\0')
	{
		if (luau_load(L, Chunk.c_str(), Bytecode.c_str(), Bytecode.size(), 0))
		{
			lua_pushnil(L);
			lua_pushstring(L, lua_tostring(L, -1));
			return 2;
		}

		Execution->SetCapabilities(((Closure*)lua_topointer(L, -1))->l.p, &MaxCapabilities);
		return 1;
	}

	lua_pushnil(L);
	lua_pushstring(L, Bytecode.c_str());
	return 2;
}

static const luaL_Reg LibraryFunctions[] = {
	{"newcclosure", NewCClosure}, {"new_cclosure", NewCClosure}, {"NewCClosure", NewCClosure},

	{"islclosure", IsLClosure}, {"is_lclosure", IsLClosure}, {"IsLClosure", IsLClosure},

	{"iscclosure", IsCClosure}, {"is_cclosure", IsCClosure}, {"IsCClosure", IsCClosure},

	{"checkcaller", CheckCaller}, {"check_caller", CheckCaller}, {"CheckCaller", CheckCaller},

	{"checkclosure", CheckClosure}, {"check_closure", CheckClosure}, {"CheckClosure", CheckClosure},
	{"isexecutorclosure", CheckClosure}, {"is_executor_closure", CheckClosure}, {"IsExecutorClosure", CheckClosure},
	{"isourclosure", CheckClosure}, {"is_our_closure", CheckClosure}, {"IsOurClosure", CheckClosure},

	{"clonefunction", CloneFunction}, {"clone_function", CloneFunction}, {"CloneFunction", CloneFunction},
	
	{"hookfunction", HookFunction}, {"hook_function", HookFunction}, {"HookFunction", HookFunction},
	{"replaceclosure", HookFunction}, {"replace_closure", HookFunction}, {"ReplaceClosure", HookFunction},

	{"getscriptbytecode", GetScriptBytecode}, {"get_script_bytecode", GetScriptBytecode}, {"GetScriptBytecode", GetScriptBytecode},
	{"dumpstring", GetScriptBytecode}, {"dump_string", GetScriptBytecode}, {"DumpString", GetScriptBytecode},
	
	{"loadstring", Loadstring}, {"Loadstring", Loadstring},
	
	{nullptr, nullptr}
};

void ClosureLib::Initialize(lua_State* L)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, LibraryFunctions);
	lua_pop(L, 1);
}