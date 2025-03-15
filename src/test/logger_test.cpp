#include "logger/logger.hpp"
#include "logger/default_console_policy.hpp"
#include "logger/default_file_policy.hpp"

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

TEST(LoggerTest, ConfigParsing)
{
	constexpr std::string_view log_file = "log.txt";
	constexpr std::string_view json_config = R"(
	{{
		"logger" : {{
			"log_file": "{}",
			"log_level": "warning"
		}}
	}})";

	std::string json_text = std::format(json_config, log_file);

	auto config = logger::read_config_from_json(json_text);

	EXPECT_EQ(config.log_file_path, log_file);
	EXPECT_EQ(config.log_level, logger::Level::WARNING);
}

TEST(LoggerTest, ConfigParsingFromFile)
{
	constexpr std::string_view log_file = "log.txt";
	constexpr std::string_view json_config = R"(
	{{
		"logger" : {{
			"log_file": "{}",
			"log_level": "info"
		}}
	}})";

	std::string json_text = std::format(json_config, log_file);

	const char config_path[] = "config_log.json";

	std::ofstream config_file (config_path);
	config_file << json_text << std::endl;
	config_file.close();

	auto config = logger::read_config(static_cast<fs::path>(config_path));

	EXPECT_EQ(config.log_file_path, log_file);
	EXPECT_EQ(config.log_level, logger::Level::INFO);

	fs::remove(config_path);
}

} // namespace logger_test

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}