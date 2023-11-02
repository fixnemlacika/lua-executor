require('Plugins.Outside');

local MyStations =
{
	['N0CALL-5'] = 'Phone',
	['N0CALL-7'] = 'HT'
};

if Outside.Init('N0CALL', 00000, 'WIDE1-1', 'noam.aprs2.net', 14580, 10 * 60, 48 * 3600, 'Outside.db', MyStations) then
	Outside.SetPosition(0, 0, 0);

	Outside.SetDefaultIdleMessage('Lost', 'Location unknown', 1);

	Outside.AddButton('What is this "Outside"?', 'https://github.com/LeoTHPS/LuaAPRS-IS/blob/master/Build/demo_outside.lua');

	Outside.AddIdleMessageByDistance(0,      'Wandering aimlessly', '%.2f ft away from keyboard',    1);
	Outside.AddIdleMessageByDistance(1320,   'Exploring aimlessly', '%.2f miles away from keyboard', 5280);
	Outside.AddIdleMessageByDistance(528000, 'Exploring',           '%.2f miles away from keyboard', 5280);

	Outside.AddIdleMessageByPosition(0, 0, 100, 'At the creek', 'Menacing the public', 1);

	Outside.AddStationPathIcon('NA1SS*',  'aprs_icon_satellite', 'ISS (NA1SS) via %s');
	Outside.AddStationPathIcon('RS0ISS*', 'aprs_icon_satellite', 'ISS (RS0ISS) via %s');

	Outside.Events.RegisterEvent(Outside.Events.OnEnterIdleState, function()
		Console.WriteLine(nil, 'Entered idle state');
	end);

	Outside.Events.RegisterEvent(Outside.Events.OnLeaveIdleState, function()
		Console.WriteLine(nil, 'Exited idle state');
	end);

	Outside.Events.RegisterEvent(Outside.Events.OnPositionChanged, function(name, station, path, igate, latitude, longitude, altitude, comment)
		Console.WriteLine(nil, string.format('%s is now located at %.6f, %.6f', station, latitude, longitude));
	end);

	Outside.Run();
end
