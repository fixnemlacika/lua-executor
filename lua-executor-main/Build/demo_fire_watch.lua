require('Plugins.FireWatch');

if FireWatch.Init('N0CALL', 0, 'WIDE1-1', 'noam.aprs2.net', 14580, 'FireWatch.db') then
	FireWatch.Sources.Add('127.0.0.1', 9000);

	FireWatch.Events.RegisterEvent(FireWatch.Events.OnFireStarted, function(fire_id, fire_name, fire_hotspot_positions)
		Console.WriteLine(nil, string.format('Fire #%u (%s) has started', fire_id, fire_name));
	end);

	FireWatch.Events.RegisterEvent(FireWatch.Events.OnFireUpdated, function(fire_id, fire_name, fire_hotspot_positions)
		Console.WriteLine(nil, string.format('Fire #%u (%s) has been updated', fire_id, fire_name));
	end);

	FireWatch.Events.RegisterEvent(FireWatch.Events.OnFireNameChanged, function(fire_id, fire_name, fire_prev_name)
		Console.WriteLine(nil, string.format('Fire #%u has been renamed from %s to %s', fire_id, fire_prev_name, fire_name));
	end);

	FireWatch.Events.RegisterEvent(FireWatch.Events.OnFireExtinguished, function(fire_id, fire_name)
		Console.WriteLine(nil, string.format('Fire #%u (%s) has been extinguished', fire_id, fire_name));
	end);

	FireWatch.Events.RegisterEvent(FireWatch.Events.OnStationEnteredFireZone, function(fire_id, fire_name, station, station_latitude, station_longitude, station_altitude, station_comment, station_distance_to_nearest_fire_hotspot)
		Console.WriteLine(nil, string.format('Station %s has entered fire #%u zone. [Fire: %s, Distance: %.2f ft]', station, fire_id, fire_name, station_distance_to_nearest_fire_hotspot));
	end);

	FireWatch.Events.RegisterEvent(FireWatch.Events.OnStationLeftFireZone, function(fire_id, fire_name, station, station_latitude, station_longitude, station_altitude, station_comment)
		Console.WriteLine(nil, string.format('Station %s has left fire #%u zone. [Fire: %s]', station, fire_id, fire_name));
	end);

	FireWatch.Events.RegisterEvent(FireWatch.Events.OnStationPositionChangedInFireZone, function(fire_id, fire_name, station, station_latitude, station_longitude, station_altitude, station_comment, station_distance_to_nearest_fire_hotspot)
		Console.WriteLine(nil, string.format('Station %s has moved within fire #%u zone. [Fire: %s, Distance: %.2f ft, Latitude: %.5f, Longitude: %.5f, Altitude: %.2f ft, Comment: %s]', station, fire_id, fire_name, station_distance_to_nearest_fire_hotspot, station_latitude, station_longitude, station_altitude, station_comment));
	end);

	FireWatch.Run();
end
