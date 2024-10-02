#pragma once
#include <filesystem>
#include <Shlobj.h>
#include <iostream>
#include <fstream>
#include <string>

#include "xorstr/xorstr.hpp"

namespace RBX
{
	class CLogReader
	{
	public:
		std::string ReadFile(const std::string& FilePath);
		std::filesystem::path FindLatestLog(const std::string& LogLocation);
		std::string GetLogInfo();
		uintptr_t ExtractPointer(const std::string& Line);
	};

	inline auto LogReader = std::make_unique<CLogReader>();
}