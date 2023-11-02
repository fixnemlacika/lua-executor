#include "API.hpp"
#include "Script.hpp"

#include <AL/OS/Console.hpp>

AL::int16 script_exit_code;

void                 script_init()
{
	script_exit_code = SCRIPT_EXIT_CODE_SUCCESS;
}
void                 script_deinit()
{
}

AL::int16            script_get_exit_code()
{
	return script_exit_code;
}
void                 script_set_exit_code(AL::int16 value)
{
	script_exit_code = value;
}

SCRIPT_PLATFORMS     script_get_platform()
{
#if defined(AL_PLATFORM_LINUX)
	return SCRIPT_PLATFORM_LINUX;
#elif defined(AL_PLATFORM_WINDOWS)
	return SCRIPT_PLATFORM_WINDOWS;
#endif
}

bool                 script_lua_eval(const char* lua)
{
	try
	{
		APRS::IS::API::Run(lua);
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(exception);

		return false;
	}

	return true;
}

APRS::IS::Extension* script_load_extension(const char* path)
{
	try
	{
		if (auto extension = APRS::IS::API::LoadExtension(path))
			return extension;
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(exception);
	}

	return nullptr;
}
void                 script_unload_extension(APRS::IS::Extension* extension)
{
	if (extension != nullptr)
		APRS::IS::API::UnloadExtension(extension);
}
