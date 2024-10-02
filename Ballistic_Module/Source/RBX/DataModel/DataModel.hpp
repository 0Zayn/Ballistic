#pragma once
#include <iostream>

#include "../Graphics/Graphics.hpp"

namespace RBX
{
	class CDataModel
	{
	public:
		uintptr_t Get();
	};

	inline auto DataModel = std::make_unique<CDataModel>();
}