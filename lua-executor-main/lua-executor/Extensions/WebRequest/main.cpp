#define LUA_APRS_IS_EXTENSION

#include "Extension.hpp"

#include <AL/OS/Console.hpp>

#include <AL/Network/HTTP/Request.hpp>

#include <AL/Collections/LinkedList.hpp>

typedef typename AL::Get_Enum_Or_Integer_Base<AL::Network::HTTP::RequestMethods>::Type T_WEB_REQUEST_HTTP_METHODS;
typedef typename AL::Get_Enum_Or_Integer_Base<AL::Network::HTTP::StatusCodes>::Type    T_WEB_REQUEST_HTTP_STATUS_CODES;

enum WEB_REQUEST_HTTP_METHODS : T_WEB_REQUEST_HTTP_METHODS
{
	WEB_REQUEST_HTTP_METHOD_GET  = static_cast<T_WEB_REQUEST_HTTP_METHODS>(AL::Network::HTTP::RequestMethods::GET),
	WEB_REQUEST_HTTP_METHOD_POST = static_cast<T_WEB_REQUEST_HTTP_METHODS>(AL::Network::HTTP::RequestMethods::POST)
};

enum WEB_REQUEST_HTTP_STATUS_CODES : T_WEB_REQUEST_HTTP_STATUS_CODES
{
	WEB_REQUEST_HTTP_STATUS_CODE_OK = static_cast<T_WEB_REQUEST_HTTP_STATUS_CODES>(AL::Network::HTTP::StatusCodes::OK)
};

struct web_response
{
	AL::Network::HTTP::Response response;
};

struct web_request
{
	WEB_REQUEST_HTTP_METHODS   method;
	AL::Network::HTTP::Request request;
	web_response               response;
};

web_request*  web_request_init(WEB_REQUEST_HTTP_METHODS method)
{
	auto web_request = new ::web_request
	{
		.method  = method,
		.request = AL::Network::HTTP::Request(AL::Network::HTTP::Versions::HTTP_1_1, static_cast<AL::Network::HTTP::RequestMethods>(method))
	};

	return web_request;
}
void          web_request_deinit(web_request* web_request)
{
	if (web_request != nullptr)
		delete web_request;
}

const char*   web_request_get_header(web_request* web_request, const char* key)
{
	if (web_request == nullptr)
		return nullptr;

	for (auto& header : web_request->request.GetHeader())
	{
		if (header.Key.Compare(key, AL::True))
			return header.Value.GetCString();
	}

	return nullptr;
}
void          web_request_set_header(web_request* web_request, const char* key, const char* value)
{
	if (web_request == nullptr)
		return;

	auto key_length = AL::String::GetLength(key);

	for (auto it = web_request->request.GetHeader().begin(); it != web_request->request.GetHeader().end(); ++it)
	{
		if (it->Key.Compare(key, key_length, AL::True))
		{
			web_request->request.GetHeader().Erase(it);

			break;
		}
	}

	web_request->request.GetHeader().Add(
		AL::String(key, key_length),
		AL::String(value)
	);
}

web_response* web_request_execute(web_request* web_request, const char* uri)
{
	try
	{
		web_request->response.response = web_request->request.Execute(
			AL::Network::HTTP::Uri::FromString(uri)
		);
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return nullptr;
	}

	return &web_request->response;
}

const char*   web_response_get_content(web_response* web_response)
{
	return web_response ? web_response->response.GetContent().GetCString() : nullptr;
}
AL::uint16    web_response_get_status_code(web_response* web_response)
{
	return web_response ? static_cast<AL::uint16>(web_response->response.GetStatus()) : 0;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobal(WEB_REQUEST_HTTP_METHOD_GET);
	LUA_APRS_IS_RegisterGlobal(WEB_REQUEST_HTTP_METHOD_POST);

	LUA_APRS_IS_RegisterGlobal(WEB_REQUEST_HTTP_STATUS_CODE_OK);

	LUA_APRS_IS_RegisterGlobalFunction(web_request_init);
	LUA_APRS_IS_RegisterGlobalFunction(web_request_deinit);

	LUA_APRS_IS_RegisterGlobalFunction(web_request_get_header);
	LUA_APRS_IS_RegisterGlobalFunction(web_request_set_header);

	LUA_APRS_IS_RegisterGlobalFunction(web_request_execute);

	LUA_APRS_IS_RegisterGlobalFunction(web_response_get_content);
	LUA_APRS_IS_RegisterGlobalFunction(web_response_get_status_code);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobal(WEB_REQUEST_HTTP_METHOD_GET);
	LUA_APRS_IS_UnregisterGlobal(WEB_REQUEST_HTTP_METHOD_POST);

	LUA_APRS_IS_UnregisterGlobal(WEB_REQUEST_HTTP_STATUS_CODE_OK);

	LUA_APRS_IS_UnregisterGlobalFunction(web_request_init);
	LUA_APRS_IS_UnregisterGlobalFunction(web_request_deinit);

	LUA_APRS_IS_UnregisterGlobalFunction(web_request_get_header);
	LUA_APRS_IS_UnregisterGlobalFunction(web_request_set_header);

	LUA_APRS_IS_UnregisterGlobalFunction(web_request_execute);

	LUA_APRS_IS_UnregisterGlobalFunction(web_response_get_content);
	LUA_APRS_IS_UnregisterGlobalFunction(web_response_get_status_code);
});
