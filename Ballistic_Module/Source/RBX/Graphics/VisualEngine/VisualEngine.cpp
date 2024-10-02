#include "VisualEngine.hpp"

using namespace RBX::Graphics;

uintptr_t CVisualEngine::Get()
{
    return *(uintptr_t*)(RenderView->Get() + 0x10);
}