#pragma once
#include "../Library.hpp"

#include "lfunc.h"

#include "Execution/Execution.hpp"

namespace Libraries
{
	class ClosureLib : public Library
	{
	public:
		void Initialize(lua_State*) override;
	};
}