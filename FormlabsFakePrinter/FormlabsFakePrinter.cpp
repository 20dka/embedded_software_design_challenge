#include <iostream>
#include "FormlabsFakePrinter.h"

int main(int argc, const char* argv[])
{
	std::cout << "Welcome to Formlabs' newest product, the ~FakePrinter~!\n";

	input_config_t config = process_inputs(argc, argv);

	if (!config.valid)
	{
		std::cout << "The entered input is not valid!\n";
		return 1;
	}
}

input_config_t process_inputs(int argc, const char* argv[])
{
	input_config_t config =
	{
		false // not valid by default
	};

	if (argc != 4)
	{
		return config;
	}

	config.print_name = argv[1];
	config.destination_path = argv[2];

	if (*argv[3] == 'A')
	{
		config.print_mode = AUTOMATIC;
	}
	else if (*argv[3] == 'S')
	{
		config.print_mode = SUPERVISED;
	}
	else {
		return config;
	}

	config.valid = true;

	return config;
}
