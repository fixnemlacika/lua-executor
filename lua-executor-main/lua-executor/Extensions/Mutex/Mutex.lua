Mutex =
{
	GetDefaultInstance = function()
		return mutex_get_default_instance();
	end,

	Init = function()
		return mutex_init();
	end,

	Deinit = function(mutex)
		mutex_deinit(mutex);
	end,

	Lock = function(mutex)
		mutex_lock(mutex);
	end,

	Unlock = function(mutex)
		mutex_unlock(mutex);
	end
};
