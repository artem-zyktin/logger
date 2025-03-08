#pragma once

#include <type_traits>
#include <string_view>

namespace logger
{

template<class T>
concept LoggerPolicy = requires (std::string_view message)
{
	{ T::write(message) };
};

template<class T>
concept InitializedPolicy = LoggerPolicy<T> && requires
{
	{ T::init() };
};

template<class T>
concept ReleasablePolicy = LoggerPolicy<T> && requires
{
	{ T::release() };
};

template<class Policy, class... Policies>
concept IsPolisyInList = (std::same_as<Policy, Policies> || ...);

template<class Policy, class... Policies>
concept HasPolicy = IsPolisyInList<Policy, Policies...>;

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
concept IsLogger = HasLevels<T> && requires (T logger, const T const_logger, typename T::Level level, std::string_view message)
{
	{ const_logger.log(level, message) };
	{ const_logger.debug(message) };
	{ const_logger.info(message) };
	{ const_logger.warning(message) };
	{ const_logger.error(message) };

	{ logger.set_log_level(level) };
	{ logger.get_log_level() } -> std::same_as<typename T::Level>;
};

template<class T>
concept LoggerType = IsLogger<T>;

}
