#include "API.hpp"

#include <APRS-IS.hpp>

#include <AL/OS/Console.hpp>
#include <AL/OS/Process.hpp>

#include <AL/Network/DNS.hpp>

typedef APRS::Packet   aprs_packet;
typedef APRS::Message  aprs_message;
typedef APRS::Position aprs_position;

struct aprs_is
{
	APRS::IS::Client client;
	aprs_packet      packet;
	bool             packet_is_set;
	AL::String       digipath;
};

aprs_is*                                   aprs_is_init(const char* callsign, AL::uint16 passcode, const char* filter, const char* digipath)
{
	auto is = new aprs_is
	{
		.client   = APRS::IS::Client(AL::String(callsign), passcode, AL::String(filter ? filter : "")),
		.digipath = digipath,
	};

	is->client.OnReceivePacket.Register([is](const APRS::Packet& packet)
	{
		is->packet        = packet;
		is->packet_is_set = true;
	});

	return is;
}
void                                       aprs_is_deinit(aprs_is* is)
{
	delete is;
}
bool                                       aprs_is_is_blocking(aprs_is* is)
{
	return is->client.IsBlocking();
}
bool                                       aprs_is_is_connected(aprs_is* is)
{
	if (is == nullptr)
	{

		return false;
	}

	return is->client.IsConnected();
}
bool                                       aprs_is_connect(aprs_is* is, const char* host, AL::uint16 port)
{
	if (is == nullptr)
	{

		return false;
	}

	AL::Network::IPEndPoint remoteEP = { .Port = port };

	try
	{
		if (!AL::Network::DNS::Resolve(remoteEP.Host, host))
		{

			return false;
		}
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	try
	{
		is->client.Connect(
			remoteEP
		);
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	return true;
}
void                                       aprs_is_disconnect(aprs_is* is)
{
	if (is != nullptr)
	{
		is->client.Disconnect();
	}
}
// @return would_block, packet
AL::Collections::Tuple<bool, aprs_packet*> aprs_is_read_packet(aprs_is* is)
{
	AL::Collections::Tuple<bool, aprs_packet*> result(false, nullptr);

	if (is != nullptr)
	{
		try
		{
			is->client.Update();
		}
		catch (const AL::Exception& exception)
		{
			AL::OS::Console::WriteException(
				exception
			);
		}

		result.Set<0>(!is->packet_is_set);

		if (is->packet_is_set)
		{
			is->packet_is_set = false;

			result.Set<1>(&is->packet);
		}
	}

	return result;
}
bool                                       aprs_is_write_packet(aprs_is* is, aprs_packet* packet)
{
	try
	{
		if (!is->client.SendPacket(*packet))
		{
			aprs_is_disconnect(is);

			return false;
		}
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	return true;
}
bool                                       aprs_is_send_packet(aprs_is* is, const char* tocall, const char* content)
{
	aprs_packet packet =
	{
		.ToCall   = tocall,
		.Sender   = is->client.GetCallsign(),
		.Content  = content,
		.DigiPath = is->digipath
	};

	return aprs_is_write_packet(is, &packet);
}
bool                                       aprs_is_send_message(aprs_is* is, const char* tocall, const char* destination, const char* content, const char* ack)
{
	APRS::Message message =
	{
		.Ack         = ack ? ack : "",
		.Content     = content,
		.Destination = destination
	};

	try
	{
		if (!is->client.SendMessage(message, tocall, is->digipath))
		{
			aprs_is_disconnect(is);

			return false;
		}
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	return true;
}
bool                                       aprs_is_send_message_ack(aprs_is* is, const char* tocall, const char* destination, const char* value)
{
	APRS::Message message =
	{
		.Content     = AL::String::Format("ack%s", value),
		.Destination = destination
	};

	auto packet = message.Encode(tocall, is->client.GetCallsign(), is->digipath);

	return aprs_is_write_packet(is, &packet);
}
bool                                       aprs_is_send_position(aprs_is* is, const char* tocall, AL::Float latitude, AL::Float longitude, AL::uint16 altitude, const char* comment, const char* symbol_table, const char* symbol_table_key)
{
	APRS::Position position =
	{
		.Altitude       = altitude,
		.Latitude       = latitude,
		.Longitude      = longitude,
		.Comment        = comment ? comment : "",
		.SymbolTable    = *symbol_table,
		.SymbolTableKey = *symbol_table_key
	};

	try
	{
		if (!is->client.SendPosition(position, tocall, is->digipath))
		{
			aprs_is_disconnect(is);

			return false;
		}
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	return true;
}
bool                                       aprs_is_set_blocking(aprs_is* is, bool value)
{
	try
	{
		is->client.SetBlocking(
			value
		);
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	return true;
}

bool                                       aprs_packet_is_message(aprs_packet* packet)
{
	return packet->IsMessage();
}
bool                                       aprs_packet_is_position(aprs_packet* packet)
{
	return packet->IsPosition();
}

aprs_packet*                               aprs_packet_decode(const char* string)
{
	auto packet = new aprs_packet();

	if (!APRS::Packet::Decode(*packet, string))
	{
		delete packet;

		return nullptr;
	}

	return packet;
}
aprs_packet*                               aprs_packet_init(const char* sender, const char* tocall, const char* digipath, const char* content)
{
	return new aprs_packet
	{
		.ToCall   = tocall,
		.Sender   = sender,
		.Content  = content,
		.DigiPath = digipath
	};
}
void                                       aprs_packet_deinit(aprs_packet* packet)
{
	delete packet;
}
const char*                                aprs_packet_get_igate(aprs_packet* packet)
{
	return packet->IGate.GetCString();
}
void                                       aprs_packet_set_igate(aprs_packet* packet, const char* value)
{
	packet->IGate = value;
}
const char*                                aprs_packet_get_qflag(aprs_packet* packet)
{
	return packet->QFlag.GetCString();
}
void                                       aprs_packet_set_qflag(aprs_packet* packet, const char* value)
{
	packet->QFlag = value;
}
const char*                                aprs_packet_get_tocall(aprs_packet* packet)
{
	return packet->ToCall.GetCString();
}
void                                       aprs_packet_set_tocall(aprs_packet* packet, const char* value)
{
	packet->ToCall = value;
}
const char*                                aprs_packet_get_sender(aprs_packet* packet)
{
	return packet->Sender.GetCString();
}
void                                       aprs_packet_set_sender(aprs_packet* packet, const char* value)
{
	packet->Sender = value;
}
const char*                                aprs_packet_get_content(aprs_packet* packet)
{
	return packet->Content.GetCString();
}
void                                       aprs_packet_set_content(aprs_packet* packet, const char* value)
{
	packet->Content = value;
}
const char*                                aprs_packet_get_digipath(aprs_packet* packet)
{
	return packet->DigiPath.GetCString();
}
void                                       aprs_packet_set_digipath(aprs_packet* packet, const char* value)
{
	packet->DigiPath = value;
}

aprs_message*                              aprs_message_init(const char* content, const char* destination, const char* ack)
{
	return new aprs_message
	{
		.Ack         = ack ? ack : "",
		.Content     = content,
		.Destination = destination
	};
}
aprs_message*                              aprs_message_decode(aprs_packet* packet)
{
	auto message = new aprs_message();

	if (!aprs_message::Decode(*message, *packet))
	{
		delete message;

		return nullptr;
	}

	return message;
}
void                                       aprs_message_deinit(aprs_message* message)
{
	delete message;
}
const char*                                aprs_message_get_ack(aprs_message* message)
{
	return (message->Ack.GetLength() != 0) ? message->Ack.GetCString() : nullptr;
}
void                                       aprs_message_set_ack(aprs_message* message, const char* value)
{
	message->Ack = value;
}
const char*                                aprs_message_get_content(aprs_message* message)
{
	return message->Content.GetCString();
}
void                                       aprs_message_set_content(aprs_message* message, const char* value)
{
	message->Content = value;
}
const char*                                aprs_message_get_destination(aprs_message* message)
{
	return message->Destination.GetCString();
}
void                                       aprs_message_set_destination(aprs_message* message, const char* value)
{
	message->Destination = value;
}

aprs_position*                             aprs_position_init(AL::Float latitude, AL::Float longitude, AL::uint16 altitude, const char* symbol_table, const char* symbol_table_key)
{
	return new aprs_position
	{
		.Altitude       = altitude,
		.Latitude       = latitude,
		.Longitude      = longitude,
		.SymbolTable    = *symbol_table,
		.SymbolTableKey = *symbol_table_key
	};
}
aprs_position*                             aprs_position_decode(aprs_packet* packet)
{
	auto position = new aprs_position();

	if (!aprs_position::Decode(*position, *packet))
	{
		delete position;

		return nullptr;
	}

	return position;
}
void                                       aprs_position_deinit(aprs_position* position)
{
	delete position;
}
AL::uint16                                 aprs_position_get_altitude(aprs_position* position)
{
	return position->Altitude;
}
void                                       aprs_position_set_altitude(aprs_position* position, AL::uint16 value)
{
	position->Altitude = value;
}
AL::Float                                  aprs_position_get_latitude(aprs_position* position)
{
	return position->Latitude;
}
void                                       aprs_position_set_latitude(aprs_position* position, AL::Float value)
{
	position->Latitude = value;
}
AL::Float                                  aprs_position_get_longitude(aprs_position* position)
{
	return position->Longitude;
}
void                                       aprs_position_set_longitude(aprs_position* position, AL::Float value)
{
	position->Longitude = value;
}
const char*                                aprs_position_get_comment(aprs_position* position)
{
	return (position->Comment.GetLength() != 0) ? position->Comment.GetCString() : nullptr;
}
void                                       aprs_position_set_comment(aprs_position* position, const char* value)
{
	position->Comment = value;
}
const char*                                aprs_position_get_symbol_table(aprs_position* position)
{
	static char symbol_table[2] = { 0 };

	symbol_table[0] = position->SymbolTable;

	return symbol_table;
}
void                                       aprs_position_set_symbol_table(aprs_position* position, const char* value)
{
	position->SymbolTable = *value;
}
const char*                                aprs_position_get_symbol_table_key(aprs_position* position)
{
	static char symbol_table_key[2] = { 0 };

	symbol_table_key[0] = position->SymbolTableKey;

	return symbol_table_key;
}
void                                       aprs_position_set_symbol_table_key(aprs_position* position, const char* value)
{
	position->SymbolTableKey = *value;
}

void APRS::IS::API::RegisterGlobals()
{
	APRS_IS_API_RegisterGlobalFunction(aprs_is_init);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_deinit);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_is_blocking);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_is_connected);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_connect);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_disconnect);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_read_packet);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_write_packet);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_send_packet);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_send_message);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_send_message_ack);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_send_position);
	APRS_IS_API_RegisterGlobalFunction(aprs_is_set_blocking);

	APRS_IS_API_RegisterGlobalFunction(aprs_packet_is_message);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_is_position);

	APRS_IS_API_RegisterGlobalFunction(aprs_packet_decode);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_init);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_deinit);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_get_igate);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_set_igate);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_get_qflag);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_set_qflag);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_get_tocall);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_set_tocall);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_get_sender);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_set_sender);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_get_content);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_set_content);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_get_digipath);
	APRS_IS_API_RegisterGlobalFunction(aprs_packet_set_digipath);

	APRS_IS_API_RegisterGlobalFunction(aprs_message_init);
	APRS_IS_API_RegisterGlobalFunction(aprs_message_decode);
	APRS_IS_API_RegisterGlobalFunction(aprs_message_deinit);
	APRS_IS_API_RegisterGlobalFunction(aprs_message_get_ack);
	APRS_IS_API_RegisterGlobalFunction(aprs_message_set_ack);
	APRS_IS_API_RegisterGlobalFunction(aprs_message_get_content);
	APRS_IS_API_RegisterGlobalFunction(aprs_message_set_content);
	APRS_IS_API_RegisterGlobalFunction(aprs_message_get_destination);
	APRS_IS_API_RegisterGlobalFunction(aprs_message_set_destination);

	APRS_IS_API_RegisterGlobalFunction(aprs_position_init);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_decode);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_deinit);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_get_altitude);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_set_altitude);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_get_latitude);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_set_latitude);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_get_longitude);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_set_longitude);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_get_comment);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_set_comment);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_get_symbol_table);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_set_symbol_table);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_get_symbol_table_key);
	APRS_IS_API_RegisterGlobalFunction(aprs_position_set_symbol_table_key);
}
