#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP

#include "json/src/json.hpp"
#include <string>

using json = nlohmann::json;
using config_type = json;

class ConfigLoader
{
private:
	std::string filename;
public:
	ConfigLoader(std::string filename);
	config_type get();
};

#endif
