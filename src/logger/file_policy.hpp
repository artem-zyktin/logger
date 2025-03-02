#pragma once

#include <string_view>
#include <iostream>
#include <fstream>
#include <mutex>

namespace logger
{

class DefaultFileLoggerPolicy
{
public:
	static void set_file_path(std::string_view file_path);
	static void write(std::string_view message);

private:
	static std::ofstream log_file_;
	static std::mutex log_file_mutex_;
};

}
