#pragma once

#include <filesystem>

namespace logger
{
	std::string read_file(const std::filesystem::path& filepath);
} // namespace logger
