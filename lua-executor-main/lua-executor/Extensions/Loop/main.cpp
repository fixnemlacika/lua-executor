#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/Timer.hpp>

struct loop
{
	AL::OS::Timer timer;
	AL::TimeSpan  interval;
	AL::TimeSpan  last_update;
	AL::TimeSpan  next_update;
};

loop*      loop_init(AL::uint32 interval_ms)
{
	auto loop = new ::loop
	{
		.interval = AL::TimeSpan::FromMilliseconds(interval_ms)
	};

	loop->last_update = loop->timer.GetElapsed();
	loop->next_update = AL::TimeSpan::FromMilliseconds(loop->last_update.ToMilliseconds() + interval_ms);

	return loop;
}
void       loop_deinit(loop* loop)
{
	if (loop != nullptr)
		delete loop;
}

// @return number of milliseconds since last call
AL::uint64 loop_sync(loop* loop)
{
	if (loop == nullptr)
		return 0;

	auto time = loop->timer.GetElapsed();

	if (time < loop->next_update)
		AL::Sleep(loop->next_update - time);

	auto delta = (time = loop->timer.GetElapsed()) - loop->last_update;

	loop->last_update  = time;
	loop->next_update += loop->interval;

	return delta.ToMilliseconds();
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalFunction(loop_init);
	LUA_APRS_IS_RegisterGlobalFunction(loop_deinit);
	LUA_APRS_IS_RegisterGlobalFunction(loop_sync);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalFunction(loop_init);
	LUA_APRS_IS_UnregisterGlobalFunction(loop_deinit);
	LUA_APRS_IS_UnregisterGlobalFunction(loop_sync);
});
