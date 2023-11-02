#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include "API.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		AL::OS::Console::WriteLine(
			"Invalid args"
		);

		AL::OS::Console::WriteLine(
			"%s script.lua",
			argv[0]
		);

		return -1;
	}

	AL::int16 exitCode;

	try
	{
		try
		{
			APRS::IS::API::Init();
		}
		catch (AL::Exception& exception)
		{

			throw AL::Exception(
				AL::Move(exception),
				"Error initializing API"
			);
		}

		try
		{
			if (!APRS::IS::API::LoadScript(argv[1], exitCode))
			{

				throw AL::Exception(
					"File not found"
				);
			}
		}
		catch (AL::Exception& exception)
		{

			throw AL::Exception(
				AL::Move(exception),
				"Error loading script [Path: %s]",
				argv[1]
			);
		}
	}
	catch (const AL::Exception& exception)
	{
		AL::OS::Console::WriteException(
			exception
		);

		return -1;
	}

	return exitCode;
}
