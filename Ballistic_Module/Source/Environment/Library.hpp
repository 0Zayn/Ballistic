#pragma once
#include "lualib.h"
#include "lmem.h"
#include "lstate.h"
#include "lapi.h"
#include "lgc.h"

#include "xorstr/xorstr.hpp"

#include "Configuration.hpp"

class Library
{
public:
	virtual void Initialize(lua_State*) {};
};