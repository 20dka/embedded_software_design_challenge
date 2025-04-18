#pragma once

#include <string>

enum print_mode_t {
	SUPERVISED,
	AUTOMATIC
};

struct input_config_t {
	bool valid = false;
	const char* print_name;
	const char* destination_path;
	print_mode_t print_mode;
};

enum layer_error_t {
	SUCCESS,
	TEMP_OUT_OF_RANGE,
	TIMED_OUT,
	UNKNOWN_ERROR
};

static inline layer_error_t string_to_layer_error(std::string str)
{
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

static inline std::string layer_error_to_string(layer_error_t error)
{
	switch (error)
	{
	case SUCCESS:
		return "Success";
		break;
	case TEMP_OUT_OF_RANGE:
		return "Temperature out of range";
		break;
	case TIMED_OUT:
		return "Operation timed out";
		break;
	default:
	case UNKNOWN_ERROR:
		return "Unknown error";
		break;
	}
}

struct print_layer_t {
	layer_error_t layer_error = UNKNOWN_ERROR;
	uint32_t layer_number = 0;
	uint32_t layer_height = 0; // stored in um
	uint32_t layer_time = 0; //stored in s
	std::string file_name;
	std::string image_url;
};

struct summary_data_t {
	uint32_t overall_height = 0;
	uint32_t layer_count = 0;
	uint32_t failed_layer_count = 0;
};

input_config_t process_inputs(int argc, const char* argv[]);

bool processLayer(input_config_t config, std::ifstream& file, summary_data_t* summary);
bool process_csv(input_config_t config, summary_data_t* summary);
bool summarize(summary_data_t summary);
