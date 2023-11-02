#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/Collections/ByteBuffer.hpp>

enum BYTE_BUFFER_ENDIANS : AL::uint8
{
	BYTE_BUFFER_ENDIAN_BIG,
	BYTE_BUFFER_ENDIAN_LITTLE,
	BYTE_BUFFER_ENDIAN_MACHINE
};

struct byte_buffer
{
	BYTE_BUFFER_ENDIANS                                endian;
	AL::uint32                                         buffer_capacity;
	AL::Collections::ByteBuffer<AL::Endians::Big>*     buffer_endian_big;
	AL::Collections::ByteBuffer<AL::Endians::Little>*  buffer_endian_little;
	AL::Collections::ByteBuffer<AL::Endians::Machine>* buffer_endian_machine;
};

byte_buffer*                               byte_buffer_allocate(BYTE_BUFFER_ENDIANS endian, AL::uint32 capacity)
{
	auto byte_buffer = new ::byte_buffer
	{
		.endian                = endian,
		.buffer_capacity       = capacity,
		.buffer_endian_big     = nullptr,
		.buffer_endian_little  = nullptr,
		.buffer_endian_machine = nullptr
	};

	switch (endian)
	{
		case BYTE_BUFFER_ENDIAN_BIG:
			byte_buffer->buffer_endian_big = new AL::Collections::ByteBuffer<AL::Endians::Big>(capacity);
			break;

		case BYTE_BUFFER_ENDIAN_LITTLE:
			byte_buffer->buffer_endian_little = new AL::Collections::ByteBuffer<AL::Endians::Little>(capacity);
			break;

		case BYTE_BUFFER_ENDIAN_MACHINE:
			byte_buffer->buffer_endian_machine = new AL::Collections::ByteBuffer<AL::Endians::Machine>(capacity);
			break;

		default:
			delete byte_buffer;
			return nullptr;
	}

	return byte_buffer;
}
void                                       byte_buffer_release(byte_buffer* byte_buffer)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				delete byte_buffer->buffer_endian_big;
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				delete byte_buffer->buffer_endian_little;
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				delete byte_buffer->buffer_endian_machine;
				break;
		}

		delete byte_buffer;
	}
}
void*                                      byte_buffer_get_buffer(byte_buffer* byte_buffer)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:     return const_cast<void*>(byte_buffer->buffer_endian_big->GetBuffer());
			case BYTE_BUFFER_ENDIAN_LITTLE:  return const_cast<void*>(byte_buffer->buffer_endian_little->GetBuffer());
			case BYTE_BUFFER_ENDIAN_MACHINE: return const_cast<void*>(byte_buffer->buffer_endian_machine->GetBuffer());
		}
	}

	return nullptr;
}
void*                                      byte_buffer_get_buffer_at(byte_buffer* byte_buffer, AL::uint32 offset)
{
	if (auto buffer = byte_buffer_get_buffer(byte_buffer))
		return &reinterpret_cast<AL::uint8*>(buffer)[offset];

	return nullptr;
}
BYTE_BUFFER_ENDIANS                        byte_buffer_get_endian(byte_buffer* byte_buffer)
{
	return byte_buffer ? byte_buffer->endian : BYTE_BUFFER_ENDIAN_MACHINE;
}
AL::uint32                                 byte_buffer_get_capacity(byte_buffer* byte_buffer)
{
	return byte_buffer ? byte_buffer->buffer_capacity : 0;
}
AL::uint32                                 byte_buffer_get_read_offset(byte_buffer* byte_buffer)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:     return static_cast<AL::uint32>(byte_buffer->buffer_endian_big->GetReadPosition());
			case BYTE_BUFFER_ENDIAN_LITTLE:  return static_cast<AL::uint32>(byte_buffer->buffer_endian_little->GetReadPosition());
			case BYTE_BUFFER_ENDIAN_MACHINE: return static_cast<AL::uint32>(byte_buffer->buffer_endian_machine->GetReadPosition());
		}
	}

	return 0;
}
AL::uint32                                 byte_buffer_get_write_offset(byte_buffer* byte_buffer)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:     return static_cast<AL::uint32>(byte_buffer->buffer_endian_big->GetWritePosition());
			case BYTE_BUFFER_ENDIAN_LITTLE:  return static_cast<AL::uint32>(byte_buffer->buffer_endian_little->GetWritePosition());
			case BYTE_BUFFER_ENDIAN_MACHINE: return static_cast<AL::uint32>(byte_buffer->buffer_endian_machine->GetWritePosition());
		}
	}

	return 0;
}
void                                       byte_buffer_set_read_offset(byte_buffer* byte_buffer, AL::uint32 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				byte_buffer->buffer_endian_big->SetReadPosition(value);
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				byte_buffer->buffer_endian_little->SetReadPosition(value);
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				byte_buffer->buffer_endian_machine->SetReadPosition(value);
				break;
		}
	}
}
void                                       byte_buffer_set_write_offset(byte_buffer* byte_buffer, AL::uint32 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				byte_buffer->buffer_endian_big->SetWritePosition(value);
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				byte_buffer->buffer_endian_little->SetWritePosition(value);
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				byte_buffer->buffer_endian_machine->SetWritePosition(value);
				break;
		}
	}
}
AL::Collections::Tuple<bool, byte_buffer*> byte_buffer_read(byte_buffer* byte_buffer, AL::uint32 size)
{
	AL::Collections::Tuple<bool, ::byte_buffer*> value(false, nullptr);

	if (byte_buffer != nullptr)
	{
		value.Set<1>(byte_buffer_allocate(byte_buffer_get_endian(byte_buffer), size));

		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->Read(byte_buffer_get_buffer(value.Get<1>()), size));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->Read(byte_buffer_get_buffer(value.Get<1>()), size));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->Read(byte_buffer_get_buffer(value.Get<1>()), size));
				break;

			default:
				delete value.Get<1>();
				value.Set<1>(nullptr);
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::int8>     byte_buffer_read_int8(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::int8> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadInt8(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadInt8(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadInt8(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::int16>    byte_buffer_read_int16(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::int16> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadInt16(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadInt16(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadInt16(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::int32>    byte_buffer_read_int32(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::int32> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadInt32(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadInt32(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadInt32(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::int64>    byte_buffer_read_int64(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::int64> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadInt64(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadInt64(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadInt64(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::uint8>    byte_buffer_read_uint8(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::uint8> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadUInt8(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadUInt8(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadUInt8(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::uint16>   byte_buffer_read_uint16(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::uint16> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadUInt16(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadUInt16(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadUInt16(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::uint32>   byte_buffer_read_uint32(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::uint32> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadUInt32(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadUInt32(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadUInt32(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::uint64>   byte_buffer_read_uint64(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::uint64> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadUInt64(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadUInt64(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadUInt64(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::Float>    byte_buffer_read_float(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::Float> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadFloat(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadFloat(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadFloat(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::Double>   byte_buffer_read_double(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::Double> value(false, 0);

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadDouble(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadDouble(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadDouble(value.Get<1>()));
				break;
		}
	}

	return value;
}
AL::Collections::Tuple<bool, AL::String>   byte_buffer_read_string(byte_buffer* byte_buffer)
{
	AL::Collections::Tuple<bool, AL::String> value(false, "");

	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				value.Set<0>(byte_buffer->buffer_endian_big->ReadString(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_LITTLE:
				value.Set<0>(byte_buffer->buffer_endian_little->ReadString(value.Get<1>()));
				break;

			case BYTE_BUFFER_ENDIAN_MACHINE:
				value.Set<0>(byte_buffer->buffer_endian_machine->ReadString(value.Get<1>()));
				break;
		}
	}

	return value;
}
bool                                       byte_buffer_write(byte_buffer* byte_buffer, ::byte_buffer* _byte_buffer, AL::uint32 _byte_buffer_size)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->Write(byte_buffer_get_buffer(_byte_buffer), _byte_buffer_size);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->Write(byte_buffer_get_buffer(_byte_buffer), _byte_buffer_size);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->Write(byte_buffer_get_buffer(_byte_buffer), _byte_buffer_size);
		}
	}

	return false;
}
bool                                       byte_buffer_write_int8(byte_buffer* byte_buffer, AL::int8 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteInt8(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteInt8(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteInt8(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_int16(byte_buffer* byte_buffer, AL::int16 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteInt16(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteInt16(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteInt16(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_int32(byte_buffer* byte_buffer, AL::int32 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteInt32(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteInt32(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteInt32(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_int64(byte_buffer* byte_buffer, AL::int64 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteInt64(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteInt64(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteInt64(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_uint8(byte_buffer* byte_buffer, AL::uint8 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteUInt8(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteUInt8(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteUInt8(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_uint16(byte_buffer* byte_buffer, AL::uint16 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteUInt16(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteUInt16(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteUInt16(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_uint32(byte_buffer* byte_buffer, AL::uint32 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteUInt32(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteUInt32(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteUInt32(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_uint64(byte_buffer* byte_buffer, AL::uint64 value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteUInt64(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteUInt64(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteUInt64(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_float(byte_buffer* byte_buffer, AL::Float value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteFloat(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteFloat(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteFloat(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_double(byte_buffer* byte_buffer, AL::Double value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteDouble(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteDouble(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteDouble(value);
		}
	}

	return false;
}
bool                                       byte_buffer_write_string(byte_buffer* byte_buffer, const char* value)
{
	if (byte_buffer != nullptr)
	{
		switch (byte_buffer->endian)
		{
			case BYTE_BUFFER_ENDIAN_BIG:
				return byte_buffer->buffer_endian_big->WriteString(value);

			case BYTE_BUFFER_ENDIAN_LITTLE:
				return byte_buffer->buffer_endian_little->WriteString(value);

			case BYTE_BUFFER_ENDIAN_MACHINE:
				return byte_buffer->buffer_endian_machine->WriteString(value);
		}
	}

	return false;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobal(BYTE_BUFFER_ENDIAN_BIG);
	LUA_APRS_IS_RegisterGlobal(BYTE_BUFFER_ENDIAN_LITTLE);
	LUA_APRS_IS_RegisterGlobal(BYTE_BUFFER_ENDIAN_MACHINE);

	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_allocate);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_release);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_get_buffer);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_get_buffer_at);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_get_endian);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_get_capacity);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_get_read_offset);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_get_write_offset);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_set_read_offset);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_set_write_offset);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_int8);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_int16);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_int32);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_int64);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_uint8);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_uint16);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_uint32);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_uint64);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_float);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_double);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_read_string);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_int8);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_int16);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_int32);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_int64);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_uint8);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_uint16);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_uint32);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_uint64);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_float);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_double);
	LUA_APRS_IS_RegisterGlobalFunction(byte_buffer_write_string);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobal(BYTE_BUFFER_ENDIAN_BIG);
	LUA_APRS_IS_UnregisterGlobal(BYTE_BUFFER_ENDIAN_LITTLE);
	LUA_APRS_IS_UnregisterGlobal(BYTE_BUFFER_ENDIAN_MACHINE);

	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_allocate);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_release);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_get_buffer);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_get_buffer_at);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_get_endian);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_get_capacity);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_get_read_offset);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_get_write_offset);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_set_read_offset);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_set_write_offset);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_int8);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_int16);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_int32);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_int64);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_uint8);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_uint16);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_uint32);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_uint64);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_float);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_double);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_read_string);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_int8);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_int16);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_int32);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_int64);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_uint8);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_uint16);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_uint32);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_uint64);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_float);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_double);
	LUA_APRS_IS_UnregisterGlobalFunction(byte_buffer_write_string);
});
