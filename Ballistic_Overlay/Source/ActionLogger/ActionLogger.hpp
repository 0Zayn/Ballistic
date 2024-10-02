#pragma once
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

enum class ELogLevel
{
	SUCCESS,
	INFO,
	ERR
};

class CActionLogger
{
public:
	std::tm* GetLocalTime();
	std::string TimeToString(std::tm* Time, const char* Format);

	template<typename... Ty>
	void Log(ELogLevel Level, const char* Source, const char* Format, const auto&... Args)
	{
		static const auto ConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		std::string LogString = "";
		switch (Level)
		{
		case ELogLevel::SUCCESS:
			SetConsoleTextAttribute(ConsoleOutputHandle, 10);
			LogString = "SUCCESS";
			break;
		case ELogLevel::INFO:
			SetConsoleTextAttribute(ConsoleOutputHandle, 14);
			LogString = "INFO";
			break;
		case ELogLevel::ERR:
			SetConsoleTextAttribute(ConsoleOutputHandle, 12);
			LogString = "ERROR";
			break;
		default:
			throw new std::runtime_error("Log level not implemented.");
			break;
		}

		std::ostringstream OutStream;
		OutStream << this->TimeToString(this->GetLocalTime(), "%T") << " [" << LogString << "] " << Format << " (" << Source << ")\n";

		printf(OutStream.str().c_str(), Args...);
		SetConsoleTextAttribute(ConsoleOutputHandle, 15);

		if (Level == ELogLevel::ERR)
		{
			ShowWindow(GetConsoleWindow(), SW_SHOW);
			std::cin.get();
			exit(EXIT_FAILURE);
		}
	}
};

inline auto ActionLogger = std::make_unique<CActionLogger>();