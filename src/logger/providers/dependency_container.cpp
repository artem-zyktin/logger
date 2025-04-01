#include "dependency_container.hpp"
#include "time_provider.hpp"

#include <array>

namespace 
{

struct DefaultProvidersInitializer
{
	DefaultProvidersInitializer()
	{
		logger::DependencyContainer::emplace<logger::TimeProvider, logger::DefaultTimeProvider>();
	}
};

const DefaultProvidersInitializer initializer_ = {};

}
