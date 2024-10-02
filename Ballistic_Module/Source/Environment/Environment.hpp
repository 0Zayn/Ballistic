#pragma once
#include <iostream>
#include <vector>

#include "lstate.h"

#include "Cache/Cache.hpp"
#include "Closure/Closure.hpp"
#include "Crypt/Crypt.hpp"
#include "Debug/Debug.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Http/Http.hpp"
#include "Metatable/Metatable.hpp"
#include "Script/Script.hpp"
#include "Websocket/Websocket.hpp"

#include "Scheduler/Scheduler.hpp"
#include "ResourceManager.hpp"

class CEnvironment
{
public:
	void Initialize(lua_State*);
};

inline auto Environment = std::make_unique<CEnvironment>();