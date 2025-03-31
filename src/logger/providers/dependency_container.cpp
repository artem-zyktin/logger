#include "dependency_container.hpp"
#include "time_provider.hpp"

#include <array>

namespace 
{

struct DefaultProvidersInittializer
{
	DefaultProvidersInittializer()
	{
		logger::init_default_prividers();
	}
};

const DefaultProvidersInittializer initializer_ = {};

}

namespace logger
{

void init_default_prividers()
{
	DependencyContainer::emplace<TimeProvider, DefaultTimeProvider>();
}

}
