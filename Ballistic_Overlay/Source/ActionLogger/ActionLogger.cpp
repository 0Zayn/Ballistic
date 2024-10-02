#include "ActionLogger.hpp"

std::tm* CActionLogger::GetLocalTime()
{
    using namespace std::chrono;

    const auto LocalTime = system_clock::to_time_t(system_clock::now());
    return localtime(&LocalTime);
}

std::string CActionLogger::TimeToString(std::tm* Time, const char* Format)
{
    std::ostringstream OutStream;
    OutStream << std::put_time(Time, Format);
    return OutStream.str();
}