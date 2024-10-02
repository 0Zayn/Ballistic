#include "Execution.hpp"

void CExecution::SetCapabilities(Proto* Proto, uintptr_t* CapFlags)
{
    if (!Proto)
        return;

    Proto->userdata = CapFlags;
    for (int i = 0; i < Proto->sizep; ++i)
        SetCapabilities(Proto->p[i], CapFlags);
}

void CExecution::Send(std::string Source)
{
    if (Source.empty())
        return;
    
    if (this->ResetExecutionThreadNextCall)
    {
        ExecutionThread = nullptr;
        this->ResetExecutionThreadNextCall = false;
    }

    if (!ExecutionThread)
    {
        static const auto MainThread = (lua_State*)RBX::TaskScheduler->GetLuaState();
        ExecutionThread = lua_newthread(MainThread);
    }

    static auto Encoder = CBytecodeEncoder();
    auto Bytecode = std::async(std::launch::async, [Source]() -> std::string {
        return Luau::compile(Source, { 2, 1, 2 }, { true, true }, &Encoder);
    }).get();

    if (Bytecode[0] != '\0')
    {
        lua_getglobal(ExecutionThread, xorstr_("Instance"));
        lua_getfield(ExecutionThread, -1, xorstr_("new"));
        lua_pushstring(ExecutionThread, xorstr_("LocalScript"));
        lua_pcall(ExecutionThread, 1, 1, 0);

        DummyScript = (uintptr_t)lua_touserdata(ExecutionThread, -1);
        lua_ref(ExecutionThread, -1); /* Stops dummy script from being added to garbage collector */

        lua_setglobal(ExecutionThread, xorstr_("script"));
        lua_pop(ExecutionThread, 1);

        const auto ExtraSpace = (uintptr_t)ExecutionThread->userdata;
        *(__int64*)(ExtraSpace + Update::Userdata::Identity) = 8;
        *(__int64*)(ExtraSpace + Update::Userdata::Capabilities) = MaxCapabilities;

        /* luaL_sandboxthread(ExecutionThread); */

        lua_getglobal(ExecutionThread, xorstr_("task"));
        lua_getfield(ExecutionThread, -1, xorstr_("defer"));

        if (luau_load(ExecutionThread, "@", Bytecode.c_str(), Bytecode.size(), 0) != LUA_OK)
        {
            const auto Error = lua_tostring(ExecutionThread, -1);
            lua_getglobal(ExecutionThread, xorstr_("error")); /* Could maybe switch this to warn to prevent bans in games like Arsenal that use error detection */
            lua_pushstring(ExecutionThread, Error);
            lua_call(ExecutionThread, 1, 0);
            return;
        }

        this->SetCapabilities(((Closure*)lua_topointer(ExecutionThread, -1))->l.p, &MaxCapabilities);

        if (lua_pcall(ExecutionThread, 1, 0, 0) != LUA_OK)
        {
            const auto Error = lua_tostring(ExecutionThread, -1);
            lua_getglobal(ExecutionThread, xorstr_("error")); /* Could maybe switch this to warn to prevent bans in games like Arsenal that use error detection */
            lua_pushstring(ExecutionThread, Error);
            lua_call(ExecutionThread, 1, 0);
            return;
        }
        return;
    }

    lua_getglobal(ExecutionThread, xorstr_("error")); /* Could maybe switch this to warn to prevent bans in games like Arsenal that use error detection */
    lua_pushstring(ExecutionThread, Bytecode.c_str() + 1);
    lua_call(ExecutionThread, 1, 0);
}