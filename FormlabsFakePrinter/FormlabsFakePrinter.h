#pragma once

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
};

input_config_t process_inputs(int argc, const char* argv[]);

bool processLayer(input_config_t config, std::ifstream& file, summary_data_t* summary);
bool process_csv(input_config_t config, summary_data_t* summary);