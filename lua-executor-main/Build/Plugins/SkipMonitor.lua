require('Plugins.Gateway');

SkipMonitor = {};

function SkipMonitor.Init(aprs_callsign, aprs_is_passcode, aprs_is_host, aprs_is_port, database_path, threshold_miles)
	if not Gateway.Init(aprs_callsign, aprs_is_passcode, 'WIDE1-1', aprs_is_host, aprs_is_port, 't/poimqstu', database_path) then
		Console.WriteLine('SkipMonitor', 'Error initializing Gateway');
		return false;
	end

	SkipMonitor.IgnoreDigi('WIDE%d*');
	SkipMonitor.IgnoreDigi('WIDE%d+-%d+');

	Gateway.Events.RegisterEvent(Gateway.Events.OnReceivePacket, function(packet)
		local packet_sender          = APRS.Packet.GetSender(packet);
		local packet_igate           = APRS.Packet.GetIGate(packet);
		local packet_path            = APRS.Packet.GetDigiPath(packet);
		local packet_path_first_digi = SkipMonitor.Private.GetFirstDigi(packet_path);

		if not SkipMonitor.Private.IsSenderIgnored(packet_sender) then
			if packet_path_first_digi and (packet_path_first_digi ~= packet_sender) then
				if not SkipMonitor.Private.IsDigiIgnored(packet_path_first_digi) then
					local packet_path_first_digi_distance_ft = packet_path_first_digi and SkipMonitor.Private.GetStationDistanceToStation(packet_sender, packet_path_first_digi) or nil;

					if packet_path_first_digi_distance_ft then
						local packet_path_first_digi_distance_miles = packet_path_first_digi_distance_ft / 5280;

						if packet_path_first_digi_distance_miles >= threshold_miles then
							SkipMonitor.Events.ExecuteEvent(SkipMonitor.Events.OnSkipDetected, packet_sender, packet_path_first_digi, packet_path_first_digi_distance_miles, packet_path, packet_igate);
						end
					end
				end
			elseif packet_igate ~= packet_sender then
				if not SkipMonitor.Private.IsIGateIgnored(packet_igate) then
					local packet_igate_distance_ft = SkipMonitor.Private.GetStationDistanceToStation(packet_sender, packet_igate);

					if packet_igate_distance_ft then
						local packet_igate_distance_miles = packet_igate_distance_ft / 5280;

						if packet_igate_distance_miles >= threshold_miles then
							SkipMonitor.Events.ExecuteEvent(SkipMonitor.Events.OnSkipDetected, packet_sender, packet_igate, packet_igate_distance_miles, packet_path, packet_igate);
						end
					end
				end
			end
		end
	end);

	Gateway.Events.RegisterEvent(Gateway.Events.OnReceivePosition, function(station, path, igate, latitude, longitude, altitude, comment)
		SkipMonitor.Private.SetStationPosition(station, latitude, longitude, altitude);
	end);

	return true;
end

function SkipMonitor.Run(interval_ms)
	if not Gateway.Run(interval_ms) then
		Console.WriteLine('SkipMonitor', 'Error running Gateway');
		return false;
	end

	return true;
end

function SkipMonitor.IgnoreDigi(pattern, set)
	if not SkipMonitor.Private.IgnoredDigis then
		SkipMonitor.Private.IgnoredDigis = {};
	end

	if set then
		SkipMonitor.Private.IgnoredDigis[pattern] = true;
	else
		SkipMonitor.Private.IgnoredDigis[pattern] = nil;
	end
end

function SkipMonitor.IgnoreIGate(pattern, set)
	if not SkipMonitor.Private.IgnoredIGates then
		SkipMonitor.Private.IgnoredIGates = {};
	end

	if set then
		SkipMonitor.Private.IgnoredIGates[pattern] = true;
	else
		SkipMonitor.Private.IgnoredIGates[pattern] = nil;
	end
end

function SkipMonitor.IgnoreSender(pattern, set)
	if not SkipMonitor.Private.IgnoredSenders then
		SkipMonitor.Private.IgnoredSenders = {};
	end

	if set then
		SkipMonitor.Private.IgnoredSenders[pattern] = true;
	else
		SkipMonitor.Private.IgnoredSenders[pattern] = nil;
	end
end

SkipMonitor.Events                = {};
SkipMonitor.Events.OnSkipDetected = {}; -- function(sender, receiver, distance, path, igate)

function SkipMonitor.Events.ExecuteEvent(event, ...)
	Gateway.Events.ExecuteEvent(event, ...);
end

function SkipMonitor.Events.ScheduleEvent(event, delay, ...)
	Gateway.Events.ScheduleEvent(event, delay, ...);
end

function SkipMonitor.Events.RegisterEvent(event, callback)
	Gateway.Events.RegisterEvent(event, callback);
end

function SkipMonitor.Events.UnregisterEvent(event, callback)
	Gateway.Events.UnregisterEvent(event, callback);
end

SkipMonitor.Private = {};

function SkipMonitor.Private.IsDigiIgnored(value)
	if SkipMonitor.Private.IgnoredDigis then
		for pattern, _ in pairs(SkipMonitor.Private.IgnoredDigis) do
			if string.match(value, pattern) then
				return true;
			end
		end
	end

	return false;
end

function SkipMonitor.Private.IsIGateIgnored(value)
	if SkipMonitor.Private.IgnoredIGates then
		for pattern, _ in pairs(SkipMonitor.Private.IgnoredIGates) do
			if string.match(value, pattern) then
				return true;
			end
		end
	end

	return false;
end

function SkipMonitor.Private.IsSenderIgnored(value)
	if SkipMonitor.Private.IgnoredSenders then
		for pattern, _ in pairs(SkipMonitor.Private.IgnoredSenders) do
			if string.match(value, pattern) then
				return true;
			end
		end
	end

	return false;
end

function SkipMonitor.Private.GetFirstDigi(path)
	return string.match(path, '([^,]+)%*');
end

-- @return exists, latitude, longitude, altitude
function SkipMonitor.Private.GetStatationPosition(station)
	if not SkipMonitor.Private.Stations then
		return false, 0, 0, 0;
	end

	station = SkipMonitor.Private.Stations[station];

	if not station then
		return false, 0, 0, 0;
	end

	return true, station.Latitude, station.Longitude, station.Altitude;
end

function SkipMonitor.Private.SetStationPosition(station, latitude, longitude, altitude)
	if not SkipMonitor.Private.Stations then
		SkipMonitor.Private.Stations = {};
	end

	SkipMonitor.Private.Stations[station] =
	{
		Altitude  = altitude,
		Latitude  = latitude,
		Longitude = longitude
	};
end

function SkipMonitor.Private.GetStationDistanceToStation(station1, station2)
	local station1_exists, station1_latitude, station1_longitude, station1_altitude = SkipMonitor.Private.GetStatationPosition(station1);
	local station2_exists, station2_latitude, station2_longitude, station2_altitude = SkipMonitor.Private.GetStatationPosition(station2);

	if not station1_exists or not station2_exists then
		return nil;
	end

	return Gateway.Utility.GetDistanceBetweenPoints(station1_latitude, station1_longitude, station2_latitude, station2_longitude);
end
