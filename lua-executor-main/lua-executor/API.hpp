#pragma once
#include <AL/Common.hpp>

#include <AL/Lua54/Lua.hpp>

#include <AL/OS/Process.hpp>

#include "Extension.hpp"

#include "Extensions/Script.hpp"

#define APRS_IS_API_GetGlobal(__variable__)                                   APRS_IS_API_GetGlobalEx(__variable__, #__variable__)
#define APRS_IS_API_GetGlobalEx(__variable__, __variable_name__)              __variable__ = APRS::IS::API::GetGlobal(__variable_name__)

#define APRS_IS_API_SetGlobal(__variable__)                                   APRS_IS_API_SetGlobalEx(__variable__, #__variable__)
#define APRS_IS_API_SetGlobalEx(__variable__, __variable_name__)              APRS::IS::API::SetGlobal(__variable_name__, __variable__)

#define APRS_IS_API_RegisterGlobal(__variable__)                              APRS_IS_API_RegisterGlobalEx(__variable__, #__variable__)
#define APRS_IS_API_RegisterGlobalEx(__variable__, __variable_name__)         lua.SetGlobal(__variable_name__, __variable__)

#define APRS_IS_API_UnregisterGlobal(__variable__)                            APRS_IS_API_UnregisterGlobalEx(#__variable__)
#define APRS_IS_API_UnregisterGlobalEx(__variable_name__)                     lua.RemoveGlobal(__variable_name__)

#define APRS_IS_API_RegisterGlobalFunction(__function__)                      APRS_IS_API_RegisterGlobalFunctionEx(__function__, #__function__)
#define APRS_IS_API_RegisterGlobalFunctionEx(__function__, __function_name__) lua.SetGlobalFunction<__function__>(__function_name__)

#define APRS_IS_API_UnregisterGlobalFunction(__function__)                    APRS_IS_API_UnregisterGlobalFunctionEx(#__function__)
#define APRS_IS_API_UnregisterGlobalFunctionEx(__function_name__)             lua.RemoveGlobal(__function_name__)

namespace APRS::IS
{
	struct Extension
	{
		AL::OS::Process               process;
		AL::OS::ProcessLibrary        process_library;
		::Extension*                  extension;
		lua_aprs_is_extension_init*   extension_init;
		lua_aprs_is_extension_deinit* extension_deinit;
	};

	class API
	{
		inline static AL::Lua54::State lua;

		API() = delete;

	public:
		static bool IsInitialized()
		{
			return lua.IsCreated();
		}

		// @throw AL::Exception
		static void Init()
		{
			try
			{
				lua.Create();
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error creating AL::Lua54::State"
				);
			}

			try
			{
				if (!lua.LoadLibrary(AL::Lua54::Libraries::All))
				{

					throw AL::Exception(
						"AL::Lua::Libraries::All not found"
					);
				}
			}
			catch (AL::Exception& exception)
			{
				lua.Destroy();

				throw AL::Exception(
					AL::Move(exception),
					"Error loading AL::Lua54::Libraries::All"
				);
			}

			RegisterGlobals();
			RegisterExtensions();
		}

		template<typename T>
		static T GetGlobal(const AL::String& name)
		{
			AL_ASSERT(
				IsInitialized(),
				"API not initialized"
			);

			return lua.GetGlobal<T>(name);
		}

		template<typename T>
		static void SetGlobal(const AL::String& name, T value)
		{
			AL_ASSERT(
				IsInitialized(),
				"API not initialized"
			);

			lua.SetGlobal<T>(name, AL::Forward<T>(value));
		}

		// @throw AL::Exception
		static void Run(const AL::String& lua)
		{
			AL_ASSERT(
				IsInitialized(),
				"API not initialized"
			);

			try
			{
				API::lua.Run(lua);
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error running lua"
				);
			}
		}

		// @throw AL::Exception
		// @return false if file does not exist
		static bool LoadScript(const AL::String& file, AL::int16& exitCode)
		{
			AL_ASSERT(
				IsInitialized(),
				"API not initialized"
			);

			AL::FileSystem::Path path(
				file
			);

			try
			{
				if (!path.Exists())
				{

					return false;
				}
			}
			catch (AL::Exception& exception)
			{

				throw AL::Exception(
					AL::Move(exception),
					"Error checking if file exists"
				);
			}

			script_init();

			try
			{
				lua.RunFile(
					path
				);
			}
			catch (AL::Exception& exception)
			{
				script_deinit();

				throw AL::Exception(
					AL::Move(exception),
					"Error running '%s'",
					file.GetCString()
				);
			}

			exitCode = script_get_exit_code();

			script_deinit();

			return true;
		}

		// @throw AL::Exception
		// @return nullptr if file does not exist
		static Extension* LoadExtension(const AL::String& file)
		{
			AL_ASSERT(
				IsInitialized(),
				"API not initialized"
			);

			auto extension = new Extension
			{
			};

			try
			{
				try
				{
					AL::OS::Process::OpenCurrent(
						extension->process
					);
				}
				catch (AL::Exception& exception)
				{

					throw AL::Exception(
						AL::Move(exception),
						"Error opening AL::OS::Process"
					);
				}

				try
				{
					if (!AL::OS::ProcessLibrary::Load(extension->process_library, extension->process, file))
					{
						extension->process.Close();

						delete extension;

						return nullptr;
					}
				}
				catch (AL::Exception& exception)
				{
					extension->process.Close();

					throw AL::Exception(
						AL::Move(exception),
						"Error loading AL::OS::ProcessLibrary"
					);
				}

				try
				{
					if (!extension->process_library.GetExport(extension->extension_init, "lua_aprs_is_extension_init"))
					{

						throw AL::Exception(
							"Error resolving 'lua_aprs_is_extension_init'"
						);
					}

					if (!extension->process_library.GetExport(extension->extension_deinit, "lua_aprs_is_extension_deinit"))
					{

						throw AL::Exception(
							"Error resolving 'lua_aprs_is_extension_deinit'"
						);
					}
				}
				catch (AL::Exception& exception)
				{
					extension->process_library.Unload();
					extension->process.Close();

					throw AL::Exception(
						AL::Move(exception),
						"Error resolving AL::OS::ProcessLibrary imports"
					);
				}
			}
			catch (AL::Exception& exception)
			{
				delete extension;

				throw AL::Exception(
					AL::Move(exception),
					"Error loading '%s'",
					file.GetCString()
				);
			}

			if ((extension->extension = extension->extension_init(lua)) == nullptr)
			{
				extension->process_library.Unload();
				extension->process.Close();

				delete extension;

				throw AL::Exception(
					"Error initializing extension"
				);
			}

			return extension;
		}

		static void UnloadExtension(Extension* extension)
		{
			if (IsInitialized())
			{
				extension->extension_deinit(extension->extension);

				extension->process_library.Unload();
				extension->process.Close();

				delete extension;
			}
		}

	private:
		static void RegisterGlobals();

		static void RegisterExtensions();
	};
}
