#include "logger/logger.hpp"
#include "logger/default_console_policy.hpp"

#include <gtest/gtest.h>

namespace logger_test
{

TEST(LoggerTest, BasicLogging)
{
	using logger_t = logger::Logger<logger::DefaultConsoleLoggerPolicy>;

	logger_t log ();

	EXPECT_TRUE(true);
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}