#pragma once
#include <AL/Common.hpp>

#include "Extension.hpp"

namespace APRS::IS
{
	struct Extension;
};

enum SCRIPT_EXIT_CODES : AL::int16
{
	SCRIPT_EXIT_CODE_SUCCESS
};

enum SCRIPT_PLATFORMS : AL::uint8
{
	SCRIPT_PLATFORM_LINUX,
	SCRIPT_PLATFORM_WINDOWS
};

void                 script_init();
void                 script_deinit();

AL::int16            script_get_exit_code();
void                 script_set_exit_code(AL::int16 value);

SCRIPT_PLATFORMS     script_get_platform();

bool                 script_lua_eval(const char* lua);

APRS::IS::Extension* script_load_extension(const char* path);
void                 script_unload_extension(APRS::IS::Extension* extension);
