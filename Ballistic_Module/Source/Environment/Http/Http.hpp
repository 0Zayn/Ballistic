#pragma once
#include "../Library.hpp"

#include "cpr/cpr.h"
#include "HttpStatus/HttpStatus.hpp"

#include "FNV.hpp"

#include "RBX/RBX.hpp"

namespace Libraries
{
	class Http : public Library
	{
	public:
		void Initialize(lua_State*) override;
	};
}