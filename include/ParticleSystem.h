#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <Vec2.h>

class ParticleSystem
{
	struct Particle
	{
		sf::Vector2f velocity;
		int lifetime = 0;
	};
	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	Vec2f emitterPos;
	float m_size;

	void resetParticle(size_t index);
public:
	void resetParticles(size_t count = 100, float size = 3);
	void draw(sf::RenderWindow& window);
	void init(const Vec2f& position);
	void update();

};