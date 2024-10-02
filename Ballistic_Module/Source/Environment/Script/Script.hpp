#pragma once
#include "../Library.hpp"

#include "RBX/RBX.hpp"

namespace Libraries
{
	class Script : public Library
	{
	public:
		void Initialize(lua_State*) override;
	};
}