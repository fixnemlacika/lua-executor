#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/System.hpp>

AL::uint64 system_get_idle_time()
{
#if defined(AL_PLATFORM_LINUX)
	// TODO: implement
	return 0;
#elif defined(AL_PLATFORM_WINDOWS)
	LASTINPUTINFO lastInputInfo =
	{
		.cbSize = sizeof(LASTINPUTINFO)
	};

	// TODO: 64 bit?
	if (!GetLastInputInfo(&lastInputInfo))
	{

		lastInputInfo.dwTime = 0;
	}

	return (GetTickCount64() - lastInputInfo.dwTime) / 1000;
#endif
}

AL::uint64 system_get_timestamp()
{
	return AL::OS::System::GetTimestamp().ToSeconds();
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalFunction(system_get_idle_time);
	LUA_APRS_IS_RegisterGlobalFunction(system_get_timestamp);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalFunction(system_get_idle_time);
	LUA_APRS_IS_UnregisterGlobalFunction(system_get_timestamp);
});
