#include "EntryPoint.hpp"

#pragma region DLL_EXPORTS
extern "C" __declspec(dllexport) LRESULT NextHook(int code, WPARAM wParam, LPARAM lParam) { return CallNextHookEx(nullptr, code, wParam, lParam); }
#pragma endregion

void MainThread(HMODULE DllModule)
{
    RBX::TaskScheduler->Initialize();
    RBX::IdentityManager->SetIdentity(8);

    auto LuaState = (lua_State*)RBX::TaskScheduler->GetLuaState();
    luaL_sandboxthread(LuaState);

    Environment->Initialize(LuaState);
    Scheduler->Initialize();

    Communication->Initialize();
    TeleportHandler->Watch();

    while (true) {}
    FreeLibrary(DllModule);
}

bool __stdcall DllMain(HMODULE DllModule, uintptr_t ReasonForCall, void*)
{
    if (ReasonForCall == DLL_PROCESS_ATTACH)
        std::thread(MainThread, DllModule).detach();

    return true;
}