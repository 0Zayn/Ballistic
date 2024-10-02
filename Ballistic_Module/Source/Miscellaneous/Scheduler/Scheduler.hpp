#pragma once
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "xorstr/xorstr.hpp"

#include "Execution/Execution.hpp"
#include "Globals.hpp"

using VTFunctionType = uintptr_t(__fastcall*)(uintptr_t, uintptr_t, uintptr_t);

class CScheduler
{
public:
	std::mutex Pending;

	static std::condition_variable QueueCV;
	static std::queue<std::string> ScriptQueue;

	bool Running = true;

	static uintptr_t __fastcall SchedulerHook(uintptr_t a1, uintptr_t a2, uintptr_t a3);

	void Initialize();
	void AddScript(const std::string& Script);
};

inline auto Scheduler = std::make_unique<CScheduler>();