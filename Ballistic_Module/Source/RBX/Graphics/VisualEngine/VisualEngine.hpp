#pragma once
#include <iostream>

#include "../../LogReader/LogReader.hpp"
#include "../RenderView/RenderView.hpp"

namespace RBX
{
	namespace Graphics
	{
		class CVisualEngine
		{
		public:
			uintptr_t Get();
		};

		inline auto VisualEngine = std::make_unique<CVisualEngine>();
	}
}