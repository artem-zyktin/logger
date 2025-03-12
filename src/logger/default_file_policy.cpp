#include "default_file_policy.hpp"

namespace logger
{

std::ofstream DefaultFileLoggerPolicy::log_file_;
std::mutex DefaultFileLoggerPolicy::log_file_mutex_;

void DefaultFileLoggerPolicy::set_file_path(const std::string_view file_path)
{
	release();

	std::scoped_lock lock(log_file_mutex_);
	log_file_.open(std::string(file_path), std::ios::out | std::ios::app);
}

void DefaultFileLoggerPolicy::release()
{
	std::scoped_lock lock(log_file_mutex_);

	if (log_file_.is_open())
		log_file_.close();
}

void DefaultFileLoggerPolicy::write(const std::string_view message)
{
	std::scoped_lock lock(log_file_mutex_);

	if (!log_file_.is_open())
		return;

	log_file_ << message << std::endl;
}

} // namespace logger
