Thread =
{
	Sleep = function(ms)
		thread_sleep(tonumber(ms));
	end,

	Start = function(callback)
		return thread_start(callback);
	end,

	Join = function(thread, max_wait_time_ms)
		return thread_join(thread, tonumber(max_wait_time_ms));
	end
};
