require('Extensions.ByteBuffer');

Socket =
{
	Types =
	{
		TCP = SOCKET_TYPE_TCP,
		UDP = SOCKET_TYPE_UDP,
	},

	Open = function(type)
		return socket_open(type);
	end,

	Close = function(socket)
		socket_close(socket);
	end,

	IsBlocking = function(socket)
		return socket_is_blocking(socket);
	end,

	IsConnected = function(socket)
		return socket_is_connected(socket);
	end,

	IsListening = function(socket)
		return socket_is_listening(socket);
	end,

	GetType = function(socket)
		return socket_get_type(socket);
	end,

	SetBlocking = function(socket, value)
		return socket_set_blocking(socket, value and true or false);
	end,

	Listen = function(socket, host, port)
		return socket_listen(socket, tostring(host), tonumber(port));
	end,

	-- @return socket, would_block
	Accept = function(socket)
		return socket_accept(socket);
	end,

	Connect = function(socket, host, port)
		return socket_connect(socket, tostring(host), tonumber(port));
	end,

	-- @return connection_closed, number_of_bytes_sent
	Send = function(socket, byte_buffer, byte_buffer_size)
		return socket_send(socket, ByteBuffer.GetBuffer(byte_buffer), byte_buffer_size and tonumber(byte_buffer_size) or ByteBuffer.GetCapacity(byte_buffer));
	end,

	-- @return connection_closed
	SendAll = function(socket, byte_buffer, byte_buffer_size)
		return socket_send_all(socket, ByteBuffer.GetBuffer(byte_buffer), byte_buffer_size and tonumber(byte_buffer_size) or ByteBuffer.GetCapacity(byte_buffer));
	end,

	-- @return connection_closed, would_block, byte_buffer
	Receive = function(socket, byte_buffer_endian, byte_buffer_capacity)
		local byte_buffer                                              = ByteBuffer.Allocate(byte_buffer_endian, byte_buffer_capacity);
		local connection_closed, would_block, number_of_bytes_received = socket_receive(socket, ByteBuffer.GetBuffer(byte_buffer), tonumber(byte_buffer_capacity));

		if connection_closed or would_block then
			ByteBuffer.Release(byte_buffer);
			return connection_closed, would_block, nil;
		end

		ByteBuffer.SetWriteOffset(byte_buffer, number_of_bytes_received);

		return connection_closed, would_block, byte_buffer;
	end,

	-- @return connection_closed, byte_buffer
	ReceiveAll = function(socket, byte_buffer_endian, byte_buffer_capacity)
		local byte_buffer                                 = ByteBuffer.Allocate(byte_buffer_endian, byte_buffer_capacity);
		local connection_closed, number_of_bytes_received = socket_receive_all(socket, ByteBuffer.GetBuffer(byte_buffer), tonumber(byte_buffer_capacity));

		if connection_closed then
			ByteBuffer.Release(byte_buffer);
			return connection_closed, nil;
		end

		ByteBuffer.SetWriteOffset(byte_buffer, number_of_bytes_received);

		return connection_closed, byte_buffer;
	end
};
