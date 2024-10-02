#include "LogReader.hpp"

using namespace RBX;

std::string CLogReader::ReadFile(const std::string& FilePath)
{
    std::ifstream File(FilePath);
    std::string RelevantLine;

    if (File.is_open())
    {
        std::string Line;
        while (std::getline(File, Line))
        {
            if (Line.find(xorstr_("SurfaceController[_:1]::initialize view")) != std::string::npos)
                RelevantLine = Line;
        }
        File.close();
    }
    return RelevantLine;
}

std::filesystem::path CLogReader::FindLatestLog(const std::string& LogLocation)
{
    std::filesystem::path LatestLogPath;
    std::filesystem::file_time_type LatestTime;

    for (const auto& entry : std::filesystem::directory_iterator(LogLocation))
    {
        if (std::filesystem::is_regular_file(entry.path()))
        {
            auto LastWriteTime = std::filesystem::last_write_time(entry.path());
            if (LastWriteTime > LatestTime)
            {
                LatestTime = LastWriteTime;
                LatestLogPath = entry.path();
            }
        }
    }
    return LatestLogPath;
}

std::string CLogReader::GetLogInfo()
{
    PWSTR AppData = nullptr;
    if (SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &AppData) != S_OK)
        return "";

    char Destination[MAX_PATH];
    wcstombs(Destination, AppData, MAX_PATH);
    CoTaskMemFree(AppData);

    auto LatestLog = this->FindLatestLog(std::string(Destination) + xorstr_("\\Roblox\\logs"));
    if (LatestLog.empty())
        return "";

    return LatestLog.string();
}

uintptr_t CLogReader::ExtractPointer(const std::string& Line)
{
    const auto StartPos = Line.find('(');
    if (StartPos == std::string::npos)
        return 0;

    const auto EndPos = Line.find(')', StartPos);
    if (EndPos == std::string::npos)
        return 0;

    std::string HexString = Line.substr(StartPos + 1, EndPos - StartPos - 1);
    std::string CleanedHexString = "0x" + HexString;

    std::stringstream Stream;
    Stream << std::hex << CleanedHexString;
    uintptr_t Ptr;
    Stream >> Ptr;

    return Ptr;
}