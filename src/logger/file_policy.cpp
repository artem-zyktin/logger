#include "file_policy.hpp"

namespace logger
{

std::ofstream DefaultFileLoggerPolicy::log_file_;
std::mutex DefaultFileLoggerPolicy::log_file_mutex_;

void DefaultFileLoggerPolicy::set_file_path(std::string_view file_path)
{
	std::scoped_lock lock(log_file_mutex_);

	if (log_file_.is_open())
		log_file_.close();

	log_file_.open(std::string(file_path), std::ios::out | std::ios::app);
}

void DefaultFileLoggerPolicy::write(std::string_view message)
{
	std::scoped_lock lock(log_file_mutex_);

	if (!log_file_.is_open())
		return;

	log_file_ << message << std::endl;
}

}
