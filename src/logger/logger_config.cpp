#include "logger_config.hpp"
#include "log_level.hpp"
#include "utils.hpp"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <iostream>
#include <functional>
#include <array>
#include <format>

namespace fs = std::filesystem;

namespace
{

inline void replace_placeholder(std::string& pattern, std::string_view placeholder, std::string_view value)
{
	size_t index = 0;
	while ((index = pattern.find(placeholder, index)) != std::string::npos)
	{
		pattern.replace(index, placeholder.size(), value);
		index += value.size();
	}
}

}

namespace logger
{

void replace_log_pattern_placeholders(std::string& pattern)
{
	using value_t = std::pair<std::string_view, std::string_view>;

	static constexpr std::array<value_t, 4> variables = { {
		{ "{{time}}",      "{0}" },
		{ "{{thread-id}}", "{1}" },
		{ "{{level}}",     "{2}" } ,
		{ "{{message}}",   "{3}" }
	} };

	std::ranges::for_each(variables, [&pattern](const value_t& item) mutable
	{
		replace_placeholder(pattern, item.first, item.second);
	});
}

}

namespace
{

using namespace rapidjson;
using namespace logger;
using namespace std::literals;

inline void warning(const std::string_view message)
{
	std::cerr << "Warning: " << message << std::endl;
}


std::string parse_config_str(Value const* const section, std::string_view member_name, std::string_view default_value = "")
{
	if (section->HasMember(member_name.data()) && (*section)[member_name.data()].IsString())
		return (*section)[member_name.data()].GetString();

	return std::string(default_value);
}

std::string parse_log_file(Value const * const logger_section)
{
	std::string result = parse_config_str(logger_section, "log_file", "");
	if (result.empty())
			warning("\"log_file\" must be a string. Default value will be assigned.");

	return result;
}

Level parse_log_level(Value const * const logger_section)
{
	return str_to_level(parse_config_str(logger_section, "log_level", "debug"));
}

std::string parse_log_pattern(Value const * const logger_section)
{
	return parse_config_str(logger_section, "log_pattern", DEFAULT_LOG_PATTERN);
}

bool validate_config_log_pattern(const LoggerConfig& config)
{
	std::string log_pattern = copy(config.log_pattern);
	replace_log_pattern_placeholders(log_pattern);

	try
	{
		(void)std::vformat(log_pattern, std::make_format_args("0"sv, "1"sv, "2"sv, "3"sv));
	}
	catch (const std::format_error&)
	{
		return false;
	}

	return true;
}

} // namespace

namespace logger
{

LoggerConfig logger::read_config(const fs::path& file)
{
	const std::string json_text = read_file(file);

	if (json_text.size() > 2ull * 1024ull * 1024ull) // 2 MB
		warning(std::format("file \"{}\" larger that 2MB. logger::read_config is not designed for large files reading.", file.string()));


	return read_config_from_json(json_text);
}

LoggerConfig read_config_from_json(const std::string& json_text)
{
	using namespace rapidjson;

	Document doc;
	ParseResult parse_result = doc.Parse(json_text.c_str());

	if (!static_cast<bool>(parse_result))
		throw std::runtime_error(std::format("json text parsing error: {}", GetParseError_En(parse_result.Code())));

	if (!doc.IsObject())
		throw std::runtime_error("json document must be an object");

	if (!doc.HasMember("logger"))
		throw std::runtime_error("json document must contain \"logger\" section");

	Value const * logger_section = &doc["logger"];
	if (!logger_section)
		throw std::runtime_error("can't detect \"logger\" section in json document");

	LoggerConfig config {};

	std::string log_file_path = parse_log_file(logger_section);
	if (!log_file_path.empty())
		config.log_file_path = std::move(log_file_path);

	config.log_level = parse_log_level(logger_section);

	config.log_pattern = parse_log_pattern(logger_section);

	return config;
}

validation_result_t validate_config(const LoggerConfig& config)
{
	using func_t = bool(const LoggerConfig&);
	using value_t = std::pair<func_t*, std::string_view>;

	static std::array<value_t, 1> validators = { {
		{ &validate_config_log_pattern, "invalid log_pattern" },
	} };

	bool result = true;
	std::string message = "";

	(void) std::ranges::find_if_not(validators, [&result, &message, &config](const value_t& validator) mutable
	{
		result = validator.first(config);
		if (!result)
			message = validator.second;

		return result;
	});

	return { result, message };
}

}// namespace logger
