#pragma once

#include <type_traits>
#include <string_view>

namespace logger
{

template<class T>
concept logger_policy = requires (const std::string_view message)
{
	{ T::write(message) };
};

template<class T>
concept initialized_policy = logger_policy<T> && requires
{
	{ T::init() };
};

template<class T>
concept releasable_policy = logger_policy<T> && requires
{
	{ T::release() };
};

template<class Policy, class... Policies>
concept is_policy_in_list = (std::same_as<Policy, Policies> || ...);

template<class Policy, class... Policies>
concept has_policy = is_policy_in_list<Policy, Policies...>;

template<class T>
concept has_levels = requires
{
	typename T::Level;
	requires std::is_enum_v<typename T::Level>;
	{ T::Level::DEBUG };
	{ T::Level::INFO };
	{ T::Level::WARNING };
	{ T::Level::ERROR };
};

template<class T>
concept is_logger = has_levels<T> && requires (const T logger, const T const_logger, typename T::Level level, const std::string_view message)
{
	{ const_logger.log(level, message) };
	{ const_logger.debug(message) };
	{ const_logger.info(message) };
	{ const_logger.warning(message) };
	{ const_logger.error(message) };
};

template<class T>
concept logger_type = is_logger<T>;

} // namespace logger
