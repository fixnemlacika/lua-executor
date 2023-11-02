Script =
{
	LoadExtension = function(api)
		local platform       = Script.GetPlatform();
		local extension_path = string.gsub(api, '%.', '/');

		if platform == Script.Platforms.Linux then
			extension_path = extension_path .. '.so';
		elseif platform == Script.Platforms.Windows then
			extension_path = extension_path .. '.dll';
		end

		local extension = script_load_extension(extension_path);

		if extension ~= nil then
			require(api);
		end

		return extension;
	end,

	UnloadExtension = function(extension)
		script_unload_extension(extension);
	end,

	GetExitCode = function()
		return script_get_exit_code();
	end,

	SetExitCode = function(value)
		script_set_exit_code(tonumber(value));
	end,

	GetPlatform = function()
		return script_get_platform();
	end,

	Lua =
	{
		Eval = function(lua)
			return script_lua_eval(tostring(lua));
		end
	},

	ExitCodes =
	{
		Success     = SCRIPT_EXIT_CODE_SUCCESS,
		UserDefined = 1000,

		APRS =
		{
			IS =
			{
				ConnectionClosed = 1,
				ConnectionFailed = 2
			}
		},

		SQLite3 =
		{
			OpenFailed = 3
		}
	},

	Platforms =
	{
		Linux   = SCRIPT_PLATFORM_LINUX,
		Windows = SCRIPT_PLATFORM_WINDOWS,
	}
};
