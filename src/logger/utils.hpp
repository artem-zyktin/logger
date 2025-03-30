#pragma once

#include <filesystem>

namespace logger
{
	std::string read_file(const std::filesystem::path& filepath);
	
	template<class T>
		requires std::is_copy_constructible_v<std::remove_cvref_t<T>>
	constexpr std::remove_cvref_t<T> copy(const T& value)
	{
		return value;
	}

} // namespace logger
