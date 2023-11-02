require('APRS-IS');

require('Extensions.Script');

Script.LoadExtension('Extensions.Loop');
Script.LoadExtension('Extensions.Mutex');
Script.LoadExtension('Extensions.System');
Script.LoadExtension('Extensions.Console');
Script.LoadExtension('Extensions.SQLite3');

Gateway          = {};
Gateway.Private  = {};
Gateway.INFINITE = 0x7FFFFFFFFFFFFFFF;

Script.ExitCodes.Storage            = {};
Script.ExitCodes.Storage.LoadFailed = Script.ExitCodes.UserDefined + 1;
Script.ExitCodes.UserDefined        = Script.ExitCodes.UserDefined + 1000;

function Gateway.Init(aprs_callsign, aprs_is_passcode, aprs_path, aprs_is_host, aprs_is_port, aprs_is_filter, database_path)
	Gateway.Private.Config                       = {};
	Gateway.Private.Config.APRS                  = {};
	Gateway.Private.Config.APRS.Path             = aprs_path;
	Gateway.Private.Config.APRS.ToCall           = 'APRS';
	Gateway.Private.Config.APRS.Callsign         = aprs_callsign;
	Gateway.Private.Config.APRS.IS               = {};
	Gateway.Private.Config.APRS.IS.Host          = aprs_is_host;
	Gateway.Private.Config.APRS.IS.Port          = aprs_is_port;
	Gateway.Private.Config.APRS.IS.Filter        = aprs_is_filter;
	Gateway.Private.Config.APRS.IS.Passcode      = aprs_is_passcode;
	Gateway.Private.Config.Database              = {};
	Gateway.Private.Config.Database.Path         = database_path;

	if not Gateway.Private.Database.Open(Gateway.Private.Config.Database.Path) then
		Console.WriteLine('Gateway', 'Error opening database');
		Script.SetExitCode(Script.ExitCodes.SQLite3.OpenFailed);
		return false;
	end

	if not Gateway.Private.Storage.Load() then
		Console.WriteLine('Gateway', 'Error loading storage');
		Gateway.Private.Database.Close();
		Script.SetExitCode(Script.ExitCodes.Storage.LoadFailed);
		return false;
	end

	Gateway.Events.RegisterEvent(Gateway.Events.OnReceivePacket, function(packet)
		local packet_sender = APRS.Packet.GetSender(packet);
		local packet_path   = APRS.Packet.GetDigiPath(packet);
		local packet_igate  = APRS.Packet.GetIGate(packet);

		if APRS.Packet.IsMessage(packet) then
			local packet_message = APRS.Message.Decode(packet);

			if packet_message then
				if APRS.Message.GetDestination(packet_message) == Gateway.Private.Config.APRS.Callsign then
					local packet_message_ack     = APRS.Message.GetAck(packet_message);
					local packet_message_content = APRS.Message.GetContent(packet_message);

					if not string.match(packet_message_content, 'ack[^{]%S?%S?%S?%S?') then
						if packet_message_ack then
							Gateway.Private.APRS.IS.SendMessageAck(packet_sender, packet_message_ack);
						end

						local command_prefix, command_params = string.match(packet_message_content, '^([^ ]+) (.+)$');

						if not command_prefix then
							command_prefix = string.match(packet_message_content, '^([^ ]+)$');
						end

						if not command_prefix or not Gateway.Private.Commands.Execute(packet_sender, command_prefix, command_params) then
							Gateway.Events.ExecuteEvent(Gateway.Events.OnReceiveMessage, packet_sender, packet_path, packet_igate, packet_message_content);
						end
					end
				end

				APRS.Message.Deinit(packet_message);
			end
		elseif APRS.Packet.IsPosition(packet) then
			local packet_position = APRS.Position.Decode(packet);

			if packet_position then
				local packet_position_altitude  = APRS.Position.GetAltitude(packet_position);
				local packet_position_latitude  = APRS.Position.GetLatitude(packet_position);
				local packet_position_longitude = APRS.Position.GetLongitude(packet_position);
				local packet_position_comment   = APRS.Position.GetComment(packet_position);

				APRS.Position.Deinit(packet_position);

				Gateway.Events.ExecuteEvent(Gateway.Events.OnReceivePosition, packet_sender, packet_path, packet_igate, packet_position_latitude, packet_position_longitude, packet_position_altitude, packet_position_comment);
			end
		end
	end);

	Gateway.Events.RegisterEvent(Gateway.Private.Events.SaveStorage, function()
		if not Gateway.Private.Storage.Save() then
			Console.WriteLine('Gateway', 'Error saving storage');
		end

		Gateway.Events.ScheduleEvent(Gateway.Private.Events.SaveStorage, 30);
	end)

	return true;
end

function Gateway.Run(interval_ms)
	if not Gateway.Private.Config then
		return false;
	end

	if not interval_ms then
		interval_ms = 200;
	end

	if not Gateway.Private.APRS.IS.Connect(Gateway.Private.Config.APRS.IS.Host, Gateway.Private.Config.APRS.IS.Port, Gateway.Private.Config.APRS.Callsign, Gateway.Private.Config.APRS.IS.Passcode, Gateway.Private.Config.APRS.Path, Gateway.Private.Config.APRS.IS.Filter) then
		Gateway.Private.Database.Close();
		Script.SetExitCode(Script.ExitCodes.APRS.IS.ConnectionFailed);
		return false;
	end

	Gateway.Events.ScheduleEvent(Gateway.Private.Events.SaveStorage, 30);

	Gateway.Private.RunLoop(interval_ms);

	Gateway.Private.APRS.IS.Disconnect();

	if not Gateway.Private.Storage.Save() then
		Console.WriteLine('Gateway', 'Error saving storage');
	end

	Gateway.Private.Database.Close();

	return true;
end

function Gateway.Private.RunLoop(interval_ms)
	local loop = Loop.Init(interval_ms);

	if loop then
		if Gateway.Private.OnUpdate(0) then
			repeat
				local loop_delta = Loop.Sync(loop);
			until not Gateway.Private.OnUpdate(loop_delta);
		end

		Loop.Deinit(loop);
	end
end

function Gateway.Private.OnUpdate(delta_ms)
	if not Gateway.Private.APRS.IS.IsConnected() then
		Console.WriteLine('Gateway', 'Not connected - Exiting');
		return false;
	end

	repeat
		local aprs_is_would_block, aprs_packet = Gateway.Private.APRS.IS.ReceivePacket();

		if not aprs_is_would_block and not aprs_packet then
			Script.SetExitCode(Script.ExitCodes.APRS.IS.ConnectionClosed);
			return false;
		end

		if not aprs_is_would_block and aprs_packet then
			Gateway.Events.ExecuteEvent(Gateway.Events.OnReceivePacket, aprs_packet);
		end
	until aprs_is_would_block or not aprs_packet;

	Gateway.Private.Events.UpdateScheduledEvents(delta_ms);

	Gateway.Events.ExecuteEvent(Gateway.Events.OnUpdate, delta_ms);

	return true;
end

Gateway.Events                     = {};
Gateway.Events.OnEvent             = {}; -- function(sender, prefix, params)
Gateway.Events.OnUpdate            = {}; -- function(delta_ms)
Gateway.Events.OnConnected         = {}; -- function(host, port, callsign)
Gateway.Events.OnDisconnected      = {}; -- function()
Gateway.Events.OnReceivePacket     = {}; -- function(packet)
Gateway.Events.OnReceiveMessage    = {}; -- function(station, path, igate, content)
Gateway.Events.OnReceivePosition   = {}; -- function(station, path, igate, latitude, longitude, altitude, comment)
Gateway.Private.Events             = {};
Gateway.Private.Events.SaveStorage = {}; -- function()

function Gateway.Events.ExecuteEvent(event, ...)
	for event_index, event_callback in ipairs(event) do
		event_callback(...);
	end
end

function Gateway.Events.ScheduleEvent(event, delay, ...)
	if not Gateway.Private.ScheduledEvents then
		Gateway.Private.ScheduledEvents = {};
	end

	if Gateway.Private.ScheduledEventTimer == nil then
		Gateway.Private.ScheduledEventTimer = 0;
	end

	local timestamp = Gateway.Private.ScheduledEventTimer + (delay * 1000);

	if not Gateway.Private.ScheduledEvents[timestamp] then
		Gateway.Private.ScheduledEvents[timestamp] = {};
	end

	table.insert(Gateway.Private.ScheduledEvents[timestamp], { event, table.pack(...) });
end

function Gateway.Events.RegisterEvent(event, callback)
	table.insert(event, callback);
end

function Gateway.Events.UnregisterEvent(event, callback)
	for event_index, event_callback in ipairs(event) do
		if event_callback == callback then
			table.remove(event, event_index);
			break;
		end
	end
end

function Gateway.Private.Events.UpdateScheduledEvents(delta_ms)
	if Gateway.Private.ScheduledEventTimer == nil then
		Gateway.Private.ScheduledEventTimer = 0;
	end

	Gateway.Private.ScheduledEventTimer = Gateway.Private.ScheduledEventTimer + delta_ms;

	while Gateway.Private.Events.ExecuteScheduledEvents() do
		-- do nothing
	end
end

function Gateway.Private.Events.ExecuteScheduledEvents()
	if Gateway.Private.ScheduledEvents then
		for timestamp, events in pairs(Gateway.Private.ScheduledEvents) do
			if timestamp <= Gateway.Private.ScheduledEventTimer then
				for event_index, event in ipairs(events) do
					Gateway.Events.ExecuteEvent(event[1], table.unpack(event[2]));
				end

				Gateway.Private.ScheduledEvents[timestamp] = nil;
				return true;
			end
		end
	end

	return false
end

Gateway.APRS                   = {};
Gateway.APRS.IS                = {};
Gateway.Private.APRS           = {};
Gateway.Private.APRS.IS        = {};
Gateway.Private.APRS.IS.Handle = nil;

function Gateway.Private.APRS.IS.IsConnected()
	return Gateway.Private.APRS.IS.Handle ~= nil;
end

function Gateway.Private.APRS.IS.Connect(host, port, callsign, passcode, path, filter)
	if Gateway.Private.APRS.IS.IsConnected() then
		Console.WriteLine('Gateway.APRS.IS', 'Already connected');
		return false;
	end

	Gateway.Private.APRS.IS.Handle = APRS.IS.Init(callsign, passcode, filter, path);

	if not Gateway.Private.APRS.IS.Handle then
		Console.WriteLine('Gateway.APRS.IS', 'Init failed');
		return false;
	end

	if not APRS.IS.Connect(Gateway.Private.APRS.IS.Handle, host, port) then
		Console.WriteLine('Gateway.APRS.IS', 'Connection failed');
		APRS.IS.Deinit(Gateway.Private.APRS.IS.Handle);
		Gateway.Private.APRS.IS.Handle = nil;
		return false;
	end

	Console.WriteLine('Gateway.APRS.IS', string.format('Connected to %s:%u as %s', host, port, callsign));

	Gateway.Events.ExecuteEvent(Gateway.Events.OnConnected, host, port, callsign);

	return true;
end

function Gateway.Private.APRS.IS.Disconnect()
	if Gateway.Private.APRS.IS.IsConnected() then
		Gateway.Events.ExecuteEvent(Gateway.Events.OnDisconnected);
		APRS.IS.Disconnect(Gateway.Private.APRS.IS.Handle);
		APRS.IS.Deinit(Gateway.Private.APRS.IS.Handle);
		Gateway.Private.APRS.IS.Handle = nil;
	end
end

-- @return false on connection closed
function Gateway.APRS.IS.SendPacket(content)
	if not Gateway.Private.APRS.IS.IsConnected() then
		Console.WriteLine('Gateway.APRS.IS', 'Not connected');
		return false;
	end

	if not APRS.IS.SendPacket(Gateway.Private.APRS.IS.Handle, Gateway.Private.Config.APRS.ToCall, content) then
		Console.WriteLine('Gateway.APRS.IS', 'Connection closed');
		Gateway.Private.APRS.IS.Disconnect();
		return false;
	end

	return true;
end

-- @return false on connection closed
function Gateway.APRS.IS.SendMessage(destination, content, ack)
	if not Gateway.Private.APRS.IS.IsConnected() then
		Console.WriteLine('Gateway.APRS.IS', 'Not connected');
		return false;
	end

	if not APRS.IS.SendMessage(Gateway.Private.APRS.IS.Handle, Gateway.Private.Config.APRS.ToCall, destination, content, ack) then
		Gateway.Private.APRS.IS.Disconnect();
		return false;
	end

	return true;
end

-- @return false on connection closed
function Gateway.Private.APRS.IS.SendMessageAck(destination, value)
	if not Gateway.Private.APRS.IS.IsConnected() then
		return false;
	end

	if not APRS.IS.SendMessageAck(Gateway.Private.APRS.IS.Handle, Gateway.Private.Config.APRS.ToCall, destination, value) then
		Console.WriteLine('Gateway.APRS.IS', 'Connection closed');
		Gateway.Private.APRS.IS.Disconnect();
		return false;
	end

	return true;
end

-- @return would_block, packet
-- @return false, nil on connection closed
function Gateway.Private.APRS.IS.ReceivePacket()
	local would_block, packet = APRS.IS.ReadPacket(Gateway.Private.APRS.IS.Handle);

	if not would_block and not packet then
		Console.WriteLine('Gateway.APRS.IS', 'Connection closed');
		Gateway.Private.APRS.IS.Disconnect();
		return false, nil;
	end

	return would_block, packet;
end

Gateway.Commands         = {};
Gateway.Private.Commands = {};

-- @param handler function(sender, prefix, params)
function Gateway.Commands.SetHandler(prefix, handler)
	if not Gateway.Private.Commands.Handlers then
		Gateway.Private.Commands.Handlers = {};
	end

	Gateway.Private.Commands.Handlers[string.lower(prefix)] = handler;
end

function Gateway.Commands.RemoveHandler(prefix)
	if Gateway.Private.Commands.Handlers then
		Gateway.Private.Commands.Handlers[string.lower(prefix)] = nil;
	end
end

function Gateway.Private.Commands.Execute(sender, prefix, params)
	if Gateway.Private.Commands.Handlers then
		local command_handler = Gateway.Private.Commands.Handlers[string.lower(prefix)];

		if command_handler then
			local command_params = {};

			if params then
				for param in string.gmatch(params, '[^ ]+') do
					table.insert(command_params, param);
				end
			end

			Gateway.Events.ExecuteEvent(Gateway.Events.OnEvent, sender, prefix, command_params);

			command_handler(sender, prefix, command_params);

			return true;
		end
	end

	return false;
end

function Gateway.Private.Commands.Enumerate(callback)
	for command_prefix, command_handler in pairs(Gateway.Private.Commands.Handlers) do
		callback(command_prefix, command_handler);
	end
end

Gateway.Private.Database        = {};
Gateway.Private.Database.Handle = nil;

function Gateway.Private.Database.IsOpen()
	return Gateway.Private.Database.Handle ~= nil;
end

function Gateway.Private.Database.GetColumnName(query_result_row, index)
	return SQLite3.QueryResultRow.GetColumnName(query_result_row, index);
end

function Gateway.Private.Database.GetColumnValue(query_result_row, index)
	return SQLite3.QueryResultRow.GetColumnValue(query_result_row, index);
end

function Gateway.Private.Database.TableExists(name)
	if not Gateway.Private.Database.IsOpen() then
		Console.WriteLine('Gateway.Database', 'Not open');
		return false;
	end

	local table_exists = false;

	local function callback(query_result_row, query_result_row_index, query_result_row_column_count)
		if query_result_row_column_count == 1 then
			table_exists = true;
		end
	end

	if not Gateway.Private.Database.ExecuteQuery(string.format('SELECT name FROM sqlite_master WHERE type=\'table\' AND name=\'%s\'', name), callback) then
		Console.WriteLine('Gateway.Database', 'Error reading sqlite_master');
		return false;
	end

	return table_exists;
end

function Gateway.Private.Database.Open(path)
	if Gateway.Private.Database.IsOpen() then
		Console.WriteLine('Gateway.Database', 'Already open');
		return false;
	end

	Gateway.Private.Database.Handle = SQLite3.Init(path, SQLite3.Flags.Create | SQLite3.Flags.ReadWrite);

	if not Gateway.Private.Database.Handle then
		Console.WriteLine('Gateway.Database', 'Error initializing SQLite3');
		return false;
	end

	if not SQLite3.Open(Gateway.Private.Database.Handle) then
		Console.WriteLine('Gateway.Database', string.format('Error opening SQLite3 database \'%s\'', path));
		SQLite3.Deinit(Gateway.Private.Database.Handle);
		Gateway.Private.Database.Handle = nil;
		return false;
	end

	return true;
end

function Gateway.Private.Database.Close()
	if Gateway.Private.Database.IsOpen() then
		SQLite3.Deinit(Gateway.Private.Database.Handle);
		Gateway.Private.Database.Handle = nil;
	end
end

-- callback function(query_result_row, query_result_row_index, query_result_row_column_count)
function Gateway.Private.Database.ExecuteQuery(query, callback)
	if not Gateway.Private.Database.IsOpen() then
		Console.WriteLine('Gateway.Database', 'Not open');
		return false;
	end

	return SQLite3.ExecuteQuery(Gateway.Private.Database.Handle, query, function(sqlite3, query_result_row, query_result_row_index)
		local query_result_row_column_count = SQLite3.QueryResultRow.GetColumnCount(query_result_row);

		callback(query_result_row, query_result_row_index, query_result_row_column_count);
	end);
end

function Gateway.Private.Database.ExecuteNonQuery(query)
	if not Gateway.Private.Database.IsOpen() then
		Console.WriteLine('Gateway.Database', 'Not open');
		return false;
	end

	return SQLite3.ExecuteNonQuery(Gateway.Private.Database.Handle, query);
end

Gateway.Storage                    = {};
Gateway.Private.Storage            = {};
Gateway.Private.Storage.TableNames =
{
	Public  = 'storage_public',
	Private = 'storage_private'
};

function Gateway.Storage.Get(key)
	if not Gateway.Private.Storage.Public then
		return nil;
	end

	return Gateway.Private.Storage.Public[key];
end

function Gateway.Storage.Set(key, value)
	if not Gateway.Private.Storage.Public then
		Gateway.Private.Storage.Public = {};
	end

	Gateway.Private.Storage.Public[key] = value;
end

function Gateway.Private.Storage.Get(key)
	if not Gateway.Private.Storage.Private then
		return nil;
	end

	return Gateway.Private.Storage.Private[key];
end

function Gateway.Private.Storage.Set(key, value)
	if not Gateway.Private.Storage.Private then
		Gateway.Private.Storage.Private = {};
	end

	Gateway.Private.Storage.Private[key] = value;
end

function Gateway.Private.Storage.Load()
	local public_success, public_table   = Gateway.Private.Storage.LoadTable(Gateway.Private.Storage.TableNames.Public);
	local private_success, private_table = Gateway.Private.Storage.LoadTable(Gateway.Private.Storage.TableNames.Private);

	if not public_success then
		Console.WriteLine('Gateway.Storage', 'Error loading public');
		return false;
	end

	if not private_success then
		Console.WriteLine('Gateway.Storage', 'Error loading private');
		return false;
	end

	Gateway.Private.Storage.Public  = public_table;
	Gateway.Private.Storage.Private = private_table;

	return true;
end

function Gateway.Private.Storage.Save()
	if not Gateway.Private.Storage.SaveTable(Gateway.Private.Storage.TableNames.Public, Gateway.Private.Storage.Public) then
		Console.WriteLine('Gateway.Storage', 'Error saving public');
		return false;
	end

	if not Gateway.Private.Storage.SaveTable(Gateway.Private.Storage.TableNames.Private, Gateway.Private.Storage.Private) then
		Console.WriteLine('Gateway.Storage', 'Error saving private');
		return false;
	end

	return true;
end

-- @return success, table
function Gateway.Private.Storage.LoadTable(name)
	local table = nil;

	if not Gateway.Private.Database.TableExists(name) then
		return true, table;
	else
		local function callback(query_result_row, query_result_row_index, query_result_row_column_count)
			table = {};

			for i = 1, query_result_row_column_count do
				local column_name         = Gateway.Private.Database.GetColumnName(query_result_row, i);
				local column_value        = Gateway.Private.Database.GetColumnValue(query_result_row, i);
				local column_value_prefix = string.sub(column_value, 1, 1);
				local column_value_data   = string.sub(column_value, 2);

				if column_value_prefix == 's' then
					table[column_name] = tostring(column_value_data);
				elseif (column_value_prefix == 'i') or (column_value_prefix == 'f') then
					table[column_name] = tonumber(column_value_data);
				elseif column_value_prefix == 'b' then
					table[column_name] = (tonumber(column_value_data) ~= 0) and true or false;
				end
			end
		end

		if not Gateway.Private.Database.ExecuteQuery(string.format('SELECT * FROM \'%s\'', name), callback) then
			Console.WriteLine('Gateway.Storage', string.format('Error reading table %s', name));

			return false, table;
		end
	end

	return true, table;
end

function Gateway.Private.Storage.SaveTable(name, value)
	if value then
		local columns       = '';
		local column_count  = 1;
		local column_names  = '';
		local column_values = '';

		for key, value in pairs(value) do
			if type(key) == 'string' then
				if column_count > 1 then
					columns       = columns .. ', ';
					column_names  = column_names .. ', ';
					column_values = column_values .. ', ';
				end

				columns      = columns .. string.format("'%s'", key);
				column_count = column_count + 1;
				column_names = column_names .. string.format("'%s' TEXT NOT NULL", key);

				local column_value_type = type(value);

				if column_value_type == 'string' then
					column_values = column_values .. string.format("'s%s'", value);
				elseif column_value_type == 'number' then
					if (value % 1) == 0 then
						column_values = column_values .. string.format("'i%i'", value);
					else
						column_values = column_values .. string.format("'f%f'", value);
					end
				elseif column_value_type == 'boolean' then
					column_values = column_values .. string.format("'b%u'", value and 1 or 0);
				end
			end
		end

		if not Gateway.Private.Database.ExecuteNonQuery(string.format('DROP TABLE IF EXISTS \'%s\'', name)) then
			Console.WriteLine('Gateway.Storage', string.format('Error dropping table %s', name));

			return false;
		end

		if not Gateway.Private.Database.ExecuteNonQuery(string.format('CREATE TABLE \'%s\' (%s)', name, column_names)) then
			Console.WriteLine('Gateway.Storage', string.format('Error creating table %s', name));

			return false;
		end

		if not Gateway.Private.Database.ExecuteNonQuery(string.format('INSERT INTO %s (%s) VALUES(%s)', name, columns, column_values)) then
			Console.WriteLine('Gateway.Storage', string.format('Error writing table %s', name));

			return false;
		end
	end

	return true;
end

Gateway.Utility = {};

function Gateway.Utility.GetDistanceBetweenPoints(latitude1, longitude1, latitude2, longitude2)
	return Gateway.Utility.GetDistanceBetweenPoints3D(latitude1, longitude1, 0, latitude2, longitude2, 0);
end

function Gateway.Utility.GetDistanceBetweenPoints3D(latitude1, longitude1, altitude1, latitude2, longitude2, altitude2)
	local latitude_delta  = math.rad(latitude2 - latitude1);
	local longitude_delta = math.rad(longitude2 - longitude1);
	local latitude_1      = math.rad(latitude1);
	local latitude_2      = math.rad(latitude2);
	local a               = math.sin(latitude_delta / 2) * math.sin(latitude_delta / 2) + math.sin(longitude_delta / 2) * math.sin(longitude_delta / 2) * math.cos(latitude_1) * math.cos(latitude_2);
	local distance        = 2 * math.atan(math.sqrt(a), math.sqrt(1 - a));
	local distance_z      = 0;

	if altitude1 >= altitude2 then
		distance_z = altitude1 - altitude2;
	else
		distance_z = altitude2 - altitude1;
	end

	return ((distance * 6371) * 3280.84) + distance_z;
end
