#pragma once
#include <Windows.h>
#include <string_view>
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>

#include "xorstr/xorstr.hpp"
#include "lstate.h"

#include "Update/Offsets.hpp"
#include "../Graphics/Graphics.hpp"
#include "../DataModel/DataModel.hpp"

namespace RBX
{
	class CTaskScheduler
	{
	private:
		uintptr_t Address = 0;
	public:
		uintptr_t LuaState = 0;
		std::vector<uintptr_t> Jobs{};

		void Initialize();

		uintptr_t GetJobByName(std::string_view);
		uintptr_t GetLuaState();
	};

	inline auto TaskScheduler = std::make_unique<CTaskScheduler>();
}