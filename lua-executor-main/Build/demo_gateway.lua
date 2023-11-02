require('Plugins.Gateway');

if Gateway.Init('N0CALL', 0, 'WIDE1-1', 'noam.aprs2.net', 14580, 't/mp', 'Gateway.db') then
	Gateway.Commands.SetHandler('test', function(sender, prefix, params)
		local params_as_string = '';

		for i, param in ipairs(params) do
			if i > 1 then
				params_as_string = params_as_string .. ', ';
			end

			params_as_string = params_as_string .. param;
		end

		Gateway.APRS.IS.SendMessage(sender, string.format('Hello %s!', sender));
		Gateway.APRS.IS.SendMessage(sender, string.format('Your params were: %s', params_as_string));
	end);

	Gateway.Run();
end
