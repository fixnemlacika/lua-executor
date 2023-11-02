#include "API.hpp"

#include "Extensions/Script.hpp"

#define APRS_IS_API_DEFINE_Extension(__extension_name__, ...) void RegisterExtension_##__extension_name__(AL::Lua54::State& lua) __VA_ARGS__
#define APRS_IS_API_RegisterExtension(__extension_name__)          RegisterExtension_##__extension_name__(lua)

APRS_IS_API_DEFINE_Extension(Script,
{
	APRS_IS_API_RegisterGlobal(SCRIPT_EXIT_CODE_SUCCESS);

	APRS_IS_API_RegisterGlobal(SCRIPT_PLATFORM_LINUX);
	APRS_IS_API_RegisterGlobal(SCRIPT_PLATFORM_WINDOWS);

	APRS_IS_API_RegisterGlobalFunction(script_get_exit_code);
	APRS_IS_API_RegisterGlobalFunction(script_set_exit_code);

	APRS_IS_API_RegisterGlobalFunction(script_get_platform);

	APRS_IS_API_RegisterGlobalFunction(script_lua_eval);

	APRS_IS_API_RegisterGlobalFunction(script_load_extension);
	APRS_IS_API_RegisterGlobalFunction(script_unload_extension);
})

void APRS::IS::API::RegisterExtensions()
{
	APRS_IS_API_RegisterExtension(Script);
}
