#pragma once
#include "../Library.hpp"

#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"

class CWebsocket
{
public:
    lua_State* Thread = nullptr;
    bool Connected = false;
    ix::WebSocket* WebSocket;
    int OnMessageRef;
    int OnCloseRef;
    int ThreadRef;

    void FireMessage(const std::string&);

    void FireClose();

    int HandleIndex(lua_State* L);

    int HandleNewIndex(lua_State* L);
};

namespace Libraries
{
	class Websocket : public Library
	{
	public:
		void Initialize(lua_State*) override;
	};
}