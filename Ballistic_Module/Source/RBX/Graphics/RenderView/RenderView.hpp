#pragma once
#include <iostream>

#include "../../LogReader/LogReader.hpp"

namespace RBX
{
	namespace Graphics
	{
		class CRenderView
		{
		public:
			uintptr_t Get();
		};

		inline auto RenderView = std::make_unique<CRenderView>();
	}
}