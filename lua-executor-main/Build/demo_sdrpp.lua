require('Extensions.Script');

Script.LoadExtension('Extensions.SDR++');
Script.LoadExtension('Extensions.Thread');

local sdrpp = SDRPP.Init();

if SDRPP.Connect(sdrpp, '127.0.0.1', 4500) then
	local delta = 1;

	for i = 1, 100 do
		for j = 1, 10 do
			local success_frequency, vfo_frequency = SDRPP.VFO.GetFrequency(sdrpp);

			if success_frequency then
				SDRPP.VFO.SetFrequency(sdrpp, vfo_frequency + (1000 * delta));
			end

			Thread.Sleep(10);
		end

		delta = delta * -1;
	end

	SDRPP.Disconnect(sdrpp);
end

SDRPP.Deinit(sdrpp);
