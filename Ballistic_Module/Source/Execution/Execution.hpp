#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <format>
#include <string>
#include <random>
#include <stack>
#include <mutex>
#include <queue>

#include "Luau/Compiler.h"
#include "Luau/BytecodeBuilder.h"
#include "Luau/BytecodeUtils.h"
#include "Luau/Bytecode.h"
#include "lstate.h"
#include "zstd/zstd.h"
#include "zstd/xxhash.h"

#include "RBX/RBX.hpp"
#include "Environment/Environment.hpp"
#include "Configuration.hpp"
#include "Update/Offsets.hpp"

static lua_State* ExecutionThread = nullptr;

static uintptr_t MaxCapabilities = 0xEFFFFFFFFFFFFFFF;
static uintptr_t DummyScript = 0;

class CBytecodeEncoder : public Luau::BytecodeEncoder
{
    inline void encode(uint32_t* Data, size_t Count) override
    {
        for (auto i = 0u; i < Count;)
        {
            auto& Opcode = *(uint8_t*)(Data + i);
            i += Luau::getOpLength(LuauOpcode(Opcode));
            Opcode *= 227;
        }
    }
};

class CExecution
{
public:
    bool ResetExecutionThreadNextCall = false;

    void SetCapabilities(Proto*, uintptr_t*);
	void Send(std::string);
};

inline auto Execution = std::make_unique<CExecution>();