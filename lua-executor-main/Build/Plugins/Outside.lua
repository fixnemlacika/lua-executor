require('Plugins.Gateway');

Script.LoadExtension('Extensions.DiscordRPC');

Outside          = {};
Outside.Private  = {};
Outside.INFINITE = Gateway.INFINITE;

Script.ExitCodes.UserRequested = Script.ExitCodes.UserDefined + 1;
Script.ExitCodes.UserDefined   = Script.ExitCodes.UserDefined + 1000;

function Outside.Init(aprs_callsign, aprs_is_passcode, aprs_path, aprs_is_host, aprs_is_port, min_idle_time, position_ttl, database_path, station_list)
	Outside.Private.Config                       = {};
	Outside.Private.Config.MinIdleTime           = min_idle_time;
	Outside.Private.Config.PositionTTL           = position_ttl;
	Outside.Private.Config.Discord               = {};
	Outside.Private.Config.Discord.ApplicationID = '1108502895051149372';

	local aprs_is_filter = 'b/' .. aprs_callsign;

	if station_list then
		Outside.Private.Config.StationList = station_list;

		for station_callsign, station_name in pairs(station_list) do
			aprs_is_filter = aprs_is_filter .. '/' .. station_callsign;
		end
	else
		Console.WriteLine('Outside', 'Warning: No station list defined - *ALL STATIONS* will be accepted');

		if string.lower(Console.ReadLine('Outside', 'Are you sure you want to unleash the spam? Y/N')) ~= 'y' then
			Console.WriteLine('Outside', 'Good choice.');
			Script.SetExitCode(Script.ExitCodes.UserRequested);
			return false;
		end

		aprs_is_filter = 't/p';
	end

	Outside.Private.Discord.InitResources();

	if not Gateway.Init(aprs_callsign, aprs_is_passcode, aprs_path, aprs_is_host, aprs_is_port, aprs_is_filter, database_path) then
		Console.WriteLine('Outside', 'Error initializing Gateway');
		return false;
	end

	if Outside.Private.RestoreMostRecentStationLocation() then
		local timestamp, name, callsign, path, igate, latitude, longitude, altitude, comment = Outside.GetMostRecentStationPosition();
		local timestamp_delta                                                                = System.GetTimestamp() - timestamp;

		Console.WriteLine('Outside', string.format('Restored position of %s from %s ago', callsign, Outside.Private.FormatTimestampDeltaAsString(timestamp_delta)));
	end

	Outside.Events.RegisterEvent(Gateway.Events.OnUpdate, function(delta_ms)
		Outside.Private.UpdateIdleState();
	end);

	Outside.Events.RegisterEvent(Gateway.Events.OnReceivePosition, function(station, path, igate, latitude, longitude, altitude, comment)
		local function update_station_position(name)
			Gateway.Storage.SetLastPosition(name, station, path, igate, latitude, longitude, altitude, comment);
			Outside.Private.SetStationPosition(System.GetTimestamp(), name, station, path, igate, latitude, longitude, altitude, comment);
		end

		if not Outside.Private.Config.StationList then
			update_station_position(station);
		else
			for station_callsign, station_name in pairs(Outside.Private.Config.StationList) do
				if station_callsign == station then
					update_station_position(station_name);
					break;
				end
			end
		end
	end);

	Outside.Events.RegisterEvent(Outside.Private.Events.UpdateDiscord, function()
		if Outside.Private.IsIdle() then
			local discord_header                  = nil;
			local discord_details                 = nil;
			local discord_icon_big                = ((tonumber(os.date('%H')) >= 6) and (tonumber(os.date('%H')) < 21)) and 'outside_day' or 'outside_night';
			local discord_icon_big_text           = nil;
			local discord_icon_small              = nil;
			local discord_icon_small_text         = nil;

			local latitude, longitude, altitude                                                                                                                          = Outside.GetPosition();
			local station_timestamp, station_name, station_callsign, station_path, station_igate, station_latitude, station_longitude, station_altitude, station_comment = Outside.GetMostRecentStationPosition();

			if station_timestamp == 0 then
				discord_header, discord_details = Outside.GetDefaultIdleMessage();
			else
				local station_distance                                    = Gateway.Utility.GetDistanceBetweenPoints(latitude, longitude, station_latitude, station_longitude);
				local idle_message_header, idle_message, distance_divider = Outside.Private.GetIdleMessageByPosition(station_latitude, station_longitude);

				if not idle_message_header or not idle_message then
					idle_message_header, idle_message, distance_divider = Outside.Private.GetIdleMessageByDistance(station_distance);
				end

				if not idle_message_header or not idle_message then
					idle_message_header, idle_message, distance_divider = Outside.GetDefaultIdleMessage();
				end

				local path_icon_station, path_icon, path_icon_comment = Outside.Private.GetStationPathIcon(station_path);

				discord_header          = idle_message_header;
				discord_details         = string.format(idle_message, station_distance / distance_divider);
				discord_icon_big_text   = station_comment;
				discord_icon_small      = path_icon or 'aprs_icon';
				discord_icon_small_text = path_icon_comment and string.format(path_icon_comment, station_igate) or string.format('%s via %s', station_path, station_igate);
			end

			while not Outside.Private.Discord.Presence.Update(discord_header, discord_details, Outside.Private.IdleTimestamp, discord_icon_big, discord_icon_big_text, discord_icon_small, discord_icon_small_text) do
				Outside.Private.Discord.Deinit();

				if not Outside.Private.Discord.Init(Outside.Private.Config.Discord.ApplicationID) then
					break;
				end
			end

			if Outside.Private.DiscordAutoUpdate then
				Outside.Events.ScheduleEvent(Outside.Private.Events.UpdateDiscord, 5);
			end
		end
	end);

	return true;
end

function Outside.Run(interval_ms)
	if not Gateway.Run(interval_ms) then
		Console.WriteLine('Outside', 'Error running Gateway');
		return false;
	end

	Outside.Private.LeaveIdleState();

	Outside.Private.Discord.DeinitResources();

	return true;
end

-- @return latitude, longitude, altitude
function Outside.GetPosition()
	if not Outside.Private.Position then
		return 0, 0, 0;
	end

	return Outside.Private.Position.Latitude, Outside.Private.Position.Longitude, Outside.Private.Position.Altitude;
end

function Outside.SetPosition(latitude, longitude, altitude)
	Outside.Private.Position =
	{
		Altitude  = altitude,
		Latitude  = latitude,
		Longitude = longitude
	};
end

-- @return timestamp, name, callsign, path, igate, latitude, longitude, altitude, comment
function Outside.GetMostRecentStationPosition()
	if not Outside.Private.StationPositions then
		return 0, nil, nil, nil, nil, 0, 0, 0, nil;
	end

	local most_recent_station_callsign  = nil;
	local most_recent_station_timestamp = 0;

	for station_callsign, station in pairs(Outside.Private.StationPositions) do
		if (System.GetTimestamp() - station.Timestamp) <= Outside.Private.Config.PositionTTL then
			if station.Timestamp > most_recent_station_timestamp then
				most_recent_station_callsign  = station_callsign;
				most_recent_station_timestamp = station.Timestamp;
			end
		end
	end

	local station = Outside.Private.StationPositions[most_recent_station_callsign];

	if not station then
		return 0, nil, nil, nil, nil, 0, 0, 0, nil;
	end

	return station.Timestamp, tostring(station.Name), tostring(most_recent_station_callsign), tostring(station.Path), tostring(station.IGate), tonumber(station.Latitude), tonumber(station.Longitude), tonumber(station.Altitude), tostring(station.Comment or '');
end

function Outside.Private.SetStationPosition(timestamp, name, callsign, path, igate, latitude, longitude, altitude, comment)
	if not Outside.Private.StationPositions then
		Outside.Private.StationPositions = {};
	end

	Outside.Private.StationPositions[callsign] =
	{
		Timestamp = timestamp,
		Name      = name,
		Path      = path,
		IGate     = igate,
		Comment   = comment,
		Altitude  = altitude,
		Latitude  = latitude,
		Longitude = longitude
	};

	Outside.Events.ExecuteEvent(Outside.Events.OnPositionChanged, name, callsign, path, igate, latitude, longitude, altitude, comment);
end

-- @return header, message, distance_divider
function Outside.Private.GetIdleMessageByDistance(value)
	if not Outside.Private.IdleMessagesByDistance then
		return nil, nil, 1;
	end

	local nearest_position_index = 0;
	local nearest_position_delta = 0;

	for i, idle_message in ipairs(Outside.Private.IdleMessagesByDistance) do
		if value >= idle_message.MinDistance then
			local idle_message_distance_delta = value - idle_message.MinDistance;

			if (idle_message_distance_delta < nearest_position_delta) or (nearest_position_index == 0) then
				nearest_position_index = i;
				nearest_position_delta = idle_message_distance_delta;
			end
		end
	end

	local idle_message = Outside.Private.IdleMessagesByDistance[nearest_position_index];

	if not idle_message then
		return nil, nil, 1;
	end

	return tostring(idle_message.Header), tostring(idle_message.Message), idle_message.Divider;
end

function Outside.AddIdleMessageByDistance(min_distance, header, message, distance_divider)
	if not Outside.Private.IdleMessagesByDistance then
		Outside.Private.IdleMessagesByDistance = {};
	end

	table.insert(Outside.Private.IdleMessagesByDistance, {
		Header      = header,
		Message     = message,
		Divider     = distance_divider,
		MinDistance = min_distance
	});
end

-- @return header, message, distance_divider
function Outside.Private.GetIdleMessageByPosition(latitude, longitude)
	if not Outside.Private.IdleMessagesByPosition then
		return nil, nil, 1;
	end

	local nearest_position_index                = 0;
	local nearest_position_distance_delta       = 0;
	local nearest_position_distance_is_infinite = false;

	for i, idle_message in ipairs(Outside.Private.IdleMessagesByPosition) do
		local idle_message_distance = Gateway.Utility.GetDistanceBetweenPoints(latitude, longitude, idle_message.Latitude, idle_message.Longitude);

		if not nearest_position_distance_is_infinite then
			if idle_message_distance == Outside.INFINITE then
				nearest_position_index                = i;
				nearest_position_distance_delta       = idle_message_distance;
				nearest_position_distance_is_infinite = true;
			elseif idle_message_distance <= idle_message.Radius then
				local idle_message_distance_delta = idle_message.Radius - idle_message_distance;
	
				if (idle_message_distance_delta < nearest_position_distance_delta) or (nearest_position_index == 0) then
					nearest_position_index          = i;
					nearest_position_distance_delta = idle_message_distance_delta;
				end
			end
		elseif idle_message_distance < nearest_position_distance_delta then
			nearest_position_index          = i;
			nearest_position_distance_delta = idle_message_distance;
		end
	end

	local idle_message = Outside.Private.IdleMessagesByPosition[nearest_position_index];

	if not idle_message then
		return nil, nil, 1;
	end

	return tostring(idle_message.Header), tostring(idle_message.Message), idle_message.Divider;
end

function Outside.AddIdleMessageByPosition(latitude, longitude, radius, header, message, distance_divider)
	if not Outside.Private.IdleMessagesByPosition then
		Outside.Private.IdleMessagesByPosition = {};
	end

	table.insert(Outside.Private.IdleMessagesByPosition, {
		Header    = header,
		Message   = message,

		Radius    = radius,
		Divider   = distance_divider,
		Latitude  = latitude,
		Longitude = longitude
	});
end

-- @return header, message, distance_divider
function Outside.GetDefaultIdleMessage()
	local idle_message = Outside.Private.DefaultIdleMessage;

	if not idle_message then
		return '', '', 1;
	end

	return idle_message.Header, idle_message.Message, idle_message.Divider;
end

function Outside.SetDefaultIdleMessage(header, message, distance_divider)
	Outside.Private.DefaultIdleMessage =
	{
		Header  = header,
		Message = message,
		Divider = distance_divider
	};
end

-- @return station, icon, comment
function Outside.Private.GetStationPathIcon(path)
	if not Outside.Private.StationPathIcons then
		return nil, nil, nil;
	end

	for station in string.gmatch(path, '[^,]+') do
		local icon = Outside.Private.StationPathIcons[station];

		if icon then
			return station, tostring(icon.Icon), tostring(icon.Comment);
		end
	end

	return nil, nil, nil;
end

function Outside.AddStationPathIcon(station, icon, comment)
	if not Outside.Private.StationPathIcons then
		Outside.Private.StationPathIcons = {};
	end

	Outside.Private.StationPathIcons[station] =
	{
		Icon    = icon,
		Comment = comment
	};
end

-- @return button
function Outside.AddButton(label, url)
	return Outside.Private.Discord.Presence.AddButton(label, url);
end

function Outside.RemoveButton(button)
	return Outside.Private.Discord.Presence.RemoveButton(button);
end

function Outside.Private.IsIdle()
	return Outside.Private.IdleTimestamp ~= nil;
end

function Outside.Private.UpdateIdleState()
	if Outside.Private.IsIdle() then
		if System.GetIdleTime() >= Outside.Private.Config.MinIdleTime then
			while not Outside.Private.Discord.Poll() do
				Outside.Private.Discord.Deinit();

				if not Outside.Private.Discord.Init(Outside.Private.Config.Discord.ApplicationID) then
					return false;
				end
			end
		else
			Outside.Private.LeaveIdleState();
		end
	elseif not Outside.Private.IsIdle() then
		if System.GetIdleTime() >= Outside.Private.Config.MinIdleTime then
			if not Outside.Private.EnterIdleState() then
				return false;
			end
		end
	end

	return true;
end

function Outside.Private.EnterIdleState()
	Outside.Private.IdleTimestamp = System.GetTimestamp();

	if not Outside.Private.Discord.Init(Outside.Private.Config.Discord.ApplicationID) then
		Outside.Private.IdleTimestamp = nil;
		return false;
	end

	Outside.Private.DiscordAutoUpdate = true;

	Outside.Events.ExecuteEvent(Outside.Events.OnEnterIdleState);
	Outside.Events.ExecuteEvent(Outside.Private.Events.UpdateDiscord);

	return true;
end

function Outside.Private.LeaveIdleState()
	if Outside.Private.IsIdle() then
		Outside.Private.DiscordAutoUpdate = nil;

		Outside.Events.ExecuteEvent(Outside.Events.OnLeaveIdleState);

		Outside.Private.Discord.Deinit();
		Outside.Private.IdleTimestamp = nil;
	end
end

function Outside.Private.RestoreMostRecentStationLocation()
	local timestamp, name, station, path, igate, latitude, longitude, altitude, comment = Gateway.Storage.GetLastPosition();

	if not station or ((System.GetTimestamp() - timestamp) > Outside.Private.Config.PositionTTL) then
		return false;
	end

	Outside.Private.SetStationPosition(timestamp, name, station, path, igate, latitude, longitude, altitude, comment);

	return true;
end

function Outside.Private.FormatTimestampDeltaAsString(delta)
	local function is_plural(value)
		return (value < 1) or (value >= 2);
	end

	if delta < 60 then
		return string.format('%.0f second%s', delta, is_plural(delta) and 's' or '');
	elseif delta < 3600 then
		local delta_minutes = delta / 60;
		local delta_seconds = delta - (math.floor(delta_minutes) * 60);

		return string.format('%.0f minute%s and %.0f second%s', delta_minutes, is_plural(delta_minutes) and 's' or '', delta_seconds, is_plural(delta_seconds) and 's' or '');
	end

	local delta_hours   = delta / 3600;
	local delta_minutes = (delta - (math.floor(delta_hours) * 3600)) / 60;

	return string.format('%.0f hour%s and %.0f minute%s', delta_hours, is_plural(delta_hours) and 's' or '', delta_minutes, is_plural(delta_minutes) and 's' or '');
end

Outside.Private.Discord                  = {};
Outside.Private.Discord.Presence         = {};
Outside.Private.Discord.Presence.Buttons = {};
Outside.Private.Discord.EventHandlers    = {};

function Outside.Private.Discord.InitResources()
	Outside.Private.Discord.Presence.Handle = DiscordRPC.Presence.Init();
end

function Outside.Private.Discord.DeinitResources()
	if Outside.Private.Discord.Presence.Handle then
		DiscordRPC.Presence.Deinit(Outside.Private.Discord.Presence.Handle);
		Outside.Private.Discord.Presence.Handle = nil;
	end
end

function Outside.Private.Discord.Init(application_id)
	Outside.Private.Discord.Handle = DiscordRPC.Init(
		application_id,
		Outside.Private.Discord.EventHandlers.OnReady,
		Outside.Private.Discord.EventHandlers.OnError,
		Outside.Private.Discord.EventHandlers.OnConnect,
		Outside.Private.Discord.EventHandlers.OnDisconnect
	);

	if not Outside.Private.Discord.Handle then
		return false;
	end

	return true;
end

function Outside.Private.Discord.Deinit()
	if Outside.Private.Discord.Handle then
		DiscordRPC.Deinit(Outside.Private.Discord.Handle);
		Outside.Private.Discord.Handle = nil;
	end
end

function Outside.Private.Discord.Poll()
	return DiscordRPC.Poll(Outside.Private.Discord.Handle);
end

function Outside.Private.Discord.Presence.Update(header, details, timestamp, large_image_key, large_image_text, small_image_key, small_image_text)
	DiscordRPC.Presence.SetHeader(Outside.Private.Discord.Presence.Handle, header);
	DiscordRPC.Presence.SetDetails(Outside.Private.Discord.Presence.Handle, details);
	DiscordRPC.Presence.SetTimeStart(Outside.Private.Discord.Presence.Handle, timestamp);
	DiscordRPC.Presence.SetImageLargeKey(Outside.Private.Discord.Presence.Handle, large_image_key);
	DiscordRPC.Presence.SetImageLargeText(Outside.Private.Discord.Presence.Handle, large_image_text);
	DiscordRPC.Presence.SetImageSmallKey(Outside.Private.Discord.Presence.Handle, small_image_key);
	DiscordRPC.Presence.SetImageSmallText(Outside.Private.Discord.Presence.Handle, small_image_text);

	return DiscordRPC.UpdatePresence(Outside.Private.Discord.Handle, Outside.Private.Discord.Presence.Handle);
end

-- @return button
function Outside.Private.Discord.Presence.AddButton(label, url)
	return DiscordRPC.Presence.Buttons.Add(Outside.Private.Discord.Presence.Handle, label, url);
end

function Outside.Private.Discord.Presence.RemoveButton(button)
	DiscordRPC.Presence.Buttons.Remove(Outside.Private.Discord.Presence.Handle, button);
end

function Outside.Private.Discord.EventHandlers.OnReady(user_id, user_name, user_username, user_flags, user_premium)
	-- Console.WriteLine('Discord', string.format('Ready [Name: %s, Username: %s]', user_name, user_username));
end

function Outside.Private.Discord.EventHandlers.OnError(error_code, error_message)
	if not error_message or (string.len(error_message) == 0) then
		Console.WriteLine('Discord', string.format('Error [Code: %u]', error_code));
	else
		Console.WriteLine('Discord', string.format('Error [Code: %u, Message: %s]', error_code, error_message));
	end
end

function Outside.Private.Discord.EventHandlers.OnConnect()
	-- Console.WriteLine('Discord', 'Connected');
end

function Outside.Private.Discord.EventHandlers.OnDisconnect(error_code, error_message)
	if error_code == 0 then
		Console.WriteLine('Discord', 'Disconnected');
	else
		if not error_message or (string.len(error_message) == 0) then
			Console.WriteLine('Discord', string.format('Disconnected [Code: %u]', error_code));
		else
			Console.WriteLine('Discord', string.format('Disconnected [Code: %u, Message: %s]', error_code, error_message));
		end
	end
end

Outside.Events                       = {};
Outside.Events.OnEnterIdleState      = {}; -- function()
Outside.Events.OnLeaveIdleState      = {}; -- function()
Outside.Events.OnPositionChanged     = {}; -- function(name, station, path, igate, latitude, longitude, altitude, comment)
Outside.Private.Events               = {};
Outside.Private.Events.UpdateDiscord = {}; -- function()

function Outside.Events.ExecuteEvent(event, ...)
	Gateway.Events.ExecuteEvent(event, ...);
end

function Outside.Events.ScheduleEvent(event, delay, ...)
	Gateway.Events.ScheduleEvent(event, delay, ...);
end

function Outside.Events.RegisterEvent(event, callback)
	Gateway.Events.RegisterEvent(event, callback);
end

function Outside.Events.UnregisterEvent(event, callback)
	Gateway.Events.UnregisterEvent(event, callback);
end

-- @return timestamp, name, station, path, igate, latitude, longitude, altitude, comment
function Gateway.Storage.GetLastPosition()
	local last_position = Gateway.Storage.Get('last_position');

	if not last_position then
		return 0, nil, nil, nil, nil, 0, 0, 0, nil;
	end

	local last_position_name      = Gateway.Storage.Get('last_position_name');
	local last_position_path      = Gateway.Storage.Get('last_position_path');
	local last_position_igate     = Gateway.Storage.Get('last_position_igate');
	local last_position_station   = Gateway.Storage.Get('last_position_station');
	local last_position_comment   = Gateway.Storage.Get('last_position_comment');
	local last_position_altitude  = Gateway.Storage.Get('last_position_altitude');
	local last_position_latitude  = Gateway.Storage.Get('last_position_latitude');
	local last_position_longitude = Gateway.Storage.Get('last_position_longitude');
	local last_position_timestamp = Gateway.Storage.Get('last_position_timestamp');

	return tonumber(last_position_timestamp), tostring(last_position_name), tostring(last_position_station), tostring(last_position_path), tostring(last_position_igate), tonumber(last_position_latitude), tonumber(last_position_longitude), tonumber(last_position_altitude), last_position_comment and tostring(last_position_comment) or nil;
end

function Gateway.Storage.SetLastPosition(name, station, path, igate, latitude, longitude, altitude, comment)
	Gateway.Storage.Set('last_position',           true);
	Gateway.Storage.Set('last_position_name',      tostring(name));
	Gateway.Storage.Set('last_position_path',      tostring(path));
	Gateway.Storage.Set('last_position_igate',     tostring(igate));
	Gateway.Storage.Set('last_position_station',   tostring(station));
	Gateway.Storage.Set('last_position_comment',   comment and tostring(comment) or nil);
	Gateway.Storage.Set('last_position_altitude',  tonumber(altitude));
	Gateway.Storage.Set('last_position_latitude',  tonumber(latitude));
	Gateway.Storage.Set('last_position_longitude', tonumber(longitude));
	Gateway.Storage.Set('last_position_timestamp', System.GetTimestamp());
end
