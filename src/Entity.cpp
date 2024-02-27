#include "Entity.h"

bool Entity::operator==(const Entity& other) const
{
	return m_id == other.m_id;
}

bool Entity::isAlive() const
{
	return EntityMemoryPool::Instance().isActive(m_id);
}

void Entity::destroy()
{
	EntityMemoryPool::Instance().removeEntity(m_id);
}