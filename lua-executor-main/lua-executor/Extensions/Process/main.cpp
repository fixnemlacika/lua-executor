#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/FileSystem/Path.hpp>

#if defined(AL_PLATFORM_LINUX)
	#include <unistd.h>
#endif

const char* process_get_root_directory()
{
	static AL::FileSystem::Path root_directory;

	if (root_directory.GetString().GetLength() != 0)
		return root_directory.GetString().GetCString();
	else
	{
#if defined(AL_PLATFORM_LINUX)
		char        path[PATH_MAX] = { 0 };
		AL::ssize_t path_length;

		if ((path_length = ::readlink("/proc/self/exe", path, PATH_MAX)) > 0)
		{
			root_directory = AL::FileSystem::Path(AL::String(path, static_cast<AL::size_t>(path_length))).GetRootPath();

			return root_directory.GetString().GetCString();
		}
#elif defined(AL_PLATFORM_WINDOWS)
		if (auto hModule = GetModuleHandleA(NULL))
		{
			char path[AL_MAX_PATH] = { 0 };

			GetModuleFileNameA(hModule, path, AL_MAX_PATH);

			if (AL::OS::GetLastError() == ERROR_SUCCESS)
			{
				root_directory = AL::FileSystem::Path(path).GetRootPath();

				return root_directory.GetString().GetCString();
			}
		}
#endif
	}

	return nullptr;
}

const char* process_get_working_directory()
{
	static AL::FileSystem::Path working_directory;

	if (working_directory.GetString().GetLength() != 0)
		return working_directory.GetString().GetCString();
	else
	{
#if defined(AL_PLATFORM_LINUX)
		char buffer[PATH_MAX] = { 0 };

		if (::getcwd(buffer, PATH_MAX) != NULL)
		{
			working_directory = buffer;

			return working_directory.GetString().GetCString();
		}
#elif defined(AL_PLATFORM_WINDOWS)
		char buffer[MAX_PATH] = { 0 };

		if (GetCurrentDirectoryA(sizeof(buffer), buffer) != 0)
		{
			working_directory = buffer;

			return working_directory.GetString().GetCString();
		}
#endif
	}

	return nullptr;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalFunction(process_get_root_directory);
	LUA_APRS_IS_RegisterGlobalFunction(process_get_working_directory);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalFunction(process_get_root_directory);
	LUA_APRS_IS_UnregisterGlobalFunction(process_get_working_directory);
});
