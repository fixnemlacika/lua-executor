#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/Mutex.hpp>

struct mutex
{
	AL::OS::Mutex mutex;
};

mutex  mutex_default_instance;

mutex* mutex_get_default_instance()
{
	return &mutex_default_instance;
}

mutex* mutex_init()
{
	auto mutex = new ::mutex
	{
	};

	return mutex;
}
void   mutex_deinit(mutex* mutex)
{
	if ((mutex != nullptr) && (mutex != &mutex_default_instance))
		delete mutex;
}

void   mutex_lock(mutex* mutex)
{
	if (mutex != nullptr)
		mutex->mutex.Lock();
}
void   mutex_unlock(mutex* mutex)
{
	if (mutex != nullptr)
		mutex->mutex.Unlock();
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalFunction(mutex_get_default_instance);
	LUA_APRS_IS_RegisterGlobalFunction(mutex_init);
	LUA_APRS_IS_RegisterGlobalFunction(mutex_deinit);
	LUA_APRS_IS_RegisterGlobalFunction(mutex_lock);
	LUA_APRS_IS_RegisterGlobalFunction(mutex_unlock);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalFunction(mutex_get_default_instance);
	LUA_APRS_IS_UnregisterGlobalFunction(mutex_init);
	LUA_APRS_IS_UnregisterGlobalFunction(mutex_deinit);
	LUA_APRS_IS_UnregisterGlobalFunction(mutex_lock);
	LUA_APRS_IS_UnregisterGlobalFunction(mutex_unlock);
});
