APRS =
{
	Packet =
	{
		Decode = function(string)
			return aprs_packet_decode(tostring(string));
		end,

		IsMessage = function(packet)
			return aprs_packet_is_message(packet);
		end,
		IsPosition = function(packet)
			return aprs_packet_is_position(packet);
		end,

		Init = function(sender, tocall, digipath, content)
			return aprs_packet_init(tostring(sender), tostring(tocall), tostring(digipath), tostring(content));
		end,

		Deinit = function(packet)
			aprs_packet_deinit(packet);
		end,

		GetIGate = function(packet)
			return aprs_packet_get_igate(packet);
		end,
		SetIGate = function(packet, value)
			return aprs_packet_set_igate(packet, tostring(value));
		end,

		GetQFlag = function(packet)
			return aprs_packet_get_qflag(packet);
		end,
		SetQFlag = function(packet, value)
			return aprs_packet_set_qflag(packet, tostring(value));
		end,

		GetToCall = function(packet)
			return aprs_packet_get_tocall(packet);
		end,
		SetToCall = function(packet, value)
			return aprs_packet_set_tocall(packet, tostring(value));
		end,

		GetSender = function(packet)
			return aprs_packet_get_sender(packet);
		end,
		SetSender = function(packet, value)
			return aprs_packet_set_sender(packet, tostring(value));
		end,

		GetContent = function(packet)
			return aprs_packet_get_content(packet);
		end,
		SetContent = function(packet, value)
			return aprs_packet_set_content(packet, tostring(value));
		end,

		GetDigiPath = function(packet)
			return aprs_packet_get_digipath(packet);
		end,
		SetDigiPath = function(packet, value)
			return aprs_packet_set_digipath(packet, tostring(value));
		end
	},

	Message =
	{
		Init = function(content, destination, ack)
			if ack then
				ack = tostring(ack);
			end

			return aprs_message_init(tostring(content), tostring(destination), ack);
		end,

		Decode = function(packet)
			return aprs_message_decode(packet);
		end,

		Deinit = function(message)
			aprs_message_deinit(message);
		end,

		GetAck = function(message)
			return aprs_message_get_ack(message);
		end,
		SetAck = function(message, value)
			if value then
				value = tostring(value);
			end

			aprs_message_set_ack(message, value);
		end,

		GetContent = function(message)
			return aprs_message_get_content(message);
		end,
		SetContent = function(message, value)
			aprs_message_set_content(message, tostring(value));
		end,

		GetDestination = function(message)
			return aprs_message_get_destination(message);
		end,
		SetDestination = function(message, value)
			aprs_message_set_destination(message, tostring(value));
		end
	},

	Position =
	{
		Init = function(latitude, longitude, altitude, symbol_table, symbol_table_key)
			return aprs_position_init(tonumber(latitude), tonumber(longitude), tonumber(altitude), tostring(symbol_table), tostring(symbol_table_key));
		end,

		Decode = function(packet)
			return aprs_position_decode(packet);
		end,

		Deinit = function(position)
			aprs_position_deinit(position);
		end,

		GetAltitude = function(position)
			return aprs_position_get_altitude(position);
		end,
		SetAltitude = function(position, value)
			aprs_position_set_altitude(position, value);
		end,

		GetLatitude = function(position)
			return aprs_position_get_latitude(position);
		end,
		SetLatitude = function(position, value)
			aprs_position_set_latitude(position, value);
		end,

		GetLongitude = function(position)
			return aprs_position_get_longitude(position);
		end,
		SetLongitude = function(position, value)
			aprs_position_set_longitude(position, value);
		end,

		GetComment = function(position)
			return aprs_position_get_comment(position);
		end,
		SetComment = function(position, value)
			aprs_position_set_comment(position, value);
		end,

		GetSymbolTable = function(position)
			return aprs_position_get_symbol_table(position);
		end,
		SetSymbolTable = function(position, value)
			aprs_position_set_symbol_table(position, value);
		end,

		GetSymbolTableKey = function(position)
			return aprs_position_get_symbol_table_key(position);
		end,
		SetSymbolTableKey = function(position, value)
			aprs_position_set_symbol_table_key(position, value);
		end
	}
};
