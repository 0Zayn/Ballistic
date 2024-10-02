#pragma once
#include <vector>

#include "../Library.hpp"

#include "Update/Offsets.hpp"

namespace Libraries
{
	class Cache : public Library
	{
	public:
		void Initialize(lua_State*) override;
	};
}