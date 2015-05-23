#include "config_loader.hpp"
#include <fstream>

ConfigLoader::ConfigLoader(std::string filename): filename(filename) { }

config_type ConfigLoader::get()
{
	std::ifstream input(filename.c_str());
	config_type config;
	config << input;
	return config;
}
