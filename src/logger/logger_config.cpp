#include "logger_config.hpp"
#include "log_level.hpp"
#include "utils.hpp"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <iostream>

namespace fs = std::filesystem;

namespace
{

using namespace rapidjson;
using namespace logger;
using namespace std::literals;

inline void warning(const std::string_view message)
{
	std::cerr << "Warning: " << message << std::endl;
}

std::string parse_log_file(Value const * const logger_section)
{
	std::string result;
	if (logger_section->HasMember("log_file"))
	{
		if ((*logger_section)["log_file"].IsString())
			result = (*logger_section)["log_file"].GetString();
		else
			warning("\"log_file\" must be a string. Default value will be assigned.");
	}

	return result;
}

Level parse_log_level(Value const * const logger_section)
{
	Level result = DEFAULT_LOG_LEVEL;
	if (logger_section->HasMember("log_level"))
	{
		if ((*logger_section)["log_level"].IsString())
			result = str_to_level((*logger_section)["log_level"].GetString());
	}

	return result;
}

std::string parse_log_pattern(Value const * const logger_section)
{
	std::string result = std::string(DEFAULT_LOG_PATTERN);
	if (logger_section->HasMember("log_pattern"))
	{
		if ((*logger_section)["log_pattern"].IsString())
			result = std::string((*logger_section)["log_pattern"].GetString());
	}

	return result;
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

}// namespace logger
