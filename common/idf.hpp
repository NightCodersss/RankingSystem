#pragma once
#include <string>
#include <map>

class IDFHandler
{
private:
	std::map<std::string, double> idf; // word -> idf
public:
	IDFHandler();
	void Load(std::string file);
	double GetIDF(std::string word);
};
