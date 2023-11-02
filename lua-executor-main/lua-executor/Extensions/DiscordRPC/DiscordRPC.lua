DiscordRPC =
{
	UserFlags =
	{
		None            = DISCORD_RPC_USER_FLAG_NONE,
		Employee        = DISCORD_RPC_USER_FLAG_EMPLOYEE,
		Partner         = DISCORD_RPC_USER_FLAG_PARTNER,
		HypeSquad       = DISCORD_RPC_USER_FLAG_HYPE_SQUAD,
		BugHunter       = DISCORD_RPC_USER_FLAG_BUG_HUNTER,
		HouseBravery    = DISCORD_RPC_USER_FLAG_HOUSE_BRAVERY,
		HouseBrilliance = DISCORD_RPC_USER_FLAG_HOUSE_BRILLIANCE,
		HouseBalance    = DISCORD_RPC_USER_FLAG_HOUSE_BALANCE,
		EarlySupporter  = DISCORD_RPC_USER_FLAG_EARLY_SUPPORTER,
		TeamUser        = DISCORD_RPC_USER_FLAG_TEAM_USER
	},

	UserPremiumTypes =
	{
		None         = DISCORD_RPC_USER_PREMIUM_TYPE_NONE,
		Nitro        = DISCORD_RPC_USER_PREMIUM_TYPE_NITRO,
		NitroClassic = DISCORD_RPC_USER_PREMIUM_TYPE_NITRO_CLASSIC
	},

	-- @param on_ready      function(user_id, user_name, user_flags, user_premium)
	-- @param on_error      function(error_code, error_message)
	-- @param on_connect    function()
	-- @param on_disconnect function(error_code, error_message)
	Init = function(application_id, on_ready, on_error, on_connect, on_disconnect)
		return discord_rpc_init(tostring(application_id), on_ready, on_error, on_connect, on_disconnect);
	end,

	Deinit = function(discord_rpc)
		discord_rpc_deinit(discord_rpc);
	end,

	-- @return false on connection closed
	Poll = function(discord_rpc)
		return discord_rpc_poll(discord_rpc);
	end,

	-- @return false on connection closed
	UpdatePresence = function(discord_rpc, presence)
		return discord_rpc_update_presence(discord_rpc, presence);
	end,

	Presence =
	{
		Init = function()
			return discord_rpc_presence_init();
		end,

		Deinit = function(presence)
			discord_rpc_presence_deinit(presence);
		end,

		GetHeader = function(presence)
			return discord_rpc_presence_get_header(presence);
		end,

		SetHeader = function(presence, value)
			discord_rpc_presence_set_header(presence, value and tostring(value) or nil);
		end,

		GetDetails = function(presence)
			return discord_rpc_presence_get_details(presence);
		end,

		SetDetails = function(presence, value)
			discord_rpc_presence_set_details(presence, value and tostring(value) or nil);
		end,

		GetTimeStart = function(presence)
			return discord_rpc_presence_get_time_start(presence);
		end,

		SetTimeStart = function(presence, value)
			discord_rpc_presence_set_time_start(presence, value and tonumber(value) or 0);
		end,

		GetTimeEnd = function(presence)
			return discord_rpc_presence_get_time_end(presence);
		end,

		SetTimeEnd = function(presence, value)
			discord_rpc_presence_set_time_end(presence, value and tonumber(value) or 0);
		end,

		GetButton = function(presence, index)
			return discord_rpc_presence_get_button(presence, index and tonumber(index) or 0);
		end,

		GetButtonCount = function(presence)
			return discord_rpc_presence_get_button_count(presence);
		end,

		GetImageLargeKey = function(presence)
			return discord_rpc_presence_get_image_large_key(presence);
		end,

		SetImageLargeKey = function(presence, value)
			discord_rpc_presence_set_image_large_key(presence, value and tostring(value) or nil);
		end,

		GetImageLargeText = function(presence)
			return discord_rpc_presence_get_image_large_text(presence);
		end,

		SetImageLargeText = function(presence, value)
			discord_rpc_presence_set_image_large_text(presence, value and tostring(value) or nil);
		end,

		GetImageSmallKey = function(presence)
			return discord_rpc_presence_get_image_small_key(presence);
		end,

		SetImageSmallKey = function(presence, value)
			discord_rpc_presence_set_image_small_key(presence, value and tostring(value) or nil);
		end,

		GetImageSmallText = function(presence)
			return discord_rpc_presence_get_image_small_text(presence);
		end,

		SetImageSmallText = function(presence, value)
			discord_rpc_presence_set_image_small_text(presence, value and tostring(value) or nil);
		end,

		Button =
		{
			GetURL = function(button)
				return discord_rpc_presence_button_get_url(button);
			end,

			SetURL = function(button, value)
				discord_rpc_presence_button_set_url(button, value and tostring(value) or nil);
			end,

			GetLabel = function(button)
				return discord_rpc_presence_button_get_label(button);
			end,

			SetLabel = function(button, value)
				discord_rpc_presence_button_set_label(button, value and tostring(value) or nil);
			end
		},

		Buttons =
		{
			Add = function(presence, label, url)
				return discord_rpc_presence_buttons_add(presence, label and tostring(label) or nil, url and tostring(url) or nil);
			end,

			Remove = function(presence, button)
				discord_rpc_presence_buttons_remove(presence, button);
			end,

			Clear = function(presence)
				discord_rpc_presence_buttons_clear(presence);
			end
		}
	}
};
