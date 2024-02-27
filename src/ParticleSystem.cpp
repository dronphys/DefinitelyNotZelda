#include "ParticleSystem.h"

void ParticleSystem::resetParticles(size_t count, float size)
{
	m_particles = std::vector<Particle>(count);
	m_vertices = sf::VertexArray(sf::Quads, count * 4);
	m_size = size;

	for (size_t p = 0; p < m_particles.size(); p++)
	{
		resetParticle(p);
	}
}

void ParticleSystem::resetParticle(size_t index)
{

	m_vertices[4 * index + 0].position = sf::Vector2f(emitterPos.x, emitterPos.y);
	m_vertices[4 * index + 1].position = sf::Vector2f(emitterPos.x + m_size, emitterPos.y);
	m_vertices[4 * index + 2].position = sf::Vector2f(emitterPos.x + m_size, emitterPos.y + m_size);
	m_vertices[4 * index + 3].position = sf::Vector2f(emitterPos.x, emitterPos.y + m_size);

	sf::Color c(255, 0, 255, 255);
	m_vertices[4 * index + 0].color = c;
	m_vertices[4 * index + 1].color = c;
	m_vertices[4 * index + 2].color = c;
	m_vertices[4 * index + 3].color = c;

	float rx = ((float )rand() / (RAND_MAX)) * 6 - 3;
	float ry = ((float )rand() / (RAND_MAX)) * 6 - 3;
	m_particles[index].velocity = sf::Vector2f(rx, ry);
	m_particles[index].lifetime = 10 + ((float)rand() / (RAND_MAX)) * 3;

}

void ParticleSystem::draw(sf::RenderWindow& window)
{
	window.draw(m_vertices);
}

void ParticleSystem::init(const Vec2f& position)
{
	emitterPos = position;
	resetParticles();
}

void ParticleSystem::update()
{
	for (size_t i = 0; i < m_particles.size(); i++)
	{
		if (m_particles[i].lifetime <= 0)
		{
			resetParticle(i);
		}
		m_particles[i].lifetime--;
		m_vertices[4 * i + 0].position += m_particles[i].velocity;
		m_vertices[4 * i + 1].position += m_particles[i].velocity;
		m_vertices[4 * i + 2].position += m_particles[i].velocity;
		m_vertices[4 * i + 3].position += m_particles[i].velocity;
	}
}
