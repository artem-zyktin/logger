#include "default_console_policy.hpp"

#include <iostream>

namespace logger
{

void DefaultConsoleLoggerPolicy::write(const std::string_view message)
{
	std::cout << message << std::endl;
}

}
