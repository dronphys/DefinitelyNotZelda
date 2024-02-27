#pragma once
#include "Components.h"
#define MAX_ENTITIES 3000
#include <set>
typedef std::tuple<
	std::vector<CTransform>,
	std::vector<CInput>,
	std::vector<CLifeSpan>,
	std::vector<CText>,
	std::vector<CScore>,
	std::vector<CAnimation>,
	std::vector<CBoundingBox>,
	std::vector<CState>,
	std::vector<CDraggable>,
	std::vector<CDamage>,
	std::vector<CLink>,
	std::vector<CHealth>,
	std::vector<CInvincibility>,
	std::vector<CColorModifier>,
	std::vector<CPatrol>,
	std::vector<CPathFind>,
	std::vector<CPrototype>,
	std::vector<CParticleEmitter>
> EntityComponentVectorTuple;



class EntityMemoryPool
{
	size_t		m_numEntities;
	EntityComponentVectorTuple m_data;
	std::vector<std::string> m_tags;
	std::vector<bool>	m_active;
	EntityMemoryPool(size_t maxEntitites);
	/*
	This is used to keep track of the entities that have been recently freed
	so that we don't give their ID to a new entity
	on the same frame, before entity manager has a chance to remove them
	*/
	std::set<size_t> m_recentlyFreed;
public:
	static EntityMemoryPool& Instance();
	const std::string& tag(size_t entityID) const;
	size_t getNextEntityID() const;
	size_t addEntity(const std::string& tag);
	void removeEntity(size_t entityID);
	size_t addCopy(size_t entityID);
	bool isActive(size_t entityID) const;

	/*
	Is function is used to clear set of recently freed entities
	by the entity managed after it has removed all dead entities
	which happens now once a frame
	*/
	void clearRecentFreed();
	template<typename T>
	T& getComponent(size_t entityID);

	template<typename T>
	bool has(size_t entityID);
};

template<typename T>
T& EntityMemoryPool::getComponent(size_t entityID)
{
	return std::get<std::vector<T>>(m_data)[entityID];
}
template<typename T>
bool EntityMemoryPool::has(size_t entityID)
{
	return std::get<std::vector<T>>(m_data)[entityID].has;
}