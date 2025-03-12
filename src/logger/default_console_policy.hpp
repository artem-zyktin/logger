#pragma once

#include <string_view>

namespace logger
{

struct DefaultConsoleLoggerPolicy
{
	static void write(const std::string_view);
};

}
