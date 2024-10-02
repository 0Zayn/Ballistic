#pragma once
#include <iostream>

#include "../TaskScheduler/TaskScheduler.hpp"

namespace RBX
{
	class CIdentityManager
	{
	public:
		int GetIdentity();
		void SetIdentity(int);
	};

	inline auto IdentityManager = std::make_unique<CIdentityManager>();
}