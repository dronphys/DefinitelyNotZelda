#pragma once
#include <string>
#include <memory>
#include "Components.h"
#include <tuple>
#include "MemoryPool.h"


class Entity
{
private:
	size_t	m_id = 0;
public:
	Entity() = default;
	Entity( size_t id)
		: m_id(id) { }

	size_t			   id()  const    { return m_id; }
	bool			   isAlive() const;
	void			   destroy();

	const std::string& tag() const
	{ 
		return  EntityMemoryPool::Instance().tag(m_id);
	}

	template<typename T>
	T& getComponent()
	{
		return EntityMemoryPool::Instance().getComponent<T>(m_id);
	}

	// variadic templates and perfect forwarding
	template<typename T, typename ... Args>
	T& addComponent(Args&& ... args)
	{
		auto& component = EntityMemoryPool::Instance().getComponent<T>(m_id);
		component = T(std::forward<Args>(args)...); // perfect forwarding
		component.has = true;
		return component;
	}

	template <typename T>
	bool has() const
	{
		return EntityMemoryPool::Instance().has<T>(m_id);
	}

	template <typename T>
	void removeComonent()
	{
		EntityMemoryPool::Instance().getComponent<T>(m_id).has = false;
	}
	//ComponentTuple& getAllComponents()
	//{
	//	return m_components;
	//}
	bool operator==(const Entity& other) const;
};