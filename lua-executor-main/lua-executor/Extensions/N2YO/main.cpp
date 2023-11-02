#define LUA_APRS_IS_EXTENSION

#include "json.hpp"
#include "Extension.hpp"

#include <AL/OS/Console.hpp>

#include <AL/Collections/Array.hpp>

#include <AL/Network/HTTP/Request.hpp>

struct n2yo
{
	AL::String api_key;
};

struct n2yo_satellite_pass
{
	AL::Float  start_az;
	AL::String start_az_compass;
	AL::uint32 start_utc;
	AL::Float  max_az;
	AL::String max_az_compass;
	AL::Float  max_elevation;
	AL::uint32 max_utc;
	AL::Float  end_az;
	AL::String end_az_compass;
	AL::uint32 end_utc;
};

typedef AL::Collections::Array<n2yo_satellite_pass> n2yo_satellite_passes;

struct n2yo_query_result
{
	AL::uint32            norad_id;
	AL::uint32            satellite_id;
	AL::String            satellite_name;
	n2yo_satellite_passes satellite_passes;
	AL::uint32            satellite_pass_count;
	AL::uint32            transaction_count;
};

n2yo*                  n2yo_init(const char* api_key)
{
	auto n2yo = new ::n2yo
	{
		.api_key = api_key
	};

	return n2yo;
}
void                   n2yo_deinit(n2yo* n2yo)
{
	if (n2yo != nullptr)
		delete n2yo;
}

AL::Network::HTTP::Uri n2yo_build_query_uri(n2yo* n2yo, AL::uint32 norad_id, AL::Float latitude, AL::Float longitude, AL::Float altitude, AL::uint8 number_of_days, AL::uint8 min_elevation)
{
	auto url = AL::String::Format(
		"https://api.n2yo.com/rest/v1/satellite/radiopasses/%s/%f/%f/%f/%u/%u/&apiKey=%s",
		AL::ToString(norad_id).GetCString(),
		latitude,
		longitude,
		altitude,
		number_of_days,
		min_elevation,
		n2yo->api_key.GetCString()
	);

	return AL::Network::HTTP::Uri::FromString(url);
}
bool                   n2yo_execute_non_query(n2yo* n2yo, AL::Network::HTTP::Response& http_response, AL::uint32 norad_id, AL::Float latitude, AL::Float longitude, AL::Float altitude, AL::uint8 number_of_days, AL::uint8 min_elevation)
{
	AL::Network::HTTP::Request http_request(
		AL::Network::HTTP::Versions::HTTP_1_1,
		AL::Network::HTTP::RequestMethods::GET
	);

	try
	{
		auto http_request_uri = n2yo_build_query_uri(
			n2yo,
			norad_id,
			latitude,
			longitude,
			altitude,
			number_of_days,
			min_elevation
		);

		http_response = http_request.Execute(
			http_request_uri
		);
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return false;
	}

	return http_response.GetStatus() == AL::Network::HTTP::StatusCodes::OK;
}
n2yo_query_result*     n2yo_execute_query(n2yo* n2yo, AL::uint32 norad_id, AL::Float latitude, AL::Float longitude, AL::Float altitude, AL::uint8 number_of_days, AL::uint8 min_elevation)
{
	AL::Network::HTTP::Response http_response;

	if (!n2yo_execute_non_query(n2yo, http_response, norad_id, latitude, longitude, altitude, number_of_days, min_elevation))
	{

		return nullptr;
	}

	auto http_response_content = http_response.GetContent();

	if (!http_response_content.StartsWith('{'))
	{
		http_response_content = http_response_content.SubString(
			http_response_content.Find('{')
		);
	}

	if (!http_response_content.EndsWith('}'))
	{
		http_response_content = http_response_content.SubString(
			http_response_content.begin(),
			http_response_content.FindLast('}')
		);
	}

	nlohmann::json json;

	try
	{
		json = nlohmann::json::parse(
			http_response_content.GetCString()
		);
	}
	catch (const nlohmann::json::exception& exception)
	{
		AL::OS::Console::WriteLine(
			exception.what()
		);

		return nullptr;
	}

	auto query_result = new n2yo_query_result
	{
		.norad_id             = norad_id,
		.satellite_id         = 0,
		.satellite_name       = "",
		.satellite_passes     = n2yo_satellite_passes(0),
		.satellite_pass_count = 0,
		.transaction_count    = 0
	};

	try
	{
		auto& info = json["info"];

		query_result->satellite_id      = info["satid"].get<AL::uint32>();
		query_result->satellite_name    = info["satname"].get<std::string>().c_str();
		query_result->satellite_passes  = n2yo_satellite_passes(info["passescount"].get<AL::uint32>());
		query_result->transaction_count = info["transactionscount"].get<AL::uint32>();

		for (auto& pass : json["passes"])
		{
			query_result->satellite_passes[query_result->satellite_pass_count++] =
			{
				.start_az         = pass["startAz"].get<AL::Float>(),
				.start_az_compass = pass["startAzCompass"].get<std::string>().c_str(),
				.start_utc        = pass["startUTC"].get<AL::uint32>(),
				.max_az           = pass["maxAz"].get<AL::Float>(),
				.max_az_compass   = pass["maxAzCompass"].get<std::string>().c_str(),
				.max_elevation    = pass["maxEl"].get<AL::Float>(),
				.max_utc          = pass["maxUTC"].get<AL::uint32>(),
				.end_az           = pass["endAz"].get<AL::Float>(),
				.end_az_compass   = pass["endAzCompass"].get<std::string>().c_str(),
				.end_utc          = pass["endUTC"].get<AL::uint32>()
			};
		}
	}
	catch (const nlohmann::json::exception& exception)
	{
		delete query_result;

		AL::OS::Console::WriteLine(
			exception.what()
		);

		return nullptr;
	}

	return query_result;
}

void                   n2yo_query_result_deinit(n2yo_query_result* query_result)
{
	if (query_result != nullptr)
		delete query_result;
}
AL::uint32             n2yo_query_result_get_norad_id(n2yo_query_result* query_result)
{
	return query_result ? query_result->norad_id : 0;
}
AL::uint32             n2yo_query_result_get_satellite_id(n2yo_query_result* query_result)
{
	return query_result ? query_result->satellite_id : 0;
}
const char*            n2yo_query_result_get_satellite_name(n2yo_query_result* query_result)
{
	return query_result ? query_result->satellite_name.GetCString() : nullptr;
}
n2yo_satellite_pass*   n2yo_query_result_get_satellite_pass(n2yo_query_result* query_result, AL::uint32 index)
{
	return (query_result && (index > 0) && (index <= query_result->satellite_passes.GetSize())) ? &query_result->satellite_passes[index - 1] : nullptr;
}
AL::uint32             n2yo_query_result_get_satellite_pass_count(n2yo_query_result* query_result)
{
	return query_result ? query_result->satellite_pass_count : 0;
}
AL::uint32             n2yo_query_result_get_transaction_count(n2yo_query_result* query_result)
{
	return query_result ? query_result->transaction_count : 0;
}

AL::uint32             n2yo_satellite_pass_get_time_start(n2yo_satellite_pass* satellite_pass)
{
	return satellite_pass ? satellite_pass->start_utc : 0;
}
AL::uint32             n2yo_satellite_pass_get_time_stop(n2yo_satellite_pass* satellite_pass)
{
	return satellite_pass ? satellite_pass->end_utc : 0;
}
AL::Float              n2yo_satellite_pass_get_max_elevation(n2yo_satellite_pass* satellite_pass)
{
	return satellite_pass ? satellite_pass->max_elevation : 0;
}

LUA_APRS_IS_EXTENSION_INIT([](Extension& extension)
{
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_init);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_deinit);

	LUA_APRS_IS_RegisterGlobalFunction(n2yo_execute_query);

	LUA_APRS_IS_RegisterGlobalFunction(n2yo_query_result_deinit);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_query_result_get_norad_id);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_query_result_get_satellite_id);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_query_result_get_satellite_name);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_query_result_get_satellite_pass);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_query_result_get_satellite_pass_count);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_query_result_get_transaction_count);

	LUA_APRS_IS_RegisterGlobalFunction(n2yo_satellite_pass_get_time_start);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_satellite_pass_get_time_stop);
	LUA_APRS_IS_RegisterGlobalFunction(n2yo_satellite_pass_get_max_elevation);
});

LUA_APRS_IS_EXTENSION_DEINIT([](Extension& extension)
{
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_init);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_deinit);

	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_execute_query);

	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_query_result_deinit);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_query_result_get_norad_id);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_query_result_get_satellite_id);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_query_result_get_satellite_name);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_query_result_get_satellite_pass);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_query_result_get_satellite_pass_count);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_query_result_get_transaction_count);

	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_satellite_pass_get_time_start);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_satellite_pass_get_time_stop);
	LUA_APRS_IS_UnregisterGlobalFunction(n2yo_satellite_pass_get_max_elevation);
});
