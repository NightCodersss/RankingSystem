#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP

#include "json/src/json.hpp"
using json = nlohmann::json;
using config_type = json;

class ConfigLoader
{
public:
	ConfigLoader(std::string filename);
	config_type get();
};

#endif
