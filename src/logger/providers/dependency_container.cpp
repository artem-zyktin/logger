#include "dependency_container.hpp"
#include "time_provider.hpp"

#include <array>

namespace 
{

struct DefaultProvidersInittializer
{
	DefaultProvidersInittializer()
	{
		logger::DependencyContainer::emplace<logger::TimeProvider, logger::DefaultTimeProvider>();
	}
};

const DefaultProvidersInittializer initializer_ = {};

}
