#pragma once

#include "logger_concepts.hpp"

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
	Logger()
	{
		(init_if_needed<Policies>(), ...);
	}

	~Logger()
	{
		(release_if_needed<Policies>(), ...);
	}

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

	Level get_log_level() const { return log_level_; }
	void set_log_level(Level log_level) { log_level_ = log_level; }

private:

	inline std::string get_now_str() const;

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

	mutable std::mutex log_mutex_ {};
	Level log_level_ = Level::DEBUG;

	static constexpr std::array<std::string_view, 4> level_strings_ = {
		"DEBUG", "INFO", "WARNING", "ERROR"
	};
};

template<logger_policy ...Policies>
inline void Logger<Policies...>::log(Level level, std::string_view message) const
{
	if (level < log_level_)
		return;

	std::scoped_lock lock(log_mutex_);

	std::string_view level_name = log_leveL_to_str(level);
	std::string time = get_now_str();

	std::string log_entry = std::format("[{}][{}] {}", std::move(time), level_name, message);

	(Policies::write(log_entry), ...);
}

template<logger_policy ...Policies>
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
concept logger_has_policy = is_logger<T> && has_policy_v<T, P>;

template<class T, class P>
concept logger_has_no_policy = is_logger<T> && !has_policy_v<T, P>;

}