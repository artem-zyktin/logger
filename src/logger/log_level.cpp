#include "log_level.hpp"

#include <unordered_map>
#include <iostream>
#include <algorithm>

namespace
{

std::string to_lower(const std::string_view str)
{
	std::string result (str);
	std::transform(str.begin(), str.end(), result.begin(),
					[](unsigned char c)
					{
						return std::tolower(c);
					});

	return result;
}

} // namespace

namespace logger
{

Level str_to_level(const std::string_view level_str) noexcept
{
	static const std::unordered_map<std::string_view, Level> level_map = {
		{ "debug",   Level::DEBUG },
		{ "info",    Level::INFO },
		{ "warning", Level::WARNING },
		{ "error",   Level::ERROR },
	};

	auto it = level_map.find(to_lower(level_str));

	Level level = DEFAULT_LOG_LEVEL;
	if (it != level_map.end())
		level = it->second;
	else
		std::cerr << "unknown level string" << std::endl;

	return level;
}

std::string_view level_to_str(Level level) noexcept
{
	switch(level)
	{
		case Level::DEBUG:
			return "debug";
		case Level::INFO:
			return "info";
		case Level::WARNING:
			return "warning";
		case Level::ERROR:
			return "error";
		default:
			std::runtime_error("unknown level value");
			break;
	}
}

} // namespace logger
