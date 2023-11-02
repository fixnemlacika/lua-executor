Loop =
{
	Init = function(interval_ms)
		return loop_init(tonumber(interval_ms));
	end,

	Deinit = function(loop)
		loop_deinit(loop);
	end,

	-- @return number of milliseconds since last call
	Sync = function(loop)
		return loop_sync(loop);
	end,
};
