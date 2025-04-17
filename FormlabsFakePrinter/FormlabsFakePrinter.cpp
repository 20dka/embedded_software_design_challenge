#include <iostream>
#include <string>
#include "FormlabsFakePrinter.h"

#include <fstream>
#include <sstream>

int main(int argc, const char* argv[])
{
	std::cout << "Welcome to Formlabs' newest product, the ~FakePrinter~!\n";

	input_config_t config = process_inputs(argc, argv);

	if (!config.valid)
	{
		std::cout << "The entered input is not valid!\n";
		return 1;
	}

	if (process_csv(config, "sliced.csv"))
	{
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

layer_error_t string_to_layer_error(std::string str) {
	if (str == "SUCCESS")
	{
		return SUCCESS;
	}
	else if (str == "TEMP_OUT_OF_RANGE")
	{
		return TEMP_OUT_OF_RANGE;
	}
	else if (str == "TIMED_OUT")
	{
		return TIMED_OUT;
	}
	else
	{
		return UNKNOWN_ERROR;
	}
}

uint32_t layer_time_to_sec(std::string str) {
	return 0; //TODO: add implementation
}

bool processLayer(input_config_t config, std::ifstream& file)
{
	std::string line;

	print_layer_t layer_info;

	if (!std::getline(file, line)) {
		return true;
	}

	std::stringstream ss(line);
	std::string cell;
	int i = 0;
	while (std::getline(ss, cell, ',')) {
		switch (i)
		{
		case 0:
			layer_info.layer_error = string_to_layer_error(cell);
			break;
		case 1:
			layer_info.layer_number = std::strtol(cell.c_str(), NULL, 10);
			break;
		case 2:
			layer_info.layer_height = std::strtof(cell.c_str(), NULL) * 1000;
			break;
		case 15:
			layer_info.layer_time = layer_time_to_sec(cell);
			break;
		case 16:
			layer_info.file_name = cell;
			break;
		case 17:
			layer_info.image_url = cell;
			break;
		}

		i++;
	}

	if (i != 18)
	{
		std::cerr << "Failed to parse csv line" << std::endl;
		return true;
	}

	//TODO: process layer data

	return false;
}

bool process_csv(input_config_t config, const std::string& filename)
{
	std::ifstream file(filename);

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return true;
	}

	std::string dummy;
	if (!std::getline(file, dummy)) {
		return true;
	}

	while (!processLayer(config, file));

	file.close();
	return false;
}
