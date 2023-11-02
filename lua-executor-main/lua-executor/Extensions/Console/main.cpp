#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/Console.hpp>

#if defined(AL_PLATFORM_WINDOWS)
	#include <AL/OS/Windows/Console.hpp>
#endif

AL::String console_last_line;

bool        console_set_title(const char* value)
{
	return AL::OS::Console::SetTitle(value);
}

char        console_read()
{
	AL::String::Char value;

	return AL::OS::Console::Read(value) ? value : '\0';
}
const char* console_read_line()
{
	return AL::OS::Console::ReadLine(console_last_line) ? console_last_line.GetCString() : nullptr;
}

bool        console_write(const char* value)
{
	return AL::OS::Console::Write(value);
}
bool        console_write_line(const char* value)
{
	return AL::OS::Console::WriteLine(value);
}

bool        console_enable_quick_edit_mode()
{
#if defined(AL_PLATFORM_LINUX)
	// TODO: implement

	return false;
#elif defined(AL_PLATFORM_WINDOWS)
	try
	{
		AL::OS::Windows::Console::EnableQuickEdit();
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(exception);

		return false;
	}
#endif

	return true;
}
bool        console_disable_quick_edit_mode()
{
#if defined(AL_PLATFORM_LINUX)
	// TODO: implement

	return false;
#elif defined(AL_PLATFORM_WINDOWS)
	try
	{
		AL::OS::Windows::Console::DisableQuickEdit();
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(exception);

		return false;
	}
#endif

	return true;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalFunction(console_set_title);
	LUA_APRS_IS_RegisterGlobalFunction(console_read);
	LUA_APRS_IS_RegisterGlobalFunction(console_read_line);
	LUA_APRS_IS_RegisterGlobalFunction(console_write);
	LUA_APRS_IS_RegisterGlobalFunction(console_write_line);
	LUA_APRS_IS_RegisterGlobalFunction(console_enable_quick_edit_mode);
	LUA_APRS_IS_RegisterGlobalFunction(console_disable_quick_edit_mode);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalFunction(console_set_title);
	LUA_APRS_IS_UnregisterGlobalFunction(console_read);
	LUA_APRS_IS_UnregisterGlobalFunction(console_read_line);
	LUA_APRS_IS_UnregisterGlobalFunction(console_write);
	LUA_APRS_IS_UnregisterGlobalFunction(console_write_line);
	LUA_APRS_IS_UnregisterGlobalFunction(console_enable_quick_edit_mode);
	LUA_APRS_IS_UnregisterGlobalFunction(console_disable_quick_edit_mode);
});
