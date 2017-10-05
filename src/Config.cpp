#include "../include/Config.hpp"
#include <fstream>

Config::Config(std::string path)
	: path_(std::move(path))
{
	read_from_file();
}

Config::~Config()
{
	save_to_file();
}


void Config::save_to_file() const
{
	std::ofstream output(path_);
	output << config_.dump(4);
}

void Config::read_from_file()
{
	std::ifstream input(path_);
	input >> config_;
}

nlohmann::json& Config::operator[](const std::string& key)
{
	return config_[key];
}
