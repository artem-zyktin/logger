#include <logger/logger.hpp>
#include <logger/default_console_policy.hpp>
#include <logger/default_file_policy.hpp>

#include <string_view>

using ConsoleLogger     = logger::Logger<logger::DefaultConsoleLoggerPolicy>;
using FileLogger        = logger::Logger<logger::DefaultFileLoggerPolicy>;
using ConsoleFileLogger = logger::Logger<logger::DefaultConsoleLoggerPolicy, logger::DefaultFileLoggerPolicy>;

constexpr std::string_view debug_message = "some debug message";
constexpr std::string_view info_message = "some info message";
constexpr std::string_view warning_message = "some warning message";
constexpr std::string_view error_message = "some error message";

static_assert(logger::logger_type<ConsoleLogger>);
static_assert(logger::logger_type<FileLogger>);
static_assert(logger::logger_type<ConsoleFileLogger>);

template<logger::logger_type LoggerT>
void test()
{
	LoggerT logger;

	if constexpr (logger::logger_has_policy<LoggerT, logger::DefaultFileLoggerPolicy>)
		logger::DefaultFileLoggerPolicy::set_file_path("log.log");

	logger.log(LoggerT::Level::DEBUG, debug_message);
	logger.log(LoggerT::Level::INFO, info_message);
	logger.log(LoggerT::Level::WARNING, warning_message);
	logger.log(LoggerT::Level::ERROR, error_message);

	logger.debug(debug_message);
	logger.info(info_message);
	logger.warning(warning_message);
	logger.error(error_message);

	logger.set_log_level(LoggerT::Level::INFO);

	logger.log(LoggerT::Level::DEBUG, debug_message); // do not should output anything
	logger.log(LoggerT::Level::INFO, info_message);
	logger.log(LoggerT::Level::WARNING, warning_message);
	logger.log(LoggerT::Level::ERROR, error_message);

	logger.debug(debug_message); // do not should output anything
	logger.info(info_message);
	logger.warning(warning_message);
	logger.error(error_message);

	if (LoggerT::Level::INFO == logger.get_log_level())
		logger.info("OKAY");
}

int main()
{
	test<ConsoleLogger>();
	test<FileLogger>();
	test<ConsoleFileLogger>();

	return 0;
}