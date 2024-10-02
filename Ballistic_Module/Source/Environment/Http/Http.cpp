#include "Http.hpp"

using namespace Libraries;

enum RequestMethods
{
	H_GET,
	H_HEAD,
	H_POST,
	H_PUT,
	H_DELETE,
	H_OPTIONS
};

std::map<std::string, RequestMethods> RequestMethodMap = {
	{ "get", H_GET },
	{ "head", H_HEAD },
	{ "post", H_POST },
	{ "put", H_PUT },
	{ "delete", H_DELETE },
	{ "options", H_OPTIONS }
};

std::map<uint32_t, std::string> HttpCache = {};
__forceinline static int Http__Get(lua_State* L)
{
	std::string Url;
	if (!lua_isstring(L, 1))
	{
		luaL_checkstring(L, 2);
		Url = std::string(lua_tostring(L, 2));
	}
	else
	{
		Url = lua_tostring(L, 1);
	}

	if (Url.find(xorstr_("http")) != 0)
	{
		luaL_error(L, xorstr_("Invalid protocol specified (expected 'http://' or 'https://')"));
		return 0;
	}

	auto Hash = FNV1A(Url.c_str());
	auto Data = HttpCache[Hash];

	auto BlockCache = true;
	if (HttpCache[Hash].length() != 0 && !BlockCache)
	{
		lua_pushlstring(L, Data.c_str(), Data.size());
		return 1;
	}

	return RBX::Yielder->HaltExecution(L,
		[Url, Hash]() -> auto {

			auto HttpGet = std::async(std::launch::async, [Url]() -> cpr::Response {
				return cpr::GetAsync(cpr::Url{ Url }, cpr::Header{ { "User-Agent", "Roblox/WinInet" } }).get();
				});
			auto Result = HttpGet.get();

			return [Result, Url, Hash](lua_State* L) -> int {
				if (Result.status_code == 0 || HttpStatus::IsError(Result.status_code))
				{
					auto Error = std::format("Http Error {} - {}", Result.status_code, Result.error.message);
					luaL_error(L, Error.c_str());
					return 0;
				}

				HttpCache[Hash] = Result.text;

				lua_pushlstring(L, Result.text.c_str(), Result.text.size());
				return 1;
				};
		}
	);
}

__forceinline static int Http__Post(lua_State* L)
{
	luaL_stackcheck(L, 4, 4);
	luaL_checktype(L, 1, LUA_TUSERDATA);
	luaL_checktype(L, 2, LUA_TSTRING);
	luaL_checktype(L, 3, LUA_TSTRING);
	luaL_checktype(L, 4, LUA_TSTRING);

	const std::string Url = lua_tostring(L, 2);
	const std::string Data = lua_tostring(L, 3);
	const std::string ContentType = lua_tostring(L, 4);

	if (Url.find(xorstr_("http")) != 0)
	{
		luaL_error(L, xorstr_("Invalid protocol specified (expected 'http://' or 'https://')"));
		return 0;
	}

	return RBX::Yielder->HaltExecution(L,
		[Url, Data, ContentType]() -> auto {
			auto Result = cpr::Post(cpr::Url{ Url },
				cpr::Header{ {"User-Agent", "Roblox/WinInet"} },
				cpr::Body{ Data },
				cpr::Header{ { "Content-Type", ContentType} });

			return [Result](lua_State* L) -> int {
				if (Result.status_code == 0 || HttpStatus::IsError(Result.status_code))
				{
					auto Error = std::format("Http Error {} - {}", Result.status_code, Result.error.message);
					luaL_error(L, Error.c_str());
					return 0;
				}

				lua_pushlstring(L, Result.text.c_str(), Result.text.size());
				return 1;
			};
		}
	);
}

__forceinline static int Http__Request(lua_State* L)
{
	luaL_stackcheck(L, 1, 1);
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_getfield(L, 1, xorstr_("Url"));
	if (lua_type(L, -1) != LUA_TSTRING)
	{
		luaL_error(L, xorstr_("Invalid or no 'Url' field specified in request table"));
		return 0;
	}

	std::string Url = lua_tostring(L, -1);
	if (Url.find(xorstr_("http")) != 0)
	{
		luaL_error(L, xorstr_("Invalid protocol specified (expected 'http://' or 'https://')"));
		return 0;
	}

	lua_pop(L, 1);

	auto Method = H_GET;
	lua_getfield(L, 1, ("Method"));

	if (lua_type(L, -1) == LUA_TSTRING)
	{
		std::string Methods = luaL_checkstring(L, -1);
		std::transform(Methods.begin(), Methods.end(), Methods.begin(), tolower);

		if (!RequestMethodMap.count(Methods))
		{
			luaL_error(L, xorstr_("Request type '%s' is not a valid http request type."), Methods.c_str());
			return 0;
		}

		Method = RequestMethodMap[Methods];
	}

	lua_pop(L, 1);

	cpr::Header Headers;

	lua_getfield(L, 1, xorstr_("Headers"));
	if (lua_type(L, -1) == LUA_TTABLE)
	{
		lua_pushnil(L);

		while (lua_next(L, -2)) 
		{
			if (lua_type(L, -2) != LUA_TSTRING || lua_type(L, -1) != LUA_TSTRING) 
			{
				luaL_error(L, xorstr_("'Headers' table must contain string keys/values."));
				return 0;
			}

			std::string HeaderKey = luaL_checkstring(L, -2);
			auto HeaderCopy = std::string(HeaderKey);
			std::transform(HeaderKey.begin(), HeaderKey.end(), HeaderKey.begin(), tolower);

			if (HeaderCopy == xorstr_("content-length"))
			{
				luaL_error(L, ("Headers: 'Content-Length' header cannot be overwritten."));
				return 0;
			}

			std::string HeaderValue = luaL_checkstring(L, -1);
			Headers.insert({ HeaderKey, HeaderValue });
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);

	cpr::Cookies Cookies;
	lua_getfield(L, 1, xorstr_("Cookies"));

	if (lua_type(L, -1) == LUA_TTABLE) 
	{
		std::map<std::string, std::string> RobloxCookies;
		lua_pushnil(L);

		while (lua_next(L, -2)) 
		{
			if (lua_type(L, -2) != LUA_TSTRING || lua_type(L, -1) != LUA_TSTRING)
			{
				luaL_error(L, xorstr_("'Cookies' table must contain string keys/values."));
				return 0;
			}

			std::string CookieKey = luaL_checkstring(L, -2);
			std::string CookieValue = luaL_checkstring(L, -1);

			RobloxCookies[CookieKey] = CookieValue;
			lua_pop(L, 1);
		}

		Cookies = RobloxCookies;
	}

	lua_pop(L, 1);

	auto HasUserAgent = false;
	for (auto& Header : Headers) 
	{
		auto HeaderName = Header.first;
		std::transform(HeaderName.begin(), HeaderName.end(), HeaderName.begin(), tolower);

		if (HeaderName == xorstr_("user-agent"))
			HasUserAgent = true;
	}

	if (!HasUserAgent)
	{
		Headers.insert({ "User-Agent", std::string(Configuration::Name) + "/" + Configuration::Version });
	}

	std::string Body;
	lua_getfield(L, 1, xorstr_("Body"));
	if (lua_type(L, -1) == LUA_TTABLE)
	{
		if (Method == H_GET || Method == H_HEAD)
		{
			luaL_error(L, xorstr_("'Body' cannot be present in GET or HEAD requests."));
			return 0;
		}

		size_t BodySize;
		auto BodyContent = luaL_checklstring(L, -1, &BodySize);
		Body = std::string(BodyContent, BodySize);
	}

	lua_pop(L, 1);

	return RBX::Yielder->HaltExecution(L,
		[Method, Url, Headers, Cookies, Body]() -> auto {
			cpr::Response Response;

			switch (Method)
			{
			case H_GET:
			{
				Response = cpr::Get(cpr::Url{ Url }, Cookies, Headers);
				break;
			}
			case H_HEAD:
			{
				Response = cpr::Head(cpr::Url{ Url }, Cookies, Headers);
				break;
			}
			case H_POST:
			{
				Response = cpr::Post(cpr::Url{ Url }, cpr::Body{ Body }, Cookies, Headers);
				break;
			}
			case H_PUT:
			{
				Response = cpr::Put(cpr::Url{ Url }, cpr::Body{ Body }, Cookies, Headers);
				break;
			}
			case H_DELETE:
			{
				Response = cpr::Delete(cpr::Url{ Url }, cpr::Body{ Body }, Cookies, Headers);
				break;
			}

			case H_OPTIONS:
			{
				Response = cpr::Options(cpr::Url{ Url }, cpr::Body{ Body }, Cookies, Headers);
				break;
			}
			default:
			{
				throw std::exception(xorstr_("invalid request type"));
			}
			}

			return [Response](lua_State* L) -> int {
				lua_newtable(L);

				lua_pushboolean(L, HttpStatus::IsSuccessful(Response.status_code));
				lua_setfield(L, -2, xorstr_("Success"));

				lua_pushinteger(L, Response.status_code);
				lua_setfield(L, -2, xorstr_("StatusCode"));

				auto Phrase = HttpStatus::ReasonPhrase(Response.status_code);
				lua_pushlstring(L, Phrase.c_str(), Phrase.size());
				lua_setfield(L, -2, xorstr_("StatusMessage"));

				lua_newtable(L);

				for (auto& Header : Response.header)
				{
					lua_pushlstring(L, Header.first.c_str(), Header.first.size());
					lua_pushlstring(L, Header.second.c_str(), Header.second.size());

					lua_settable(L, -3);
				}

				lua_setfield(L, -2, xorstr_("Headers"));

				lua_newtable(L);

				for (auto& cookie : Response.cookies.map_)
				{
					lua_pushlstring(L, cookie.first.c_str(), cookie.first.size());
					lua_pushlstring(L, cookie.second.c_str(), cookie.second.size());

					lua_settable(L, -3);
				}

				lua_setfield(L, -2, xorstr_("Cookies"));

				lua_pushlstring(L, Response.text.c_str(), Response.text.size());
				lua_setfield(L, -2, xorstr_("Body"));

				return 1;
			};
		}
	);
}

static luaL_Reg LibraryFunctions[] = {
	{"httpget", Http__Get}, {"http_get", Http__Get}, {"HttpGet", Http__Get},
	{"httpgetasync", Http__Get}, {"http_get_async", Http__Get}, {"HttpGetAsync", Http__Get},

	{"httppost", Http__Post}, {"http_post", Http__Post}, {"HttpPost", Http__Post},
	{"httppostasync", Http__Post}, {"http_post_async", Http__Post}, {"HttpPostAsync", Http__Post},

	{"http_request", Http__Request},
	{"request", Http__Request},

	{nullptr, nullptr}
};

void Http::Initialize(lua_State* L)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, nullptr, LibraryFunctions);
	lua_pop(L, 1);

	lua_newtable(L);
	lua_pushcclosure(L, Http__Request, ("http.request"), 0);
	lua_setfield(L, -2, ("request"));
	lua_setreadonly(L, -1, true);
	lua_setglobal(L, ("http"));
}