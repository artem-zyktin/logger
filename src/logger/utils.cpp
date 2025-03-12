#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>

namespace fs = std::filesystem;

namespace logger
{

std::string read_file(const fs::path& filepath)
{
	if (!fs::is_regular_file(filepath))
		throw std::runtime_error(std::format("file \"{}\" doesn't exists or is not a regular file.", filepath.string()));

	std::fstream file (filepath);

	if (!file.is_open())
		throw std::runtime_error(std::format("can't open file \"{}\".", filepath.string()));

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

} // namespace logger
