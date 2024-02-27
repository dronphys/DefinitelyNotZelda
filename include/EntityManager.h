#pragma once
#include <vector>
#include <set>
#include <memory>
#include <map>
#include <string>
#include <algorithm>
#include "Entity.h"
#include "MemoryPool.h"

typedef std::vector<Entity> EntityVec;
typedef std::map   <std::string, EntityVec>  EntityMap;

// EntityManager class that creates entities and helds their storage. Factory pattern.
class EntityManager
{
public:
	EntityManager() = default;
	void update(); //Function must be called every frame in game loop. Destroys and adds entities in m_entities
	Entity addEntity(const std::string& tag); // create a new entity of given tag and return ptr* of it
	Entity addCopy(Entity e); // creates a copy of the given entity
	EntityVec& getEntities(); // returns m_entities
	EntityVec& getEntities(const std::string& tag); // returns entities with a given tag
	EntityMap& getEntityMap();
private:
	EntityVec m_entities;		  // main vector of all Entities
	EntityVec m_toAdd;			  // Vector used to add entities to m_entities in update
	EntityMap m_entityMap;		  // Map with entities from entity tag, to quickly access needed entities
	size_t    m_totalEntities = 0;//
	void addWaitingEntities();
	void removeDeadEntities();
};

