N2YO =
{
	Init = function(api_key)
		return n2yo_init(tostring(api_key));
	end,

	Deinit = function(n2yo)
		n2yo_deinit(n2yo);
	end,

	ExecuteQuery = function(n2yo, norad_id, latitude, longitude, altitude, number_of_days, min_elevation)
		return n2yo_execute_query(n2yo, tonumber(norad_id), tonumber(latitude), tonumber(longitude), tonumber(altitude), tonumber(number_of_days), tonumber(min_elevation));
	end,

	QueryResult =
	{
		Deinit = function(query_result)
			n2yo_query_result_deinit(query_result);
		end,

		GetNoradId = function(query_result)
			return n2yo_query_result_get_norad_id(query_result);
		end,

		GetSatelliteId = function(query_result)
			return n2yo_query_result_get_satellite_id(query_result);
		end,

		GetSatelliteName = function(query_result)
			return n2yo_query_result_get_satellite_name(query_result);
		end,

		GetSatellitePass = function(query_result, index)
			return n2yo_query_result_get_satellite_pass(query_result, tonumber(index));
		end,

		GetSatellitePassCount = function(query_result)
			return n2yo_query_result_get_satellite_pass_count(query_result);
		end,

		GetTransactionCount = function(query_result)
			return n2yo_query_result_get_transaction_count(query_result);
		end
	},

	SatellitePass =
	{
		GetTimeStart = function(satellite_pass)
			return n2yo_satellite_pass_get_time_start(satellite_pass);
		end,

		GetTimeStop = function(satellite_pass)
			return n2yo_satellite_pass_get_time_stop(satellite_pass);
		end,

		GetMaxElevation = function(satellite_pass)
			return n2yo_satellite_pass_get_max_elevation(satellite_pass);
		end
	}
};
