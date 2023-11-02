#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/Console.hpp>

#include <AL/Network/DNS.hpp>
#include <AL/Network/TcpSocket.hpp>
#include <AL/Network/UdpSocket.hpp>
#include <AL/Network/SocketExtensions.hpp>

enum _SOCKET_TYPES : AL::int8
{
	_SOCKET_TYPE_TCP,
	_SOCKET_TYPE_UDP
};

#define _SOCKET_TYPE_UNDEFINED -1

struct _socket
{
	bool                    is_blocking;
	bool                    is_connected;
	bool                    is_listening;

	_SOCKET_TYPES           type;
	AL::Network::TcpSocket* socket_tcp;
	AL::Network::UdpSocket* socket_udp;
	AL::Network::IPEndPoint socket_end_point;
};

_SOCKET_TYPES _socket_get_type(_socket* socket);

_socket*                                       _socket_open(_SOCKET_TYPES type)
{
	auto socket = new _socket
	{
		.is_blocking  = true,
		.is_connected = false,
		.is_listening = false,

		.type       = type,
		.socket_tcp = nullptr,
		.socket_udp = nullptr
	};

	return socket;
}
void                                           _socket_close(_socket* socket)
{
	if (socket != nullptr)
	{
		switch (_socket_get_type(socket))
		{
			case _SOCKET_TYPE_TCP:
			{
				if (socket->socket_tcp != nullptr)
				{
					socket->socket_tcp->Close();
					delete socket->socket_tcp;
				}
			}
			break;

			case _SOCKET_TYPE_UDP:
			{
				if (socket->socket_udp != nullptr)
				{
					socket->socket_udp->Close();
					delete socket->socket_udp;
				}
			}
			break;
		}

		delete socket;
	}
}
bool                                           _socket_is_blocking(_socket* socket)
{
	return socket && socket->is_blocking;
}
bool                                           _socket_is_connected(_socket* socket)
{
	return socket && socket->is_connected;
}
bool                                           _socket_is_listening(_socket* socket)
{
	return socket && socket->is_listening;
}
_SOCKET_TYPES                                  _socket_get_type(_socket* socket)
{
	return static_cast<_SOCKET_TYPES>(socket ? socket->type : _SOCKET_TYPE_UNDEFINED);
}
bool                                           _socket_set_blocking(_socket* socket, bool value)
{
	if (socket != nullptr)
	{
		if (_socket_is_connected(socket))
		{
			switch (_socket_get_type(socket))
			{
				case _SOCKET_TYPE_TCP:
				{
					try
					{
						socket->socket_tcp->SetBlocking(
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
				}
				break;

				case _SOCKET_TYPE_UDP:
				{
					try
					{
						socket->socket_udp->SetBlocking(
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
				}
				break;
			}
		}

		socket->is_blocking = value;
	}

	return false;
}
bool                                           _socket_listen(_socket* socket, const char* host, AL::uint16 port)
{
	if (socket != nullptr)
	{
		switch (_socket_get_type(socket))
		{
			case _SOCKET_TYPE_TCP:
			{
				try
				{
					// TODO: implement
					throw AL::NotImplementedException();
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);

					return false;
				}
			}
			break;

			case _SOCKET_TYPE_UDP:
				break;
		}

		socket->is_listening = true;
	}

	return false;
}
// @return socket, would_block
AL::Collections::Tuple<_socket*, bool>         _socket_accept(_socket* socket)
{
	AL::Collections::Tuple<_socket*, bool> result(nullptr, false);

	if (socket != nullptr)
	{
		switch (_socket_get_type(socket))
		{
			case _SOCKET_TYPE_TCP:
			{
				try
				{
					// TODO: implement
					throw AL::NotImplementedException();
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;

			case _SOCKET_TYPE_UDP:
				result.Set<1>(true);
				break;
		}
	}

	return result;
}
bool                                           _socket_connect(_socket* socket, const char* host, AL::uint16 port)
{
	if (!_socket_is_connected(socket))
	{
		socket->socket_end_point.Port = port;

		try
		{
			try
			{
				if (!AL::Network::DNS::Resolve(socket->socket_end_point.Host, host))
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
					"Error resolving '%s'",
					host
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

		switch (_socket_get_type(socket))
		{
			case _SOCKET_TYPE_TCP:
			{
				socket->socket_tcp = new AL::Network::TcpSocket(
					socket->socket_end_point.Host.GetFamily()
				);

				try
				{
					try
					{
						socket->socket_tcp->Open();
					}
					catch (AL::Exception& exception)
					{

						throw AL::Exception(
							AL::Move(exception),
							"Error opening AL::Network::TcpSocket"
						);
					}

					try
					{
						if (!socket->socket_tcp->Connect(socket->socket_end_point))
						{
							delete socket->socket_tcp;
							socket->socket_tcp = nullptr;

							return false;
						}
					}
					catch (AL::Exception& exception)
					{

						throw AL::Exception(
							AL::Move(exception),
							"Error connecting to '%s:%u'",
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}

					try
					{
						socket->socket_tcp->SetBlocking(
							socket->is_blocking
						);
					}
					catch (AL::Exception& exception)
					{
						socket->socket_tcp->Close();

						throw AL::Exception(
							AL::Move(exception),
							"Error setting AL::Network::TcpSocket blocking state"
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					delete socket->socket_tcp;
					socket->socket_tcp = nullptr;

					AL::OS::Console::WriteException(
						exception
					);

					return false;
				}

				socket->is_connected = true;
			}
			return true;

			case _SOCKET_TYPE_UDP:
			{
				socket->socket_udp = new AL::Network::UdpSocket(
					socket->socket_end_point.Host.GetFamily()
				);

				try
				{
					try
					{
						socket->socket_udp->Open();
					}
					catch (AL::Exception& exception)
					{

						throw AL::Exception(
							AL::Move(exception),
							"Error opening AL::Network::UdpSocket"
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					delete socket->socket_udp;
					socket->socket_udp = nullptr;

					AL::OS::Console::WriteException(
						exception
					);

					return false;
				}

				socket->is_connected = true;
			}
			return true;
		}
	}

	return false;
}
// @return connection_closed, number_of_bytes_sent
AL::Collections::Tuple<bool, AL::uint32>       _socket_send(_socket* socket, const void* buffer, AL::uint32 buffer_size)
{
	AL::Collections::Tuple<bool, AL::uint32> result(false, 0);

	if (_socket_is_connected(socket))
	{
		switch (_socket_get_type(socket))
		{
			case _SOCKET_TYPE_TCP:
			{
				try
				{
					try
					{
						AL::size_t number_of_bytes_sent;

						if (!socket->socket_tcp->Send(buffer, buffer_size, number_of_bytes_sent))
						{

							throw AL::Exception(
								"Connection closed"
							);
						}

						result.Set<1>(static_cast<AL::uint32>(number_of_bytes_sent));
					}
					catch (AL::Exception& exception)
					{
						result.Set<0>(true);

						throw AL::Exception(
							AL::Move(exception),
							"Error sending %lu bytes to %s:%u",
							buffer_size,
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;

			case _SOCKET_TYPE_UDP:
			{
				try
				{
					try
					{
						auto number_of_bytes_sent = socket->socket_udp->Send(
							buffer,
							buffer_size,
							socket->socket_end_point
						);

						result.Set<1>(static_cast<AL::uint32>(number_of_bytes_sent));
					}
					catch (AL::Exception& exception)
					{
						result.Set<0>(true);

						throw AL::Exception(
							AL::Move(exception),
							"Error sending %lu bytes to %s:%u",
							buffer_size,
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;
		}
	}

	return result;
}
// @return connection_closed
AL::Collections::Tuple<bool>                   _socket_send_all(_socket* socket, const void* buffer, AL::uint32 buffer_size)
{
	AL::Collections::Tuple<bool> result(false);

	if (_socket_is_connected(socket))
	{
		switch (_socket_get_type(socket))
		{
			case _SOCKET_TYPE_TCP:
			{
				try
				{
					try
					{
						AL::size_t number_of_bytes_sent;

						if (!AL::Network::SocketExtensions::SendAll(*socket->socket_tcp, buffer, buffer_size, number_of_bytes_sent))
						{

							throw AL::Exception(
								"Connection closed"
							);
						}
					}
					catch (AL::Exception& exception)
					{
						result.Set<0>(true);

						throw AL::Exception(
							AL::Move(exception),
							"Error sending %lu bytes to %s:%u",
							buffer_size,
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;

			case _SOCKET_TYPE_UDP:
			{
				try
				{
					try
					{
						for (AL::uint32 number_of_bytes_sent, total_bytes_sent = 0; total_bytes_sent < buffer_size; total_bytes_sent += number_of_bytes_sent)
							number_of_bytes_sent = socket->socket_udp->Send(&reinterpret_cast<const AL::uint8*>(buffer)[total_bytes_sent], buffer_size - total_bytes_sent, socket->socket_end_point);
					}
					catch (AL::Exception& exception)
					{
						result.Set<0>(true);

						throw AL::Exception(
							AL::Move(exception),
							"Error sending %lu bytes to %s:%u",
							buffer_size,
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;
		}
	}

	return result;
}
// @return connection_closed, would_block, number_of_bytes_received
AL::Collections::Tuple<bool, bool, AL::uint32> _socket_receive(_socket* socket, void* buffer, AL::uint32 buffer_capacity)
{
	AL::Collections::Tuple<bool, bool, AL::uint32> result(false, false, 0);

	if (_socket_is_connected(socket))
	{
		switch (_socket_get_type(socket))
		{
			case _SOCKET_TYPE_TCP:
			{
				try
				{
					try
					{
						AL::size_t number_of_bytes_received;

						result.Set<0>(!socket->socket_tcp->Receive(buffer, buffer_capacity, number_of_bytes_received));
						result.Set<1>(number_of_bytes_received == 0);
						result.Set<2>(static_cast<AL::uint32>(number_of_bytes_received));
					}
					catch (AL::Exception& exception)
					{
						result.Set<0>(true);

						throw AL::Exception(
							AL::Move(exception),
							"Error receiving %lu bytes from %s:%u",
							buffer_capacity,
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;

			case _SOCKET_TYPE_UDP:
			{
				try
				{
					try
					{
						result.Set<2>(static_cast<AL::uint32>(socket->socket_udp->Receive(buffer, buffer_capacity, socket->socket_end_point)));
						result.Set<1>(result.Get<2>() == 0);
					}
					catch (AL::Exception& exception)
					{
						result.Set<0>(true);

						throw AL::Exception(
							AL::Move(exception),
							"Error receiving %lu bytes from %s:%u",
							buffer_capacity,
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;
		}
	}

	return result;
}
// @return connection_closed, number_of_bytes_received
AL::Collections::Tuple<bool, AL::uint32>       _socket_receive_all(_socket* socket, void* buffer, AL::uint32 buffer_capacity)
{
	AL::Collections::Tuple<bool, AL::uint32> result(false, 0);

	if (_socket_is_connected(socket))
	{
		switch (_socket_get_type(socket))
		{
			case _SOCKET_TYPE_TCP:
			{
				try
				{
					try
					{
						AL::size_t number_of_bytes_received;

						result.Set<0>(!AL::Network::SocketExtensions::ReceiveAll(*socket->socket_tcp, buffer, buffer_capacity, number_of_bytes_received));
						result.Set<1>(static_cast<AL::uint32>(number_of_bytes_received));
					}
					catch (AL::Exception& exception)
					{
						result.Set<0>(true);

						throw AL::Exception(
							AL::Move(exception),
							"Error receiving %lu bytes from %s:%u",
							buffer_capacity,
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;

			case _SOCKET_TYPE_UDP:
			{
				try
				{
					try
					{
						for (AL::uint32 number_of_bytes_received, total_bytes_received = 0; total_bytes_received < buffer_capacity; total_bytes_received += number_of_bytes_received)
							number_of_bytes_received = socket->socket_udp->Receive(&reinterpret_cast<AL::uint8*>(buffer)[total_bytes_received], buffer_capacity - total_bytes_received, socket->socket_end_point);

						result.Set<1>(buffer_capacity);
					}
					catch (AL::Exception& exception)
					{
						result.Set<0>(true);

						throw AL::Exception(
							AL::Move(exception),
							"Error receiving %lu bytes from %s:%u",
							buffer_capacity,
							socket->socket_end_point.Host.ToString().GetCString(),
							socket->socket_end_point.Port
						);
					}
				}
				catch (const AL::Exception& exception)
				{
					AL::OS::Console::WriteException(
						exception
					);
				}
			}
			break;
		}
	}

	return result;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalEx(_SOCKET_TYPE_TCP, "SOCKET_TYPE_TCP");
	LUA_APRS_IS_RegisterGlobalEx(_SOCKET_TYPE_UDP, "SOCKET_TYPE_UDP");

	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_open,         "socket_open");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_close,        "socket_close");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_is_blocking,  "socket_is_blocking");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_is_connected, "socket_is_connected");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_get_type,     "socket_get_type");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_set_blocking, "socket_set_blocking");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_listen,       "socket_listen");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_accept,       "socket_accept");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_connect,      "socket_connect");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_send,         "socket_send");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_send_all,     "socket_send_all");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_receive,      "socket_receive");
	LUA_APRS_IS_RegisterGlobalFunctionEx(_socket_receive_all,  "socket_receive_all");
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalEx("SOCKET_TYPE_TCP");
	LUA_APRS_IS_UnregisterGlobalEx("SOCKET_TYPE_UDP");

	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_open");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_close");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_is_blocking");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_is_connected");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_get_type");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_set_blocking");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_listen");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_accept");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_connect");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_send");
	LUA_APRS_IS_UnregisterGlobalFunctionEx("socket_receive");
});
