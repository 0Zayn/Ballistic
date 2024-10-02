#include "Scheduler.hpp"

static VTFunctionType OldSchedulerVF = nullptr;
std::condition_variable CScheduler::QueueCV = {};
std::queue<std::string> CScheduler::ScriptQueue = {};


uintptr_t __fastcall CScheduler::SchedulerHook(uintptr_t a1, uintptr_t a2, uintptr_t a3)
{
	if (!Globals.SchedulerVF)
		OldSchedulerVF = nullptr;

	if (!ScriptQueue.empty())
	{
		Execution->Send(ScriptQueue.front().c_str());
		ScriptQueue.pop();
	}

	return OldSchedulerVF != nullptr ? OldSchedulerVF(a1, a2, a3) : 0;
}

void CScheduler::Initialize()
{
	const auto LuaGcJob = RBX::TaskScheduler->GetJobByName(xorstr_("LuaGc"));
	if (!LuaGcJob)
	{
		RBX::Print(RBX_PRINT_ERROR, xorstr_("Failed to get job \"LuaGc\" from RBX::TaskScheduler."));
		return;
	}

	auto VTable = new void* [25];
	memcpy(VTable, *(void**)LuaGcJob, sizeof(uintptr_t) * 25);
	OldSchedulerVF = (VTFunctionType)(VTable[2]);
	VTable[2] = SchedulerHook;
	*(void**)LuaGcJob = VTable;
}

void CScheduler::AddScript(const std::string& Script) 
{
	{
		std::lock_guard<std::mutex> lock(Pending);
		ScriptQueue.push(Script);
	}
}