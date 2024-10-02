#include "FileSystem.hpp"

using namespace Libraries;

__forceinline static std::filesystem::path GetWorkspace()
{
    const auto WorkspacePath = std::format("{}\\{}\\Workspace", getenv("localappdata"), Configuration::Name);
    std::filesystem::create_directories(WorkspacePath);

    return WorkspacePath;
}

__forceinline std::filesystem::path Sanitize(const std::string& Path)
{
    const auto Workspace = GetWorkspace();
    const auto FullPath = Workspace / Path;
    const auto CanonicalPath = std::filesystem::weakly_canonical(FullPath);
    if (!CanonicalPath.string().starts_with(Workspace.string()))
        return std::filesystem::path();

    return CanonicalPath;
}

__forceinline static int ReadFile(lua_State* L)
{
    const auto FilePath = Sanitize(lua_tostring(L, 1));
    const auto Result = std::async(std::launch::async, [FilePath]() -> std::string {
        std::ifstream FileStream(FilePath, std::ios_base::binary);
        std::string Content;

        if (FileStream.is_open())
        {
            Content.assign((std::istreambuf_iterator<char>(FileStream)), std::istreambuf_iterator<char>());
            FileStream.close();
            return Content;
        }
    }).get();

    if (Result.empty())
        return 0;

    lua_pushstring(L, Result.c_str());
    return 1;
}

__forceinline static int ListFiles(lua_State* L)
{
    luaL_stackcheck(L, 1, 1);
    luaL_checktype(L, 1, LUA_TSTRING);

    const auto Path = Sanitize(lua_tostring(L, 1)).string();
    if (Path.find("..") != std::string::npos)
    {
        luaL_error(L, xorstr_("attempt to escape directory"));
        return 0;
    }

    if (!std::filesystem::is_directory(Path.c_str()))
    {
        luaL_error(L, xorstr_("directory does not exist"));
        return 0;
    }

    const auto Files = std::async(std::launch::async, [Path]() -> std::vector<std::string> {
        std::vector<std::string> Ret;
        for (const auto& Entry : std::filesystem::directory_iterator(Path))
            Ret.push_back(Entry.path().string());

        return Ret;
    }).get();

    lua_newtable(L);

    int Index = 0;
    for (const auto& FilePath : Files)
    {
        lua_pushinteger(L, ++Index);
        lua_pushstring(L, FilePath.c_str());
        lua_settable(L, -3);
    }

    return 1;
}

__forceinline static int WriteFile(lua_State* L)
{
    const auto FilePath = Sanitize(lua_tostring(L, 1));
    const auto Data = lua_tostring(L, 2);

    std::async(std::launch::async, [FilePath, Data, L]() {
        std::ofstream FileStream;

        FileStream.open(FilePath, std::ios::out | std::ios::binary);
        FileStream << Data;
        FileStream.close();
    }).get();

    return 0;
}

__forceinline static int MakeFolder(lua_State* L)
{
    const auto FolderPath = Sanitize(lua_tostring(L, 1));
    std::async(std::launch::async, [FolderPath]() {
        std::filesystem::create_directories(FolderPath);
    }).get();

    return 0;
}

__forceinline static int AppendFile(lua_State* L)
{
    const auto FilePath = Sanitize(lua_tostring(L, 1));
    const std::string Data = lua_tostring(L, 2);

    std::async(std::launch::async, [FilePath, Data, L]() {
        std::ofstream FileStream(FilePath, std::ios::binary | std::ios::app);
        if (!FileStream)
            return false;

        FileStream.write(Data.c_str(), Data.size());
    }).get();

    return 0;
}

__forceinline static int IsFile(lua_State* L)
{
    const auto FilePath = Sanitize(lua_tostring(L, 1));
    const auto Result = std::async(std::launch::async, [FilePath]() {
        return std::filesystem::is_regular_file(FilePath);
    }).get();

    lua_pushboolean(L, Result);
    return 1;
}

__forceinline static int IsFolder(lua_State* L)
{
    const auto FolderPath = Sanitize(lua_tostring(L, 1));
    const auto Result = std::async(std::launch::async, [FolderPath]() {
        return std::filesystem::is_directory(FolderPath);
    }).get();

    lua_pushboolean(L, Result);
    return 1;
}

__forceinline static int DelFile(lua_State* L)
{
    const auto FilePath = Sanitize(lua_tostring(L, 1));
    std::async(std::launch::async, [FilePath]() {
        std::filesystem::remove(FilePath);
    }).get();

    return 0;
}

__forceinline static int DelFolder(lua_State* L)
{
    const auto FolderPath = Sanitize(lua_tostring(L, 1));
    std::async(std::launch::async, [FolderPath]() {
        std::filesystem::remove_all(FolderPath);
    }).get();

    return 0;
}

__forceinline static int LoadFile(lua_State* L)
{
    const auto FilePath = Sanitize(lua_tostring(L, 1));
    std::ifstream FileStream(FilePath, std::ios::binary);
    if (!FileStream)
        return 2;

    std::string Source = std::string((std::istreambuf_iterator<char>(FileStream)), std::istreambuf_iterator<char>());
    if (Source.empty() || lua_type(L, 1) != LUA_TSTRING)
        return 0;

    static auto Encoder = CBytecodeEncoder();
    const std::string& Bytecode = Luau::compile(Source, { 2, 1, 2 }, { true, true }, &Encoder);
    if (Bytecode[0] != '\0')
    {
        if (luau_load(L, "@", Bytecode.c_str(), Bytecode.size(), 0))
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

__forceinline static int GetCustomAsset(lua_State* L)
{
    luaL_stackcheck(L, 1, 1, luaL_checktype(L, 1, LUA_TSTRING););
    
    const std::string FileName = lua_tostring(L, 1);

    const auto FilePath = GetWorkspace() / FileName;
    const auto AssetPath = std::filesystem::current_path() / xorstr_("content") / FilePath.filename();
    
    const size_t ContentSize = std::filesystem::file_size(FilePath);
    std::string Result(ContentSize, '\0');
    
    const auto Future = std::async(std::launch::async, [FileName, FilePath, AssetPath, ContentSize, Result]() -> std::filesystem::path {

        std::ifstream FileStream(FilePath, std::ios::in | std::ios::binary);
        FileStream.read((char*)Result.data(), ContentSize);

        return AssetPath;
    }).get();
    
    std::ofstream Out(AssetPath, std::ios::out | std::ios::binary);
    Out.write(Result.c_str(), Result.size());
    Out.close();
    
    lua_pushstring(L, std::format("rbxasset://{}", Future.filename().string()).c_str());
    return 1;
}

static const luaL_Reg LibraryFunctions[] = {
    {"readfile", ReadFile}, {"read_file", ReadFile}, {"ReadFile", ReadFile},

    {"writefile", WriteFile}, {"write_file", WriteFile}, {"WriteFile", WriteFile},

    {"appendfile", AppendFile}, {"append_file", AppendFile}, {"AppendFile", AppendFile},

    {"loadfile", LoadFile}, {"load_file", LoadFile}, {"LoadFile", LoadFile},

    {"listfiles", ListFiles}, {"list_files", ListFiles}, {"ListFiles", ListFiles},

    {"isfolder", IsFolder}, {"is_folder", IsFolder}, {"IsFolder", IsFolder},

    {"isfile", IsFile}, {"is_file", IsFile}, {"IsFile", IsFile},

    {"makefolder", MakeFolder}, {"make_folder", MakeFolder}, {"MakeFolder", MakeFolder},

    {"delfolder", DelFolder}, {"del_folder", DelFolder}, {"DelFolder", DelFolder},

    {"delfile", DelFile}, {"del_file", DelFile}, {"DelFile", DelFile},

    {"getcustomasset", GetCustomAsset}, {"get_custom_asset", GetCustomAsset}, {"GetCustomAsset", GetCustomAsset},

    {nullptr, nullptr}
};

void FileSystem::Initialize(lua_State* L)
{
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, nullptr, LibraryFunctions);
    lua_pop(L, 1);
}