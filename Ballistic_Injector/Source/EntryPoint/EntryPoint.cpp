#include "EntryPoint.hpp"

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    const auto GetProcessId = [&](const char* ProcessName) -> DWORD {
        DWORD Ret = 0;

        HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (Snapshot != INVALID_HANDLE_VALUE)
        {
            PROCESSENTRY32 Entry;
            Entry.dwSize = sizeof(PROCESSENTRY32);

            if (Process32First(Snapshot, &Entry))
            {
                do
                {
                    if (strcmp(Entry.szExeFile, ProcessName) == 0)
                    {
                        Ret = Entry.th32ProcessID;
                        break;
                    }
                } while (Process32Next(Snapshot, &Entry));
            }
            CloseHandle(Snapshot);
        }
        return Ret;
        };

    const auto GetRandomString = [&](std::string::size_type length) -> std::string {
        static auto& Characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

        thread_local static std::mt19937 RandDevice{ std::random_device{}() };
        thread_local static std::uniform_int_distribution<std::string::size_type> Pick(0, sizeof(Characters) - 2);

        std::string Ret;
        Ret.reserve(length);

        while (length--)
            Ret += Characters[Pick(RandDevice)];

        return Ret;
        };

    std::thread([&]() -> void {
        while (true)
        {
            SetConsoleTitle(GetRandomString(16).c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();

    const auto LogIdent = xorstr_("main()");
    
    ActionLogger->Log(ELogLevel::INFO, LogIdent, xorstr_("Waiting for Roblox process..."));

    HWND WindowHandle;
    while (true)
    {
        WindowHandle = FindWindow(nullptr, xorstr_("Roblox"));
        if (IsWindowVisible(WindowHandle))
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    system(xorstr_("cls"));

    std::thread([]() -> void {
        while (true)
        {
            if (!FindWindow(nullptr, xorstr_("Roblox")))
                exit(EXIT_SUCCESS);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();

    auto ProcessId = GetProcessId(xorstr_("RobloxPlayerBeta.exe"));
    const auto ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);

    const auto WintrustModule = LoadLibrary(xorstr_("wintrust.dll"));
    const auto _WinVerifyTrust = GetProcAddress(WintrustModule, xorstr_("WinVerifyTrust"));

    unsigned char Payload[6] = {
        0x48, 0x31, 0xC0,  /* xor rax, rax     ; RAX = 0 */
        0x59,              /* pop rcx          ; RCX = [rsp], rsp += 8 */
        0xFF, 0xE1         /* jmp rcx          ; jump to the address in RCX */
    };

    DWORD OldProtect;
    if (!VirtualProtectEx(ProcessHandle, (void*)_WinVerifyTrust, sizeof(Payload), PAGE_EXECUTE_READWRITE, &OldProtect))
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Failed to protect WinVerifyTrust. (Did you run twice??)"));

    ActionLogger->Log(ELogLevel::INFO, LogIdent, xorstr_("New protection: PAGE_EXECUTE_READWRITE."));

    SIZE_T BytesWritten;
    if (!WriteProcessMemory(ProcessHandle, (void*)_WinVerifyTrust, Payload, sizeof(Payload), &BytesWritten))
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Failed to patch WinVerifyTrust."));

    if (!VirtualProtectEx(ProcessHandle, (void*)_WinVerifyTrust, sizeof(Payload), PAGE_EXECUTE_READ, &OldProtect))
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Failed to protect WinVerifyTrust."));

    const auto ThreadId = GetWindowThreadProcessId(WindowHandle, &ProcessId);
    if (!ThreadId)
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Window thread ID is invalid."));

    const auto TargetModule = LoadLibraryEx(xorstr_("Ballistic_Module.dll"), nullptr, DONT_RESOLVE_DLL_REFERENCES);
    if (!TargetModule)
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Failed to find module."));

    const auto DllExport = (HOOKPROC)GetProcAddress(TargetModule, xorstr_("NextHook"));
    if (!DllExport)
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Failed to find module hook."));

    const auto Handle = SetWindowsHookEx(WH_GETMESSAGE, DllExport, TargetModule, ThreadId);
    if (!Handle)
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Module hook failed."));

    if (!PostThreadMessage(ThreadId, WM_NULL, 0, 0))
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Failed to post thread message."));

    ActionLogger->Log(ELogLevel::SUCCESS, LogIdent, xorstr_("Module attached successfully."));
    while (true) {} /* Stop thread from ending */

    return 0;
}