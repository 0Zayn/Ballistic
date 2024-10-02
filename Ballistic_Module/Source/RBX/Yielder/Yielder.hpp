#pragma once
#include <Windows.h>
#include <iostream>
#include <cstdint>
#include <functional>
#include <thread>

#include "xorstr/xorstr.hpp"

#include "Update/Offsets.hpp"
#include "Environment/Library.hpp"

struct lua_State;

using YieldReturn = std::function<int(lua_State* L)>;

namespace RBX
{
	class CYielder
	{
	public:
		int HaltExecution(lua_State*, const std::function<YieldReturn()>&);
	};

	inline auto Yielder = std::make_unique<CYielder>();
}