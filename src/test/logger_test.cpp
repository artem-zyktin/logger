#include "logger/logger.hpp"
#include "logger/default_console_policy.hpp"
#include "logger/default_file_policy.hpp"
#include "logger/logger_config.hpp"

#include <gtest/gtest.h>

#include <fstream>
#include <filesystem>



namespace fs = std::filesystem;

namespace logger_test
{

TEST(LoggerTest, BasicLogging)
{
	using logger_t = logger::Logger<logger::DefaultConsoleLoggerPolicy>;
	logger_t log;

	EXPECT_NO_THROW(log.log(logger::Level::INFO, "Test message"));
}

TEST(LoggerTest, FileLogging)
{
	using logger_t = logger::Logger<logger::DefaultFileLoggerPolicy>;
	std::string log_file = "test_log.txt";
	logger::DefaultFileLoggerPolicy::set_file_path(log_file);

	logger_t log;
	log.log(logger::Level::INFO, "File log test");

	logger::DefaultFileLoggerPolicy::release();

	std::ifstream file(log_file);
	ASSERT_TRUE(file.is_open());

	std::string content;
	std::getline(file, content);
	file.close();

	auto rstl = content.find("File log test");

	EXPECT_NE(rstl, std::string::npos);
	fs::remove(log_file);
}

TEST(LoggerTest, LogLevelParsing)
{
	EXPECT_EQ(logger::str_to_level("debug"), logger::Level::DEBUG);
	EXPECT_EQ(logger::str_to_level("info"), logger::Level::INFO);
	EXPECT_EQ(logger::str_to_level("warning"), logger::Level::WARNING);
	EXPECT_EQ(logger::str_to_level("error"), logger::Level::ERROR);

	EXPECT_EQ(logger::level_to_str(logger::Level::DEBUG), "debug");
	EXPECT_EQ(logger::level_to_str(logger::Level::INFO), "info");
	EXPECT_EQ(logger::level_to_str(logger::Level::WARNING), "warning");
	EXPECT_EQ(logger::level_to_str(logger::Level::ERROR), "error");
}

TEST(LoggerTest, LogLevelParsingException)
{
	EXPECT_THROW(logger::str_to_level("UKNOWN"), std::runtime_error);
	EXPECT_THROW(logger::level_to_str(static_cast<logger::Level>(42)), std::runtime_error);
}

TEST(LoggerTest, ConfigParsing)
{
	constexpr std::string_view log_file = "log.txt";
	constexpr std::string_view log_pattern = "[{{level}}][{{time}}][{{thread-id}}] {{message}}";

	constexpr std::string_view json_config = R"(
	{{
		"logger" : {{
			"log_file": "{}",
			"log_level": "info",
			"log_pattern": "{}"
		}}
	}})";

	std::string json_text = std::format(json_config, log_file, log_pattern);

	auto config = logger::read_config_from_json(json_text);

	EXPECT_EQ(config.log_file_path, log_file);
	EXPECT_EQ(config.log_level, logger::Level::INFO);
}

TEST(LoggerTest, ConfigParsingFromFile)
{
	constexpr std::string_view log_file = "log.txt";
	constexpr std::string_view log_pattern = "[{{level}}][{{time}}][{{thread-id}}] {{message}}";

	constexpr std::string_view json_config = R"(
	{{
		"logger" : {{
			"log_file": "{}",
			"log_level": "info",
			"log_pattern": "{}"
		}}
	}})";

	std::string json_text = std::format(json_config, log_file, log_pattern);

	const char config_path[] = "config_log.json";

	std::ofstream config_file (config_path);
	config_file << json_text << std::endl;
	config_file.close();

	auto config = logger::read_config(static_cast<fs::path>(config_path));

	EXPECT_EQ(config.log_file_path, log_file);
	EXPECT_EQ(config.log_level, logger::Level::INFO);
	EXPECT_EQ(config.log_pattern, log_pattern);

	fs::remove(config_path);
}

TEST(LoggerTest, ConfigParsingValidatingSuccess)
{

	logger::LoggerConfig config;
	config.log_file_path = "log.txt";
	config.log_level = logger::Level::INFO;
	config.log_pattern = "[{{level}}][{{time}}][{{thread-id}}] {{message}}";

	auto result = logger::validate_config(config);
	EXPECT_TRUE(std::get<0>(result));
}

TEST(LoggerTest, ConfigParsingValidatingFailure)
{
	logger::LoggerConfig config;
	config.log_file_path = "log.txt";
	config.log_level = logger::Level::INFO;
	config.log_pattern = "[{{level}}][{{time}}][{{thread-id}] {{message}}";

	auto result = logger::validate_config(config);
	EXPECT_FALSE(std::get<0>(result));
}

TEST(LoggerTest, DependencyContainer)
{
	auto mok_time_provider = std::make_shared<logger::MokTimeProvider>();
	logger::DependencyContainer::set<logger::TimeProvider>(mok_time_provider);

	auto mok_time_provider_ptr = logger::DependencyContainer::get<logger::TimeProvider>();

	EXPECT_EQ(mok_time_provider, mok_time_provider_ptr);
}

struct MokStringPolicy
{
	inline static std::string output;

	static void write(std::string_view message)
	{
		output = message;
	}
};

TEST(LoggerTest, MessageFormatFromConfig)
{
	logger::LoggerConfig config;
	config.log_level = logger::Level::WARNING;
	config.log_pattern = "[{{time}}][{{level}}][{{thread-id}}] {{message}}";

	std::string_view check_pattern = "[{0}][{2}][{1}] {3}";
	std::string_view message = "some text message";

	std::stringstream ss;
	ss << std::this_thread::get_id();
	std::string thread_id = ss.str();

	auto time_provider = logger::DependencyContainer::get<logger::TimeProvider>();

	{
		auto log = logger::Logger<MokStringPolicy>(config);
		log.error(message);
	}

	std::string check_message = std::vformat(check_pattern, std::make_format_args(time_provider->now(),
														    thread_id,
														    logger::level_to_str(logger::Level::ERROR),
														    message));

	EXPECT_EQ(check_message, MokStringPolicy::output);
}

}

int main(int argc, char* argv[])
{
	logger::DependencyContainer::set<logger::TimeProvider>(std::make_shared<logger::MokTimeProvider>());

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}