#include "MemoryPool.h"

EntityMemoryPool::EntityMemoryPool(size_t maxEntities)
	:m_numEntities(maxEntities)
	,m_tags(m_numEntities)
	,m_active(m_numEntities, false)
{
	m_data =
		std::make_tuple(
			std::vector<CTransform>(m_numEntities),
			std::vector<CInput>(m_numEntities),
			std::vector<CLifeSpan>(m_numEntities),
			std::vector<CText>(m_numEntities),
			std::vector<CScore>(m_numEntities),
			std::vector<CAnimation>(m_numEntities),
			std::vector<CBoundingBox>(m_numEntities),
			std::vector<CState>(m_numEntities),
			std::vector<CDraggable>(m_numEntities),
			std::vector<CDamage>(m_numEntities),
			std::vector<CLink>(m_numEntities),
			std::vector<CHealth>(m_numEntities),
			std::vector<CInvincibility>(m_numEntities),
			std::vector<CColorModifier>(m_numEntities),
			std::vector<CPatrol>(m_numEntities),
			std::vector<CPathFind>(m_numEntities),
			std::vector<CPrototype>(m_numEntities),
			std::vector<CParticleEmitter>(m_numEntities)
		);
}


const std::string& EntityMemoryPool::tag(size_t entityID) const
{
	return m_tags[entityID];
}

EntityMemoryPool& EntityMemoryPool::Instance()
{
	static EntityMemoryPool pool(MAX_ENTITIES);
	return pool;
}

size_t EntityMemoryPool::getNextEntityID() const
{
	for (size_t i = 0; i < m_active.size(); i++)
	{
		// if this slot if not active 
		// and it hasn't been recently freed
		if (!m_active[i] 
			&& m_recentlyFreed.find(i) == m_recentlyFreed.end() )
		{
			return i;
		}
	}
	return 0; // fix later this
}

size_t EntityMemoryPool::addEntity(const std::string& tag)
{
	size_t index = getNextEntityID();
	m_active[index] = true;

	/*
	Setting all component's 'has' variables to false by default.
	We use std::apply and fold expression to iterate through all
	tuple components at a given index
	*/
	std::apply([index](auto&&... args) {
		(..., (args[index].has = false));
		}, m_data);

	m_tags[index] = tag;
	return index;
}

size_t EntityMemoryPool::addCopy(size_t entityID)
{
	size_t newEntityID = addEntity(m_tags[entityID]);
	std::apply([entityID, newEntityID](auto&&... args) {
		(..., (args[newEntityID] = args[entityID]));
		}, m_data);
	return newEntityID;
}

bool EntityMemoryPool::isActive(size_t entityID) const
{
	return m_active[entityID];
}

void EntityMemoryPool::removeEntity(size_t entityID)
{
	m_recentlyFreed.insert(entityID);
	m_active[entityID] = false;
}

void EntityMemoryPool::clearRecentFreed()
{
	m_recentlyFreed.clear();
}

