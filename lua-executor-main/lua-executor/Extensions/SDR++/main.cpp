#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/Console.hpp>

#include <AL/Collections/Array.hpp>
#include <AL/Collections/LinkedList.hpp>

#include <AL/Network/DNS.hpp>
#include <AL/Network/TcpSocket.hpp>
#include <AL/Network/SocketExtensions.hpp>

enum SDRPP_VFO_MODES : AL::uint8
{
	SDRPP_VFO_MODE_AM,
	SDRPP_VFO_MODE_FM,
	SDRPP_VFO_MODE_CW,
	SDRPP_VFO_MODE_WFM,
	SDRPP_VFO_MODE_DSB,
	SDRPP_VFO_MODE_USB,
	SDRPP_VFO_MODE_LSB,
	SDRPP_VFO_MODE_RAW
};

struct sdrpp
{
	AL::Network::TcpSocket* socket;
};

const char*     SDRPP_VFO_MODES_ToString(SDRPP_VFO_MODES value)
{
	switch (value)
	{
		case SDRPP_VFO_MODE_AM:  return "AM";
		case SDRPP_VFO_MODE_FM:  return "FM";
		case SDRPP_VFO_MODE_CW:  return "CW";
		case SDRPP_VFO_MODE_WFM: return "WFM";
		case SDRPP_VFO_MODE_DSB: return "DSB";
		case SDRPP_VFO_MODE_USB: return "USB";
		case SDRPP_VFO_MODE_LSB: return "LSB";
		case SDRPP_VFO_MODE_RAW: return "RAW";
	}

	return "RAW";
}
SDRPP_VFO_MODES SDRPP_VFO_MODES_FromString(const AL::String& value)
{
	if (value.Compare("AM"))       return SDRPP_VFO_MODE_AM;
	else if (value.Compare("FM"))  return SDRPP_VFO_MODE_FM;
	else if (value.Compare("CW"))  return SDRPP_VFO_MODE_CW;
	else if (value.Compare("WFM")) return SDRPP_VFO_MODE_WFM;
	else if (value.Compare("DSB")) return SDRPP_VFO_MODE_DSB;
	else if (value.Compare("USB")) return SDRPP_VFO_MODE_USB;
	else if (value.Compare("LSB")) return SDRPP_VFO_MODE_LSB;
	else if (value.Compare("RAW")) return SDRPP_VFO_MODE_RAW;

	return SDRPP_VFO_MODE_RAW;
}

typedef AL::Collections::Array<AL::String> sdrpp_command_params;
typedef AL::Collections::Array<AL::String> sdrpp_command_result;

bool sdrpp_is_connected(sdrpp* sdrpp);

void sdrpp_disconnect(sdrpp* sdrpp);

sdrpp* sdrpp_init()
{
	auto sdrpp = new ::sdrpp
	{
		.socket = nullptr
	};

	return sdrpp;
}
void   sdrpp_deinit(sdrpp* sdrpp)
{
	if (sdrpp != nullptr)
	{
		if (sdrpp_is_connected(sdrpp))
			sdrpp_disconnect(sdrpp);

		delete sdrpp;
	}
}

bool   sdrpp_is_connected(sdrpp* sdrpp)
{
	return sdrpp && sdrpp->socket;
}

bool   sdrpp_connect(sdrpp* sdrpp, const char* host, AL::uint16 port)
{
	if (sdrpp == nullptr)
		return false;

	if (sdrpp_is_connected(sdrpp))
		sdrpp_disconnect(sdrpp);

	try
	{
		AL::Network::IPEndPoint ep =
		{
			.Port = port
		};

		try
		{
			if (!AL::Network::DNS::Resolve(ep.Host, host))
			{

				throw AL::Exception(
					"Host not found"
				);
			}
		}
		catch (AL::Exception& exception)
		{

			throw AL::Exception(
				AL::Move(exception),
				"Error resolving host"
			);
		}

		sdrpp->socket = new AL::Network::TcpSocket(
			ep.Host.GetFamily()
		);

		try
		{
			sdrpp->socket->Open();
		}
		catch (AL::Exception& exception)
		{
			delete sdrpp->socket;
			sdrpp->socket = nullptr;

			throw AL::Exception(
				AL::Move(exception),
				"Error opening AL::Network::TcpSocket"
			);
		}

		try
		{
			if (!sdrpp->socket->Connect(ep))
			{
				delete sdrpp->socket;
				sdrpp->socket = nullptr;

				return false;
			}
		}
		catch (AL::Exception& exception)
		{
			delete sdrpp->socket;
			sdrpp->socket = nullptr;

			throw AL::Exception(
				AL::Move(exception),
				"Error connecting to %s:%u",
				ep.Host.ToString().GetCString(),
				ep.Port
			);
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
void   sdrpp_disconnect(sdrpp* sdrpp)
{
	if (sdrpp_is_connected(sdrpp))
	{
		sdrpp->socket->Close();

		delete sdrpp->socket;
		sdrpp->socket = nullptr;
	}
}

bool   sdrpp_send_command(sdrpp* sdrpp, const char* name, const sdrpp_command_params& params)
{
	if (!sdrpp_is_connected(sdrpp))
		return false;

	auto command = [name, &params]()
	{
		AL::StringBuilder sb;
		sb << '\\' << name;

		for (auto& param : params)
			sb << ' ' << param;

		sb << '\n';

		return sb.ToString();
	}();

	AL::size_t numberOfBytesSent;

	try
	{
		if (!AL::Network::SocketExtensions::SendAll(*sdrpp->socket, command.GetCString(), command.GetLength(), numberOfBytesSent))
		{

			throw AL::Exception(
				"Connection unexpectedly closed"
			);
		}
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		sdrpp_disconnect(sdrpp);

		return false;
	}

	return true;
}
bool   sdrpp_read_command_result(sdrpp* sdrpp, sdrpp_command_result& value)
{
	if (!sdrpp_is_connected(sdrpp))
		return false;

	auto sdrpp_read_command_result_char = [](::sdrpp* sdrpp, char& value, bool block)
	{
		AL::size_t numberOfBytesReceived;

		try
		{
			if (!block)	sdrpp->socket->SetBlocking(AL::False);

			if (!sdrpp->socket->Receive(&value, sizeof(char), numberOfBytesReceived))
			{
				sdrpp_disconnect(sdrpp);

				return false;
			}

			if (!block)	sdrpp->socket->SetBlocking(AL::True);
		}
		catch (const AL::Exception& exception)
		{
			AL::OS::Console::WriteException(
				exception
			);

			sdrpp_disconnect(sdrpp);

			return false;
		}

		return numberOfBytesReceived != 0;
	};

	char                                    c;
	AL::String                              buffer;
	AL::Collections::LinkedList<AL::String> buffers;

	if (!sdrpp_read_command_result_char(sdrpp, c, true))
		return false;

	buffer.Append(c);

	while (sdrpp_read_command_result_char(sdrpp, c, false))
	{
		if (c == '\n')
		{
			buffers.PushBack(
				buffer
			);

			buffer.Clear();

			continue;
		}

		buffer.Append(
			c
		);
	}

	value.SetCapacity(
		buffers.GetSize()
	);

	AL::size_t i = 0;

	for (auto& buffer : buffers)
		value[i++] = AL::Move(buffer);

	return true;
}
bool   sdrpp_execute_command(sdrpp* sdrpp, const char* name, const sdrpp_command_params& params, sdrpp_command_result& result)
{
	return sdrpp_send_command(sdrpp, name, params) && sdrpp_read_command_result(sdrpp, result);
}

bool   sdrpp_start(sdrpp* sdrpp)
{
	sdrpp_command_params params;
	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "start", params, result))
	{
		sdrpp_disconnect(sdrpp);
		return false;
	}

	return (result.GetSize() == 1) && result[0].Compare("RPRT 0");
}
bool   sdrpp_stop(sdrpp* sdrpp)
{
	sdrpp_command_params params;
	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "stop", params, result))
	{
		sdrpp_disconnect(sdrpp);
		return false;
	}

	return (result.GetSize() == 1) && result[0].Compare("RPRT 0");
}
bool   sdrpp_quit(sdrpp* sdrpp)
{
	sdrpp_command_params params;
	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "quit", params, result))
	{
		sdrpp_disconnect(sdrpp);
		return false;
	}

	return (result.GetSize() == 1) && result[0].Compare("RPRT 0");
}

bool   sdrpp_start_recorder(sdrpp* sdrpp)
{
	sdrpp_command_params params;
	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "recorder_start", params, result))
	{
		sdrpp_disconnect(sdrpp);
		return false;
	}

	return (result.GetSize() == 1) && result[0].Compare("RPRT 0");
}
bool   sdrpp_stop_recorder(sdrpp* sdrpp)
{
	sdrpp_command_params params;
	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "recorder_stop", params, result))
	{
		sdrpp_disconnect(sdrpp);
		return false;
	}

	return (result.GetSize() == 1) && result[0].Compare("RPRT 0");
}

// @return success, vfo_mode, vfo_bandwidth
auto   sdrpp_get_vfo_mode(sdrpp* sdrpp)
{
	AL::Collections::Tuple<bool, SDRPP_VFO_MODES, AL::uint32> value(
		false,
		SDRPP_VFO_MODE_RAW,
		0
	);

	sdrpp_command_params params;
	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "get_mode", params, result))
		sdrpp_disconnect(sdrpp);
	else if (result.GetSize() == 2)
	{
		value.Set<0>(true);
		value.Set<1>(SDRPP_VFO_MODES_FromString(result[0]));
		value.Set<2>(AL::FromString<AL::uint32>(result[1]));
	}

	return value;
}
bool   sdrpp_set_vfo_mode(sdrpp* sdrpp, SDRPP_VFO_MODES mode, AL::uint32 bandwidth)
{
	sdrpp_command_params params({
		AL::ToString(mode),
		AL::ToString(bandwidth)
	});

	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "set_mode", params, result))
	{
		sdrpp_disconnect(sdrpp);
		return false;
	}

	return (result.GetSize() == 1) && result[0].Compare("RPRT 0");
}

// @return success, vfo_frequency
auto   sdrpp_get_vfo_frequency(sdrpp* sdrpp)
{
	AL::Collections::Tuple<bool, AL::uint64> value(
		false,
		0
	);

	sdrpp_command_params params;
	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "get_freq", params, result))
		sdrpp_disconnect(sdrpp);
	else if (result.GetSize() == 1)
	{
		value.Set<0>(true);
		value.Set<1>(AL::FromString<AL::uint64>(result[0]));
	}

	return value;
}
bool   sdrpp_set_vfo_frequency(sdrpp* sdrpp, AL::uint64 value)
{
	sdrpp_command_params params({
		AL::ToString(value)
	});

	sdrpp_command_result result;

	if (!sdrpp_execute_command(sdrpp, "set_freq", params, result))
	{
		sdrpp_disconnect(sdrpp);
		return false;
	}

	return (result.GetSize() == 1) && result[0].Compare("RPRT 0");
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobal(SDRPP_VFO_MODE_AM);
	LUA_APRS_IS_RegisterGlobal(SDRPP_VFO_MODE_FM);
	LUA_APRS_IS_RegisterGlobal(SDRPP_VFO_MODE_CW);
	LUA_APRS_IS_RegisterGlobal(SDRPP_VFO_MODE_WFM);
	LUA_APRS_IS_RegisterGlobal(SDRPP_VFO_MODE_DSB);
	LUA_APRS_IS_RegisterGlobal(SDRPP_VFO_MODE_USB);
	LUA_APRS_IS_RegisterGlobal(SDRPP_VFO_MODE_LSB);
	LUA_APRS_IS_RegisterGlobal(SDRPP_VFO_MODE_RAW);

	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_init);
	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_deinit);

	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_is_connected);

	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_connect);
	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_disconnect);

	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_start);
	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_stop);
	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_quit);

	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_start_recorder);
	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_stop_recorder);

	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_get_vfo_mode);
	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_set_vfo_mode);

	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_get_vfo_frequency);
	LUA_APRS_IS_RegisterGlobalFunction(sdrpp_set_vfo_frequency);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobal(SDRPP_VFO_MODE_AM);
	LUA_APRS_IS_UnregisterGlobal(SDRPP_VFO_MODE_FM);
	LUA_APRS_IS_UnregisterGlobal(SDRPP_VFO_MODE_CW);
	LUA_APRS_IS_UnregisterGlobal(SDRPP_VFO_MODE_WFM);
	LUA_APRS_IS_UnregisterGlobal(SDRPP_VFO_MODE_DSB);
	LUA_APRS_IS_UnregisterGlobal(SDRPP_VFO_MODE_USB);
	LUA_APRS_IS_UnregisterGlobal(SDRPP_VFO_MODE_LSB);
	LUA_APRS_IS_UnregisterGlobal(SDRPP_VFO_MODE_RAW);

	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_init);
	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_deinit);

	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_is_connected);

	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_connect);
	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_disconnect);

	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_start);
	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_stop);
	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_quit);

	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_start_recorder);
	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_stop_recorder);

	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_get_vfo_mode);
	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_set_vfo_mode);

	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_get_vfo_frequency);
	LUA_APRS_IS_UnregisterGlobalFunction(sdrpp_set_vfo_frequency);
});
