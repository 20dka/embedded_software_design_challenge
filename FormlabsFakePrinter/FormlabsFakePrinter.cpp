#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "FormlabsFakePrinter.h"
#include "DownloadWrapper.h"

DownloadWrapper download_wrapper;

int main(int argc, const char* argv[])
{
	std::cout << "Welcome to Formlabs' newest product, the ~FakePrinter~!\n";

	input_config_t config = process_inputs(argc, argv);

	if (!config.valid)
	{
		std::cerr << "The entered input is not valid!\n";
		return 1;
	}

	std::error_code ec;
	if (!std::filesystem::create_directory(config.destination_path, ec))
	{
		std::cerr << "Failed creating output directory: " << ec.message() << std::endl;
	}


	summary_data_t summary;
	
	if (process_csv(config, &summary))
	{
		return 2;
	}

	summarize(summary);

	return 0;
}

input_config_t process_inputs(int argc, const char* argv[])
{
	input_config_t config =
	{
		false // not valid by default
	};

	//TODO: if argc is 1, prompt user for parameters
	if (argc != 4)
	{
		std::cerr << "Malformed parameters! <print name> <destination path> <mode>" << std::endl;
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

uint32_t layer_time_to_sec(std::string str) {
	uint32_t seconds = 0;

	std::istringstream stream(str);
	std::string segment;

	//get first segment, assume seconds
	if (getline(stream, segment, '_')) {
		seconds = std::strtoul(segment.substr(0, segment.size() - 2).c_str(), NULL, 10);
	}

	//get any later segments, offset previous ones
	while (getline(stream, segment, '_')) {
		seconds *= 60;
		seconds += std::strtoul(segment.substr(0, segment.size() - 2).c_str(), NULL, 10);
	}

	return seconds;
}

bool processLayer(input_config_t config, std::ifstream& file, summary_data_t* summary)
{
	std::string line;

	print_layer_t layer_info;

	if (!std::getline(file, line))
	{
		return true;
	}

	std::stringstream ss(line);
	std::string cell;
	int i = 0;
	while (std::getline(ss, cell, ','))
	{
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

	if (config.print_mode == SUPERVISED)
	{
		std::cout << "Waiting for user input" << std::endl;
		std::string dummy;
		std::getline(std::cin, dummy);
	}

	std::cout << "Processing layer " << layer_info.layer_number << std::endl;

	summary->overall_height += layer_info.layer_height;
	summary->layer_count += 1;

	bool download_error = download_wrapper.download(layer_info.image_url, (std::filesystem::path(config.destination_path) / std::filesystem::path(layer_info.file_name)).string());

	if (layer_info.layer_error != SUCCESS || download_error)
	{
		summary->failed_layer_count += 1;

		if (config.print_mode == SUPERVISED)
		{
			std::cout << "Layer error occurred (" << layer_error_to_string(layer_info.layer_error) << ")!End FakePrint ? (Y / N)" << std::endl;

			std::string prompt;
			std::getline(std::cin, prompt);
			if (prompt == "Y")
			{
				return true;
			}
		}
	}

	std::cout << "Printing for " << layer_info.layer_time << " seconds" << std::endl;

	return false;
}

bool process_csv(input_config_t config, summary_data_t* summary)
{
	std::ifstream file(config.print_name);

	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << config.print_name << std::endl;
		return true;
	}

	//TODO: parse first line, store cell indexes

	std::string dummy;
	if (!std::getline(file, dummy)) {
		return true;
	}

	while (!processLayer(config, file, summary));

	file.close();
	return false;
}

bool summarize(summary_data_t summary)
{
	printf("printed %d layers, %d failed. overall height: %.3fmm\r\n", summary.layer_count, summary.failed_layer_count, static_cast<float>(summary.overall_height) / 1000.0);

	return false;
}
