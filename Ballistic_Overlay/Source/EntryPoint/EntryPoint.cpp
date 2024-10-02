#include "EntryPoint.hpp"

int main()
{
    SetConsoleTitle(std::format("{} Overlay", Configuration::Name).c_str());

    const auto LogIdent = xorstr_("main()");

    if (!FindWindow(nullptr, xorstr_("Roblox")))
        ActionLogger->Log(ELogLevel::ERR, LogIdent, xorstr_("Failed to find Roblox process."));

    std::thread([&]() -> void {
        while (true)
        {
            if (!FindWindow(nullptr, xorstr_("Roblox")))
                exit(EXIT_SUCCESS);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        }).detach();

    ActionLogger->Log(ELogLevel::INFO, LogIdent, xorstr_("Press END to toggle overlay."));
    SetForegroundWindow(FindWindow(nullptr, xorstr_("Roblox")));
    OverlayHook->Initialize();

    while (true) {}
    return 0;
}