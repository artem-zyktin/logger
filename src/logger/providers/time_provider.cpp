#include "time_provider.hpp"

#include <chrono>
#include <format>

namespace logger
{ 

std::string DefaultTimeProvider::now() const
{
	const auto now = std::chrono::system_clock::now();
	const auto now_time_t = std::chrono::system_clock::to_time_t(now);
	const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	const auto tz_offset = std::chrono::duration_cast<std::chrono::minutes>(
		std::chrono::current_zone()->get_info(now).offset
	).count();

	const std::chrono::sys_seconds sys_time = std::chrono::floor<std::chrono::seconds>(now);

	return std::format("{:%Y-%m-%d %H:%M:%S}.{:03d} UTC{:+}", sys_time, now_ms.count(), tz_offset / 60);
}

std::string MokTimeProvider::now() const
{
    return "mok date and time";
}

}
