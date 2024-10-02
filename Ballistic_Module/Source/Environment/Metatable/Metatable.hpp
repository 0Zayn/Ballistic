#pragma once
#include <Windows.h>

#include "../Library.hpp"

namespace Libraries
{
	class Metatable : public Library
	{
	public:
		void Initialize(lua_State*) override;
	};
}