﻿#pragma once

#include "logger_concepts.hpp"
#include "log_level.hpp"
#include "logger_config.hpp"

#include <array>
#include <string>
#include <strstream>
#include <string_view>
#include <time.h>
#include <mutex>
#include <chrono>
#include <thread>

namespace chrono = std::chrono;

namespace logger
{

template<logger_policy... Policies>
class Logger
{
public:
	using Level = Level;

	Logger(LoggerConfig config = LoggerConfig())
		: config_(std::move(config))
	{
		(init_if_needed<Policies>(), ...);
	}

	~Logger()
	{
		(release_if_needed<Policies>(), ...);
	}

	Logger(Logger&&) = delete;
	Logger& operator=(Logger&&) = delete;
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	void log(Level level, const std::string_view message) const;

	inline void debug(const std::string_view message)   const { log(Level::DEBUG, message); }
	inline void info(const std::string_view message)    const { log(Level::INFO, message); }
	inline void warning(const std::string_view message) const { log(Level::WARNING, message); }
	inline void error(const std::string_view message)   const { log(Level::ERROR, message); }

	const LoggerConfig& get_config() const { return config_; }

private:

	inline std::string get_now_str() const;
	inline std::string get_this_thread_id() const;

	template<class Policy>
	inline void init_if_needed() const
	{
		if constexpr (initialized_policy<Policy>)
			Policy::init();
	}

	template<class Policy>
	inline void release_if_needed() const
	{
		if constexpr (releasable_policy<Policy>)
			Policy::release();
	}

	mutable std::mutex log_mutex_ = std::mutex();
	const LoggerConfig config_;
}; // class Logger

template<logger_policy ...Policies>
inline void Logger<Policies...>::log(Level level, const std::string_view message) const
{
	if (level < config_.log_level)
		return;

	std::scoped_lock lock(log_mutex_);

	const std::string_view level_name = level_to_str(level);
	const std::string time = get_now_str();

	const std::string log_entry = std::format("[{}][thread-id={}][{}] {}", std::move(time),
																		   get_this_thread_id(),
																		   level_name,
																		   message);

	(Policies::write(log_entry), ...);
}

template<logger_policy ...Policies>
inline std::string Logger<Policies...>::get_now_str() const
{
	const auto now = std::chrono::system_clock::now();
	const auto now_time_t = std::chrono::system_clock::to_time_t(now);
	const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	const std::tm tm_local = *std::localtime(&now_time_t);

	const auto tz_offset = std::chrono::duration_cast<std::chrono::minutes>(
		std::chrono::current_zone()->get_info(now).offset
	).count();

	const std::chrono::sys_seconds sys_time = std::chrono::floor<std::chrono::seconds>(now);

	return std::format("{:%Y-%m-%d %H:%M:%S}.{:03d} UTC{:+}", sys_time, now_ms.count(), tz_offset / 60);
}

template<logger_policy ...Policies>
inline std::string Logger<Policies...>::get_this_thread_id() const
{
	std::stringstream ss;
	ss << std::this_thread::get_id();

	return ss.str();
}

template<class T, class P>
constexpr bool has_policy_v = false;

template<class P, class... Policies>
constexpr bool has_policy_v<Logger<Policies...>, P> = (std::is_same_v<Policies, P> || ...);

template<class T, class P>
concept logger_has_policy = is_logger<T> && has_policy_v<T, P>;

template<class T, class P>
concept logger_has_no_policy = is_logger<T> && !has_policy_v<T, P>;

} // namespace logger