#include "config_loader.hpp"
#include <fstream>
#include <stdexcept>

ConfigLoader::ConfigLoader(std::string filename): filename(filename) { }

config_type ConfigLoader::get()
{
	std::ifstream input(filename.c_str());

	if ( !input )
		throw std::runtime_error("Could not open file " + filename);

	config_type config;
	config << input;
	return config;
}
