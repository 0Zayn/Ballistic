#include "Environment.hpp"

__forceinline HMODULE GetResourceModule()
{
    HMODULE ResourceModule = nullptr;
    GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        (LPCSTR)GetResourceModule,
        &ResourceModule
    );

    return ResourceModule;
}

__forceinline const auto RunScript(int ScriptKey, const std::string& TargetTerm = "", const std::string& Replacement = "")
{
    if (!ScriptKey)
        return;

    const auto Resource = FindResource(GetResourceModule(), MAKEINTRESOURCE(ScriptKey), MAKEINTRESOURCE(TEXTFILE));
    const auto ResourceData = LoadResource(GetResourceModule(), Resource);
    const auto ResourceSize = SizeofResource(GetResourceModule(), Resource);

    std::string Script;
    Script.assign((char*)LockResource(ResourceData), ResourceSize);

    if (!TargetTerm.empty() && !Replacement.empty())
        Script.replace(Script.find(TargetTerm), std::string(TargetTerm).size(), Replacement);

    Scheduler->AddScript(Script);
}

void CEnvironment::Initialize(lua_State* L)
{
    static const auto TargetTerm = "%EXPLOIT_NAME%";

    
    std::vector<Library*> LibraryContainer;
    LibraryContainer.push_back(new Libraries::Script());
    LibraryContainer.push_back(new Libraries::Cache());
    LibraryContainer.push_back(new Libraries::ClosureLib());
    LibraryContainer.push_back(new Libraries::Crypt());
    LibraryContainer.push_back(new Libraries::Debug());
    LibraryContainer.push_back(new Libraries::FileSystem());
    LibraryContainer.push_back(new Libraries::Http());
    LibraryContainer.push_back(new Libraries::Metatable());
    /* LibraryContainer.push_back(new Libraries::Websocket()); */

    for (auto& Lib : LibraryContainer)
        Lib->Initialize(L);

    lua_newtable(L);
    lua_setglobal(L, "_G");

    lua_newtable(L);
    lua_setglobal(L, "shared");

    
    RunScript(ENVIRONMENT_SCRIPT, TargetTerm, Configuration::Name);
    RunScript(NAMECALL_HOOK_SCRIPT);
    RunScript(SECURITY_SCRIPT);

    RunScript(INIT_SCRIPT, TargetTerm, Configuration::Name);
}