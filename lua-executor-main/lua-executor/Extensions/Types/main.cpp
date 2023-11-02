#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

bool is_null(void* value)
{
	return value == nullptr;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalFunction(is_null);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalFunction(is_null);
});
