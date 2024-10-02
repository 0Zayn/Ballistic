#include "Yielder.hpp"

using namespace RBX;

void ThreadFunc(const std::function<YieldReturn()>& YieldedFunction, lua_State* L)
{
	YieldReturn ReturnedFunc;

	try
	{
		ReturnedFunc = YieldedFunction();
	}
	catch (std::exception Exception)
	{
		RBX::Print(RBX_PRINT_ERROR, Exception.what());
		return;
	}

	auto Results = ReturnedFunc(L);

	auto Thread = lua_newthread(L);
	lua_getglobal(Thread, ("task"));
	lua_getfield(Thread, -1, ("defer"));

	lua_pushthread(L);
	lua_xmove(L, Thread, 1);
	lua_pop(L, 1);

	for (auto i = Results; i >= 1; i--)
	{
		lua_pushvalue(L, -i);
		lua_xmove(L, Thread, 1);
	}

	lua_pcall(Thread, Results + 1, 0, 0);
	lua_settop(Thread, 0);
}

int CYielder::HaltExecution(lua_State* L, const std::function<YieldReturn()>& YieldedFunction)
{
	lua_pushthread(L);
	lua_ref(L, -1);
	lua_pop(L, 1);

	std::thread(ThreadFunc, YieldedFunction, L).detach();

	L->base = L->top;
	L->status = LUA_YIELD;

	L->ci->flags |= 1;
	return -1;
}