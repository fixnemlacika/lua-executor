SDRPP =
{
	Init = function()
		return sdrpp_init();
	end,

	Deinit = function(sdrpp)
		sdrpp_deinit(sdrpp);
	end,

	IsConnected = function(sdrpp)
		return sdrpp_is_connected(sdrpp);
	end,

	Connect = function(sdrpp, host, port)
		return sdrpp_connect(sdrpp, tostring(host), tonumber(port));
	end,

	Disconnect = function(sdrpp)
		sdrpp_disconnect(sdrpp);
	end,

	Start = function(sdrpp)
		return sdrpp_start(sdrpp);
	end,

	Stop = function(sdrpp)
		return sdrpp_stop(sdrpp);
	end,

	Quit = function(sdrpp)
		return sdrpp_quit(sdrpp);
	end,

	VFO =
	{
		Modes =
		{
			AM  = SDRPP_VFO_MODE_AM,
			FM  = SDRPP_VFO_MODE_FM,
			CW  = SDRPP_VFO_MODE_CW,
			WFM = SDRPP_VFO_MODE_WFM,
			DSB = SDRPP_VFO_MODE_DSB,
			USB = SDRPP_VFO_MODE_USB,
			LSB = SDRPP_VFO_MODE_LSB,
			RAW = SDRPP_VFO_MODE_RAW
		},

		-- @return success, vfo_mode, vfo_bandwidth
		GetMode = function(sdrpp)
			return sdrpp_get_vfo_mode(sdrpp);
		end,

		SetMode = function(sdrpp, mode, bandwidth)
			return sdrpp_set_vfo_mode(sdrpp, tonumber(mode), tonumber(bandwidth));
		end,

		-- @return success, vfo_frequency
		GetFrequency = function(sdrpp)
			return sdrpp_get_vfo_frequency(sdrpp);
		end,

		SetFrequency = function(sdrpp, value)
			return sdrpp_set_vfo_frequency(sdrpp, tonumber(value));
		end
	},

	Recorder =
	{
		Start = function(sdrpp)
			return sdrpp_start_recorder(sdrpp);
		end,

		Stop = function(sdrpp)
			return sdrpp_stop_recorder(sdrpp);
		end
	}
};
