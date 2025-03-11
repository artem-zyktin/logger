#pragma once

#include <cstdint>
#include <string_view>

namespace logger
{

enum class Level : uint16_t
{
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

constexpr Level DEFAULT_LOG_LEVEL = Level::DEBUG;

/// <summary>
/// Converting string representation of level to logger::Level enum value
/// </summary>
/// <param name="level_str">string representation of level</param>
/// <returns>Level enum value according to level_str or Level::DEBUG if string representation is unknown and write log message</returns>
Level str_to_level(const std::string_view level_str) noexcept;

std::string_view level_to_str(Level level) noexcept;

} // namespace logger
