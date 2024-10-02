#pragma once
#include <iostream>

#include "Globals.hpp"
#include "Scheduler/Scheduler.hpp"

#include "RBX/DataModel/DataModel.hpp"
#include "RBX/TaskScheduler/TaskScheduler.hpp"

class CTeleportHandler
{
public:
    uintptr_t GetPlaceId();

    void Watch();
};

inline auto TeleportHandler = std::make_unique<CTeleportHandler>();