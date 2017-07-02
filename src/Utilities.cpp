#include "../include/Utilities.hpp"

float volume_int_to_float(int vol)
{
	return float(vol) / 100.f;
}

unsigned int volume_float_to_int(float vol)
{
	return unsigned int(vol*100.f);
}

bool str_to_bool(const std::string& str)
{
	return str == "true" ? true : false;
}

std::string bool_to_str(bool b)
{
	return b ? "true" : "false";
}