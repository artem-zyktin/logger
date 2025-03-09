#pragma once

#include "log_level.hpp"

namespace logger
{

struct LoggerConfig
{
	Level log_level = Level::DEBUG;
	std::string log_file_path = "log.log";
};

}
