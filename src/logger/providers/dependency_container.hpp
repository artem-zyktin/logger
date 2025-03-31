#pragma once

#include <unordered_map>
#include <mutex>
#include <typeindex>
#include <type_traits>
#include <memory>
#include <any>

namespace logger
{

class DependencyContainer
{
public:
	template<class Interface, class Impl, class... Args>
		requires std::is_base_of_v<Interface, Impl>
	static void emplace(Args&&... args);

	template<class Interface>
	static std::shared_ptr<Interface> get();

	template<class Interface, class Impl>
		requires std::is_base_of_v<Interface, Impl>
	static void set(std::shared_ptr<Impl> provider);

private:
	static inline std::unordered_map<std::type_index, std::any> services_ = {};
	static inline std::mutex mutex_ = {};
};

template<class Interface, class Impl, class ...Args>
	requires std::is_base_of_v<Interface, Impl>
inline void DependencyContainer::emplace(Args && ...args)
{
	std::scoped_lock lock (mutex_);
	services_[typeid(Interface)] = std::static_pointer_cast<Interface>(std::make_shared<Impl>(std::forward<Args>(args)...));
}

template<class Interface>
inline std::shared_ptr<Interface> DependencyContainer::get()
{
	std::scoped_lock lock(mutex_);

	if (auto it = services_.find(typeid(Interface)); it != services_.end())
	{
		return std::any_cast<std::shared_ptr<Interface>>(it->second);
	}
	else
	{
		return nullptr;
	}
}

template<class Interface, class Impl>
	requires std::is_base_of_v<Interface, Impl>
inline void DependencyContainer::set(std::shared_ptr<Impl> provider)
{
	std::scoped_lock lock(mutex_);
	services_[typeid(Interface)] = std::static_pointer_cast<Interface>(std::move(provider));
}

} // namespace logger::internal
