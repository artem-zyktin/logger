#pragma once

#include "log_level.hpp"

#include <filesystem>
#include <string>

namespace logger
{

constexpr std::string_view DEFAULT_LOG_FILE = "log.log";
constexpr std::string_view DEFAULT_LOG_PATTERN = "[{{time}}][[thread-id={{thread-id}}]][{{log-level}}] {{message}}";

struct LoggerConfig
{
	Level log_level                     = DEFAULT_LOG_LEVEL;
	std::filesystem::path log_file_path = DEFAULT_LOG_FILE;
	std::string log_pattern             = std::string(DEFAULT_LOG_PATTERN);
};

LoggerConfig read_config(const std::filesystem::path& file);
LoggerConfig read_config_from_json(const std::string& json_text);
bool validate_config(const LoggerConfig& config);

} // namespace logger
