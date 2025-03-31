#pragma once

#include "logger_concepts.hpp"
#include "log_level.hpp"
#include "logger_config.hpp"
#include "utils.hpp"
#include "providers/dependency_container.hpp"
#include "providers/time_provider.hpp"

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

	explicit Logger(LoggerConfig config = LoggerConfig())
		: config_(std::move(config))
	{
		(init_if_needed<Policies>(), ...);

		setup_config();
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

	void setup_config();

	mutable std::mutex log_mutex_ = std::mutex();
	const LoggerConfig config_;
	std::string message_format_;

}; // class Logger

template<logger_policy ...Policies>
inline void Logger<Policies...>::log(Level level, const std::string_view message) const
{
	if (level < config_.log_level)
		return;

	std::scoped_lock lock(log_mutex_);

	std::string now_str = DependencyContainer::get<TimeProvider>()->now();

	const std::string log_entry = std::vformat(message_format_, std::make_format_args(now_str,
															   get_this_thread_id(),
															   level_to_str(level),
															   message));

	(Policies::write(log_entry), ...);
}

template<logger_policy ...Policies>
inline std::string Logger<Policies...>::get_this_thread_id() const
{
	std::stringstream ss;
	ss << std::this_thread::get_id();

	return ss.str();
}

extern void replace_log_pattern_placeholders(std::string& pattern);

template<logger_policy ...Policies>
inline void Logger<Policies...>::setup_config()
{
	const auto [ result, message ] = validate_config(config_);
	if (!result)
		throw std::invalid_argument(message);

	message_format_ = copy(config_.log_pattern);
	replace_log_pattern_placeholders(message_format_);
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