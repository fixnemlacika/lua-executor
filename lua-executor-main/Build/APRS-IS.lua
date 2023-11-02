require('APRS');

APRS.IS =
{
	-- @return aprs_is
	Init = function(callsign, passcode, filter, digipath)
		if filter then
			filter = tostring(filter);
		end

		return aprs_is_init(tostring(callsign), tonumber(passcode), filter, tostring(digipath));
	end,

	Deinit = function(aprs_is)
		aprs_is_deinit(aprs_is);
	end,

	IsBlocking = function(aprs_is)
		return aprs_is_is_blocking(aprs_is);
	end,

	IsConnected = function(aprs_is)
		return aprs_is_is_connected(aprs_is);
	end,

	-- @return false on error
	Connect = function(aprs_is, host, port)
		return aprs_is_connect(aprs_is, tostring(host), tonumber(port));
	end,

	Disconnect = function(aprs_is)
		aprs_is_disconnect(aprs_is);
	end,

	-- @return would_block, packet
	-- @return packet == nil on connection closed
	ReadPacket = function(aprs_is)
		return aprs_is_read_packet(aprs_is);
	end,

	-- @return false on connection closed
	WritePacket = function(aprs_is, packet)
		return aprs_is_write_packet(aprs_is, packet);
	end,

	-- @return false on connection closed
	SendPacket = function(aprs_is, tocall, content)
		return aprs_is_send_packet(aprs_is, tostring(tocall), tostring(content));
	end,

	-- @return false on connection closed
	SendMessage = function(aprs_is, tocall, destination, content, ack)
		if ack then
			ack = tostring(ack);
		end

		return aprs_is_send_message(aprs_is, tostring(tocall), tostring(destination), tostring(content), ack);
	end,

	-- @return false on connection closed
	SendMessageAck = function(aprs_is, tocall, destination, value)
		return aprs_is_send_message_ack(aprs_is, tostring(tocall), tostring(destination), tostring(value));
	end,

	-- @return false on connection closed
	SendPosition = function(aprs_is, tocall, latitude, longitude, altitude, comment, symbol_table, symbol_table_key)
		if comment then
			comment = tostring(comment);
		end

		return aprs_is_send_position(aprs_is, tostring(tocall), tonumber(latitude), tonumber(longitude), tonumber(altitude), comment, tostring(symbol_table), tostring(symbol_table_key));
	end,

	SetBlocking = function(aprs_is, value)
		return aprs_is_set_blocking(aprs_is, value and true or false);
	end
};
