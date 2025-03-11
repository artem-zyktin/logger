#pragma once

#include <cstdint>

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

} // namespace logger
