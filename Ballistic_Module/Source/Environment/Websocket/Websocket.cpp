#include "Websocket.hpp"

using namespace Libraries;

void CWebsocket::FireMessage(const std::string& Message)
{
    if (!this->Connected)
        return;

    lua_getref(this->Thread, this->OnMessageRef);
    lua_getfield(this->Thread, -1, xorstr_("Fire"));
    lua_getref(this->Thread, this->OnMessageRef);
    lua_pushlstring(this->Thread, Message.c_str(), Message.size());
    lua_pcall(this->Thread, 2, 0, 0);
    lua_settop(this->Thread, 0);
}

void CWebsocket::FireClose()
{
    if (!this->Connected)
        return;

    this->Connected = false;

    lua_getref(this->Thread, this->OnCloseRef);
    lua_getfield(this->Thread, -1, xorstr_("Fire"));
    lua_getref(this->Thread, this->OnCloseRef);
    lua_pcall(this->Thread, 1, 0, 0);
    lua_settop(this->Thread, 0);

    lua_unref(this->Thread, this->OnMessageRef);
    lua_unref(this->Thread, this->OnCloseRef);
    lua_unref(this->Thread, this->ThreadRef);
}

int CWebsocket::HandleIndex(lua_State* L)
{
    if (!this->Connected)
        return 0;

    luaL_stackcheck(L, 2, 2, luaL_checktype(L, 1, LUA_TUSERDATA););
    luaL_checktype(L, 2, LUA_TSTRING);

    const auto Index = lua_tostring(L, 2);
    if (Index == "OnMessage")
    {
        lua_getref(L, this->OnMessageRef);
        lua_getfield(L, -1, "Event");
        return 1;
    }
    else if (Index == "OnClose")
    {
        lua_getref(L, this->OnCloseRef);
        lua_getfield(L, -1, "Event");
        return 1;
    }
    else if (Index == "Send")
    {
        lua_pushvalue(L, -10003);
        lua_pushcclosure(L,
            [](lua_State* L) -> int {
                luaL_stackcheck(L, 2, 2, luaL_checktype(L, 1, LUA_TUSERDATA););
                luaL_checktype(L, 2, LUA_TSTRING);

                const auto Data = lua_tostring(L, 1);

                auto __Websocket = (CWebsocket*)lua_touserdata(L, -10003);
                __Websocket->WebSocket->send(Data, true);

                return 0;
            }, "WebSocket.Send", 1);

        return 1;
    }
    else if (Index == "Close")
    {
        lua_pushvalue(L, -10003);
        lua_pushcclosure(L,
            [](lua_State* L) -> int {
                luaL_stackcheck(L, 1, 1, luaL_checktype(L, 1, LUA_TUSERDATA););

                auto __Websocket = (CWebsocket*)lua_touserdata(L, -10003);
                __Websocket->WebSocket->close();

                return 0;
            }, "WebSocket.Close", 1);
        return 1;
    }

    return 0;
}

int CWebsocket::HandleNewIndex(lua_State* L)
{
    luaL_error(L, "Unable to set property!");
    return 0;
}

__forceinline static int Websocket__Connect(lua_State* L)
{
    luaL_stackcheck(L, 1, 1, luaL_checktype(L, 1, LUA_TSTRING););
    const auto Url = lua_tostring(L, 1);

    CWebsocket* __Websocket = (CWebsocket*)lua_newuserdata(L, sizeof(CWebsocket));
    *__Websocket = CWebsocket{};

    __Websocket->Thread = lua_newthread(L);
    __Websocket->ThreadRef = lua_ref(L, -1);
    __Websocket->WebSocket = new ix::WebSocket();
    __Websocket->WebSocket->setUrl(Url);
    lua_pop(L, 1);

    lua_getglobal(L, "Instance");
    lua_getfield(L, -1, "new");
    lua_pushstring(L, "BindableEvent");
    lua_pcall(L, 1, 1, 0);
    __Websocket->OnMessageRef = lua_ref(L, -1);
    lua_pop(L, 2);

    lua_getglobal(L, "Instance");
    lua_getfield(L, -1, "new");
    lua_pushstring(L, "BindableEvent");
    lua_pcall(L, 1, 1, 0);
    __Websocket->OnCloseRef = lua_ref(L, -1);
    lua_pop(L, 2);

    __Websocket->WebSocket->setOnMessageCallback(
        [__Websocket](const ix::WebSocketMessagePtr& msg) -> void {
            if (msg->type == ix::WebSocketMessageType::Message)
                __Websocket->FireMessage(msg->str);
            else if (msg->type == ix::WebSocketMessageType::Close || msg->type == ix::WebSocketMessageType::Error)
                __Websocket->FireClose();
        }
    );

    lua_newtable(L);

    lua_pushstring(L, "WebSocket");
    lua_setfield(L, -2, "__type");

    lua_pushvalue(L, -2);
    lua_pushcclosure(L,
        [](lua_State* L) -> int {
            auto __Websocket = (CWebsocket*)lua_touserdata(L, -10003);;
            return __Websocket->HandleIndex(L);
        },
        "__index", 1);
    lua_setfield(L, -2, "__index");

    lua_pushvalue(L, -2);
    lua_pushcclosure(L,
        [](lua_State* L) -> int {
            auto __Websocket = (CWebsocket*)lua_touserdata(L, -10003);;
            return __Websocket->HandleNewIndex(L);
        },
        "__newindex", 1);
    lua_setfield(L, -2, "__newindex");

    lua_setmetatable(L, -2);

    __Websocket->WebSocket->connect(5);

    if (__Websocket->WebSocket->getReadyState() != ix::ReadyState::Open)
        luaL_error(L, "WebSocket connection failed");

    __Websocket->Connected = true;
    __Websocket->WebSocket->start();

    return 1;
}

static const luaL_Reg LibraryFunctions[] = {
    {"connect", Websocket__Connect}, {"Connect", Websocket__Connect},
    
    {nullptr, nullptr}
};

void Websocket::Initialize(lua_State* L)
{
    lua_newtable(L);
    luaL_register(L, nullptr, LibraryFunctions);
    lua_setreadonly(L, -1, true);
    lua_setglobal(L, "WebSocket");
}