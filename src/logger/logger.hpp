﻿#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <string_view>
#include <time.h>
#include <mutex>
#include <chrono>

namespace chrono = std::chrono;

namespace logger
{

template<class T>
concept LoggerPolicy = requires (std::string_view message)
{
	{ T::write(message) };
};

template<class T>
concept HasLevels = requires
{
	typename T::Level;
	requires std::is_enum_v<typename T::Level>;
	{ T::Level::DEBUG };
	{ T::Level::INFO };
	{ T::Level::WARNING };
	{ T::Level::ERROR };
};

template<class T>
concept IsLogger = HasLevels<T> && requires (const T logger, typename T::Level level, std::string_view message)
{
	{ logger.log(level, message) };
	{ logger.debug(message) };
	{ logger.info(message) };
	{ logger.warning(message) };
	{ logger.error(message) };
};

template<class T>
concept LoggerType = IsLogger<T>;

template<LoggerPolicy... Policies>
class Logger
{
public:
	Logger() = default;
	~Logger() = default;

	Logger(Logger&&) noexcept = default;
	Logger& operator=(Logger&&) noexcept = default;

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;


	enum class Level : uint16_t
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR
	};

	void log(Level level, std::string_view message) const;

	inline void debug(std::string_view message)   const { log(Level::DEBUG, message); }
	inline void info(std::string_view message)    const { log(Level::INFO, message); }
	inline void warning(std::string_view message) const { log(Level::WARNING, message); }
	inline void error(std::string_view message)   const { log(Level::ERROR, message); }

	inline std::string_view log_leveL_to_str(Level level) const { return level_strings_[static_cast<size_t>(level)]; }

private:

	inline std::string get_now_str() const;

	mutable std::mutex log_mutex_ {};

	static constexpr std::array<std::string_view, 4> level_strings_ = {
		"DEBUG", "INFO", "WARNING", "ERROR"
	};
};

template<LoggerPolicy ...Policies>
inline void Logger<Policies...>::log(Level level, std::string_view message) const
{
	std::scoped_lock lock(log_mutex_);

	std::string_view level_name = log_leveL_to_str(level);
	std::string time = get_now_str();

	std::string log_entry = std::format("[{}][{}] {}", std::move(time), level_name, message);

	(Policies::write(log_entry), ...);
}

template<LoggerPolicy ...Policies>
inline std::string Logger<Policies...>::get_now_str() const
{
	auto now = std::chrono::system_clock::now();
	auto now_time_t = std::chrono::system_clock::to_time_t(now);
	auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	std::tm tm_local = *std::localtime(&now_time_t);

	auto tz_offset = std::chrono::duration_cast<std::chrono::minutes>(
		std::chrono::current_zone()->get_info(now).offset
	).count();

	std::chrono::sys_seconds sys_time = std::chrono::floor<std::chrono::seconds>(now);

	return std::format("{:%Y-%m-%d %H:%M:%S}.{:03d} UTC{:+}", sys_time, now_ms.count(), tz_offset / 60);
}

template<class T, class P>
constexpr bool has_policy_v = false;

template<class P, class... Policies>
constexpr bool has_policy_v<Logger<Policies...>, P> = (std::is_same_v<Policies, P> || ...);

template<class T, class P>
concept HasPolicy = IsLogger<T> && has_policy_v<T, P>;

template<class T, class P>
concept HasNoPolicy = IsLogger<T> && !has_policy_v<T, P>;

}