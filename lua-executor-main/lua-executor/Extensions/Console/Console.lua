require('Extensions.Mutex');

local date_time_format = '%m/%d/%Y %I:%M:%S %p';

Console =
{
	ShowDateTimeByDefault = true,

	SetTitle = function(value)
		return console_set_title(tostring(value));
	end,

	ReadLine = function(module, prefix, include_date_time)
		if include_date_time == nil then
			include_date_time = Console.ShowDateTimeByDefault;
		end

		Mutex.Lock(Mutex.GetDefaultInstance());

		if include_date_time then
			console_write('[' .. os.date(date_time_format) .. '] ');
		end

		if module and (module ~= '') then
			console_write('[' .. module .. '] ');
		end

		if prefix and (prefix ~= nil) then
			console_write(prefix .. ': ');
		end

		local value = console_read_line();

		Mutex.Unlock(Mutex.GetDefaultInstance());

		return value;
	end,

	WriteLine = function(module, value, include_date_time)
		if include_date_time == nil then
			include_date_time = Console.ShowDateTimeByDefault;
		end

		Mutex.Lock(Mutex.GetDefaultInstance());

		if include_date_time then
			local date_time = os.date(date_time_format);

			if not module or (module == '') then
				console_write_line('[' .. date_time .. '] ' .. value);
			else
				console_write_line('[' .. date_time .. '] [' .. module .. '] ' .. value);
			end
		else
			console_write_line(value);
		end

		Mutex.Unlock(Mutex.GetDefaultInstance());
	end,

	EnableQuickEdit = function()
		return console_enable_quick_edit_mode();
	end,

	DisableQuickEdit = function()
		return console_disable_quick_edit_mode();
	end
};
