#pragma once

#include <string>

namespace logger
{

struct TimeProvider
{
	virtual ~TimeProvider() {};
	virtual std::string now() const = 0;
};

struct DefaultTimeProvider : TimeProvider
{
	std::string now() const override;
};

struct MokTimeProvider : TimeProvider
{
	std::string now() const override;
};

}
