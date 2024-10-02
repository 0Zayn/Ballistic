#include "TaskScheduler.hpp"

using namespace RBX;

void CTaskScheduler::Initialize()
{
	this->Address = RBX::GetTaskScheduler();

	auto JobsStart = *(uintptr_t*)(this->Address + Update::TaskScheduler::JobsStart);
	const auto JobsEnd = *(uintptr_t*)(this->Address + Update::TaskScheduler::JobsEnd);

	for (auto i = JobsStart; i < JobsEnd; i += 0x10)
		this->Jobs.push_back(*(uintptr_t*)i);
}

uintptr_t CTaskScheduler::GetJobByName(std::string_view JobName)
{
	for (const auto i : this->Jobs)
	{
		if (*(std::string*)(i + Update::TaskScheduler::JobName) == JobName)
			return i;
	}
}

uintptr_t CTaskScheduler::GetLuaState()
{
	if (!this->LuaState)
	{
		uintptr_t Identity = 0, Script = 0;
		const auto EncryptedState = RBX::GetGlobalStateForInstance(*(uintptr_t*)(this->GetJobByName("WaitingHybridScriptsJob") + Update::TaskScheduler::ScriptContextJob) + Update::TaskScheduler::ScriptContextState, &Identity, &Script);
		const auto DecryptedLuaState = (uintptr_t)lua_newthread((lua_State*)RBX::DecryptLuaState(EncryptedState + Update::TaskScheduler::DecryptState));

		this->LuaState = DecryptedLuaState;
	}
	return this->LuaState;
}