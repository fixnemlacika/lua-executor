#pragma once
#include <AL/Common.hpp>

#include <AL/Lua54/Lua.hpp>

struct Extension
{
	AL::Lua54::State* lua;
};

#if defined(LUA_APRS_IS_EXTENSION)
	#define LUA_APRS_IS_EXTENSION_INIT(...) \
		extern "C" AL_DLL_EXPORT Extension* AL_CDECL lua_aprs_is_extension_init(AL::Lua54::State& lua) \
		{ \
			auto extension = new Extension \
			{ \
				.lua = &lua \
			}; \
			\
			__VA_ARGS__(*extension); \
			\
			return extension; \
		}

	#define LUA_APRS_IS_EXTENSION_DEINIT(...) \
		extern "C" AL_DLL_EXPORT void AL_CDECL lua_aprs_is_extension_deinit(Extension* extension) \
		{ \
			__VA_ARGS__(*extension); \
			\
			delete extension; \
		}
#else
	typedef Extension*(AL_CDECL lua_aprs_is_extension_init)(AL::Lua54::State& lua);
	typedef void      (AL_CDECL lua_aprs_is_extension_deinit)(Extension* extension);
#endif

#define LUA_APRS_IS_GetGlobal(__variable__)                                   LUA_APRS_IS_GetGlobalEx(__variable__, #__variable__)
#define LUA_APRS_IS_GetGlobalEx(__variable__, __variable_name__)              __variable__ = extension.lua->GetGlobal<decltype(__variable__)>(__variable_name__)

#define LUA_APRS_IS_SetGlobal(__variable__)                                   LUA_APRS_IS_SetGlobalEx(__variable__, #__variable__)
#define LUA_APRS_IS_SetGlobalEx(__variable__, __variable_name__)              extension.lua->SetGlobal(__variable_name__, __variable__)

#define LUA_APRS_IS_RegisterGlobal(__variable__)                              LUA_APRS_IS_RegisterGlobalEx(__variable__, #__variable__)
#define LUA_APRS_IS_RegisterGlobalEx(__variable__, __variable_name__)         extension.lua->SetGlobal(__variable_name__, __variable__)

#define LUA_APRS_IS_UnregisterGlobal(__variable__)                            LUA_APRS_IS_UnregisterGlobalEx(#__variable__)
#define LUA_APRS_IS_UnregisterGlobalEx(__variable_name__)                     extension.lua->RemoveGlobal(__variable_name__)

#define LUA_APRS_IS_RegisterGlobalFunction(__function__)                      LUA_APRS_IS_RegisterGlobalFunctionEx(__function__, #__function__)
#define LUA_APRS_IS_RegisterGlobalFunctionEx(__function__, __function_name__) extension.lua->SetGlobalFunction<__function__>(__function_name__)

#define LUA_APRS_IS_UnregisterGlobalFunction(__function__)                    LUA_APRS_IS_UnregisterGlobalFunctionEx(#__function__)
#define LUA_APRS_IS_UnregisterGlobalFunctionEx(__function_name__)             extension.lua->RemoveGlobal(__function_name__)
