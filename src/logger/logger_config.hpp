#pragma once

#include "log_level.hpp"

#include <filesystem>
#include <string>

namespace logger
{

constexpr std::string_view DEFAULT_LOG_FILE = "log.log";

struct LoggerConfig
{
	Level log_level = DEFAULT_LOG_LEVEL;
	std::filesystem::path log_file_path = DEFAULT_LOG_FILE;
};

LoggerConfig read_config(const std::filesystem::path& file);
LoggerConfig read_config_from_json(const std::string& json_text);

} // namespace logger
