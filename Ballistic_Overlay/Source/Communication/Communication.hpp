#pragma once
#include <iostream>

#include "nlohmann/nlohmann.hpp"
#include "cpr/cpr.h"

#include "../Configuration.hpp"

class CCommunication
{
public:
	void ExecuteScript(const std::string&);
};

inline auto Communication = std::make_unique<CCommunication>();