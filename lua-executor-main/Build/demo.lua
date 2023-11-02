require('APRS-IS');

require('Extensions.Script');

Script.LoadExtension('Extensions.Types');

local config =
{
	Host     = 'noam.aprs2.net',
	Port     = 14580,
	Filter   = 't/mp',
	Callsign = 'N0CALL',
	Passcode = 0,
	DigiPath = 'WIDE1-1'
};

local aprs_is = APRS.IS.Init(config.Callsign, config.Passcode, config.Filter, config.DigiPath);

if not APRS.IS.Connect(aprs_is, config.Host, config.Port) then
	Script.SetExitCode(Script.ExitCodes.APRS.IS.ConnectionFailed);
else
	print(string.format(
		'Connected to %s:%u as %s',
		config.Host,
		config.Port,
		config.Callsign
	));

	APRS.IS.SetBlocking(aprs_is, true);

	while APRS.IS.IsConnected(aprs_is) do
		local aprs_is_would_block, aprs_packet = APRS.IS.ReadPacket(aprs_is);

		if not aprs_is_would_block and not Types.IsNull(aprs_packet) then
			local aprs_packet_igate    = APRS.Packet.GetIGate(aprs_packet);
			local aprs_packet_qflag    = APRS.Packet.GetQFlag(aprs_packet);
			local aprs_packet_tocall   = APRS.Packet.GetToCall(aprs_packet);
			local aprs_packet_sender   = APRS.Packet.GetSender(aprs_packet);
			local aprs_packet_content  = APRS.Packet.GetContent(aprs_packet);
			local aprs_packet_digipath = APRS.Packet.GetDigiPath(aprs_packet);

			print(string.format(
				'Received packet from %s [IGate: %s, QFlag: %s, ToCall: %s, DigiPath: %s] %s',
				aprs_packet_sender,
				aprs_packet_igate,
				aprs_packet_qflag,
				aprs_packet_tocall,
				aprs_packet_digipath,
				aprs_packet_content
			));
		end
	end
end

APRS.IS.Deinit(aprs_is);
