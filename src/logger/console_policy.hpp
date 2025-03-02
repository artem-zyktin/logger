#pragma once

#include <string_view>

namespace logger
{

struct DefaultConsoleLoggerPolicy
{
	static void write(std::string_view);
};

}