#include <logger/logger.hpp>
#include <logger/console_policy.hpp>
#include <logger/file_policy.hpp>

#include <string_view>

using ConsoleLogger     = logger::Logger<logger::DefaultConsoleLoggerPolicy>;
using FileLogger        = logger::Logger<logger::DefaultFileLoggerPolicy>;
using ConsoleFileLogger = logger::Logger<logger::DefaultConsoleLoggerPolicy, logger::DefaultFileLoggerPolicy>;

constexpr std::string_view debug_message = "some debug message";
constexpr std::string_view info_message = "some info message";
constexpr std::string_view warning_message = "some warning message";
constexpr std::string_view error_message = "some error message";

static_assert(logger::LoggerType<ConsoleLogger>);
static_assert(logger::LoggerType<FileLogger>);
static_assert(logger::LoggerType<ConsoleFileLogger>);

template<logger::LoggerType T>
void test()
{
	const T logger;

	if constexpr (logger::HasPolicy<T, logger::DefaultFileLoggerPolicy>)
		logger::DefaultFileLoggerPolicy::set_file_path("log.log");

	logger.log(T::Level::DEBUG, debug_message);
	logger.log(T::Level::INFO, info_message);
	logger.log(T::Level::WARNING, warning_message);
	logger.log(T::Level::ERROR, error_message);

	logger.debug(debug_message);
	logger.info(info_message);
	logger.warning(warning_message);
	logger.error(error_message);
}

int main()
{
	test<ConsoleLogger>();
	test<FileLogger>();
	test<ConsoleFileLogger>();

	return 0;
}