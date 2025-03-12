#pragma once

#include "log_level.hpp"

#include <filesystem>
#include <string>

namespace logger
{

struct LoggerConfig
{
	Level log_level = Level::DEBUG;
	std::string log_file_path = "log.log";
};

LoggerConfig read_config(const std::filesystem::path& file);
LoggerConfig read_config_from_json(const std::string& json_text);

} // namespace logger
