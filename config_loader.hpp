class ConfigLoader
{
public:
	ConfigLoader(std::string filename);
	ubjson::Value get();
};
