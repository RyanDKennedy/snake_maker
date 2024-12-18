#include "utils.hpp"

#include <iostream>

std::string read_file(const char *path, bool *error)
{
    std::ifstream file(path, std::ios::in);
    std::stringstream stream;

    if (!file.is_open() || file.fail())
    {
	*error = true;
	file.close();
	return "";
    }
    else
    {
	stream << file.rdbuf();
	file.close();

	*error = false;

	return stream.str();

    }
}
