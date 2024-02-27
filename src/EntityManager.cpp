#include "EntityManager.h"
#include <iostream>


void EntityManager::update()
{
	addWaitingEntities();
	removeDeadEntities();
}



Entity EntityManager::addEntity(const std::string& tag)
{
	size_t id = EntityMemoryPool::Instance().addEntity(tag);
	Entity e(id);
	m_toAdd.push_back(e);
	return e;
}

Entity EntityManager::addCopy(Entity e)
{
	size_t id = EntityMemoryPool::Instance().addCopy(e.id());
	Entity newEntity(id);
	m_toAdd.push_back(newEntity);
	return newEntity;
}

EntityVec& EntityManager::getEntities() 
{
	return m_entities;
}
EntityMap& EntityManager::getEntityMap()
{
	return m_entityMap;
}


EntityVec& EntityManager::getEntities(const std::string& tag) 
{
	return m_entityMap[tag];
}

void EntityManager::addWaitingEntities()
{
	for (auto& e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e.tag()].push_back(e);
	}
	m_toAdd.clear();
}
void EntityManager::removeDeadEntities()
{
	// removing dead entities from m_entities C++ 20 version
	std::erase_if(m_entities,
		[](const Entity e) { return !e.isAlive(); }
	);

	//Removing dead entities from the map
	for (auto& [tag, eVec] : m_entityMap)
	{
		std::erase_if(eVec,
			[](const Entity& e) { return !e.isAlive(); }
		);
	}
	// clearing the recent freed entities
	// from the memory pool
	EntityMemoryPool::Instance().clearRecentFreed();
}