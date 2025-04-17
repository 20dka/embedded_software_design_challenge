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

input_config_t process_inputs(int argc, const char* argv[]);

config_t process_inputs(int argc, const char* argv[]);