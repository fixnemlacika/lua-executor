ByteBuffer =
{
	Endians =
	{
		Big     = BYTE_BUFFER_ENDIAN_BIG,
		Little  = BYTE_BUFFER_ENDIAN_LITTLE,
		Machine = BYTE_BUFFER_ENDIAN_MACHINE
	},

	Allocate = function(endian, capacity)
		return byte_buffer_allocate(endian, tonumber(capacity));
	end,

	Release = function(buffer)
		byte_buffer_release(buffer);
	end,

	GetBuffer = function(buffer)
		return byte_buffer_get_buffer(buffer);
	end,

	GetBufferAt = function(buffer, offset)
		return byte_buffer_get_buffer_at(buffer, tonumber(offset));
	end,

	GetCapacity = function(buffer)
		return byte_buffer_get_capacity(buffer);
	end,

	GetReadOffset = function(buffer)
		return byte_buffer_get_read_offset(buffer);
	end,

	GetWriteOffset = function(buffer)
		return byte_buffer_get_write_offset(buffer);
	end,

	SetReadOffset = function(buffer, value)
		byte_buffer_set_read_offset(buffer, tonumber(value));
	end,

	SetWriteOffset = function(buffer, value)
		byte_buffer_set_write_offset(buffer, tonumber(value));
	end,

	-- @return success, value
	Read = function(buffer, byte_buffer_size)
		return byte_buffer_read(buffer, byte_buffer_size);
	end,
	-- @return success, value
	ReadInt8 = function(buffer)
		return byte_buffer_read_int8(buffer);
	end,
	-- @return success, value
	ReadInt16 = function(buffer)
		return byte_buffer_read_int16(buffer);
	end,
	-- @return success, value
	ReadInt32 = function(buffer)
		return byte_buffer_read_int32(buffer);
	end,
	-- @return success, value
	ReadInt64 = function(buffer)
		return byte_buffer_read_int64(buffer);
	end,
	-- @return success, value
	ReadUInt8 = function(buffer)
		return byte_buffer_read_uint8(buffer);
	end,
	-- @return success, value
	ReadUInt16 = function(buffer)
		return byte_buffer_read_uint16(buffer);
	end,
	-- @return success, value
	ReadUInt32 = function(buffer)
		return byte_buffer_read_uint32(buffer);
	end,
	-- @return success, value
	ReadUInt64 = function(buffer)
		return byte_buffer_read_uint64(buffer);
	end,
	-- @return success, value
	ReadFloat = function(buffer)
		return byte_buffer_read_float(buffer);
	end,
	-- @return success, value
	ReadDouble = function(buffer)
		return byte_buffer_read_double(buffer);
	end,
	-- @return success, value
	ReadString = function(buffer)
		return byte_buffer_read_string(buffer);
	end,

	-- @return success
	Write = function(buffer, byte_buffer, byte_buffer_size)
		return byte_buffer_write(buffer, byte_buffer, byte_buffer_size and tonumber(byte_buffer_size) or ByteBuffer.GetCapacity(byte_buffer));
	end,
	-- @return success
	WriteInt8 = function(buffer, value)
		return byte_buffer_write_int8(buffer, tonumber(value));
	end,
	-- @return success
	WriteInt16 = function(buffer, value)
		return byte_buffer_write_int16(buffer, tonumber(value));
	end,
	-- @return success
	WriteInt32 = function(buffer, value)
		return byte_buffer_write_int32(buffer, tonumber(value));
	end,
	-- @return success
	WriteInt64 = function(buffer, value)
		return byte_buffer_write_int64(buffer, tonumber(value));
	end,
	-- @return success
	WriteUInt8 = function(buffer, value)
		return byte_buffer_write_uint8(buffer, tonumber(value));
	end,
	-- @return success
	WriteUInt16 = function(buffer, value)
		return byte_buffer_write_uint16(buffer, tonumber(value));
	end,
	-- @return success
	WriteUInt32 = function(buffer, value)
		return byte_buffer_write_uint32(buffer, tonumber(value));
	end,
	-- @return success
	WriteUInt64 = function(buffer, value)
		return byte_buffer_write_uint64(buffer, tonumber(value));
	end,
	-- @return success
	WriteFloat = function(buffer, value)
		return byte_buffer_write_float(buffer, tonumber(value));
	end,
	-- @return success
	WriteDouble = function(buffer, value)
		return byte_buffer_write_double(buffer, tonumber(value));
	end,
	-- @return success
	WriteString = function(buffer, value)
		return byte_buffer_write_string(buffer, tostring(value));
	end
};
