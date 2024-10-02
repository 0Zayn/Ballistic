#pragma once
#include <iostream>
#include <thread>

#include "nlohmann/nlohmann.hpp"
#include "httplib/httplib.hpp"
#include "cpr/cpr.h"

#include "xorstr/xorstr.hpp"

#include "Scheduler/Scheduler.hpp"
#include "Configuration.hpp"

class CCommunication
{
private:
	httplib::Server Server;
public:
	void Initialize();
};

inline auto Communication = std::make_unique<CCommunication>();