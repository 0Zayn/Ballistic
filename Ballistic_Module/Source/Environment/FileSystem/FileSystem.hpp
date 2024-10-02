#pragma once
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "../Library.hpp"

#include "Execution/Execution.hpp"

namespace Libraries
{
	class FileSystem : public Library
	{
	public:
		void Initialize(lua_State*) override;
	};
}