#include "DataModel.hpp"

using namespace RBX;

uintptr_t CDataModel::Get()
{
	return *(uintptr_t*)(Graphics::RenderView->Get() + 0x118) + 0x190;
}