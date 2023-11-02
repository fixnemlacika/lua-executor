#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/Thread.hpp>
#include <AL/OS/Console.hpp>

struct thread
{
	AL::OS::Thread thread;
};

typedef AL::Lua54::Function::LuaCallback<void(thread* thread)> thread_start_callback;

void    thread_sleep(AL::uint32 ms)
{
	AL::Sleep(AL::TimeSpan::FromMilliseconds(ms));
}

thread* thread_init()
{
	auto thread = new ::thread
	{
	};

	return thread;
}
void    thread_deinit(thread* thread)
{
	if (thread != nullptr)
		delete thread;
}

thread* thread_start(thread_start_callback callback)
{
	if (auto thread = thread_init())
	{
		try
		{
			thread->thread.Start([thread, callback]()
			{
				callback(thread);
			});
		}
		catch (const AL::Exception& exception)
		{
			AL::OS::Console::WriteException(
				exception
			);

			thread_deinit(thread);

			return nullptr;
		}

		return thread;
	}

	return nullptr;
}
bool    thread_join(thread* thread, AL::uint32 max_wait_time_ms)
{
	if (thread && thread->thread.IsRunning())
	{
		try
		{
			if (!thread->thread.Join(AL::TimeSpan::FromMilliseconds(max_wait_time_ms)))
			{

				return false;
			}
		}
		catch (const AL::Exception& exception)
		{
			AL::OS::Console::WriteException(
				exception
			);

			return false;
		}

		thread_deinit(thread);
	}

	return true;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalFunction(thread_sleep);
	LUA_APRS_IS_RegisterGlobalFunction(thread_start);
	LUA_APRS_IS_RegisterGlobalFunction(thread_join);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalFunction(thread_sleep);
	LUA_APRS_IS_UnregisterGlobalFunction(thread_start);
	LUA_APRS_IS_UnregisterGlobalFunction(thread_join);
});
