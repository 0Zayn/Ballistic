#include "TeleportHandler.hpp"

/* TODO: Improve this. */

uintptr_t CTeleportHandler::GetPlaceId()
{
    const auto DataModel = RBX::DataModel->Get();
    if (DataModel == 0)
        return 0;

    const auto PlaceIdPtr = (uintptr_t*)(DataModel + Update::DataModel::PlaceId);
    return (PlaceIdPtr == nullptr || IsBadReadPtr(PlaceIdPtr, sizeof(uintptr_t))) ? 0 : *PlaceIdPtr;
}

void CTeleportHandler::Watch()
{
    uintptr_t CurrentPlaceId = 0, LastPlaceId = this->GetPlaceId();

    std::thread([&, LastPlaceId]() mutable -> void {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            CurrentPlaceId = this->GetPlaceId();
            if (CurrentPlaceId > 0 && !LastPlaceId)
            {
                std::this_thread::sleep_for(std::chrono::seconds(3)); /* OnGameLoaded and OnGameLeave events are inlined now, therefore the delay is hardcoded temporarily. */

                RBX::TaskScheduler->Jobs.clear();
                RBX::TaskScheduler->LuaState = 0;

                RBX::TaskScheduler->Initialize();
                RBX::IdentityManager->SetIdentity(8);

                Execution->ResetExecutionThreadNextCall = true;

                Environment->Initialize((lua_State*)RBX::TaskScheduler->GetLuaState());
                Globals.SchedulerVF = 0;

                Scheduler->Initialize();
            }

            if (CurrentPlaceId > 0 || LastPlaceId > 0)
                LastPlaceId = CurrentPlaceId;
        }
    }).detach();
}