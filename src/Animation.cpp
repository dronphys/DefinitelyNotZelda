#include "Animation.h"
#include <iostream>
Animation::Animation(const std::string& name, sf::Texture& t)
	: Animation(name, t, 1, 0) { }



Animation::Animation(const std::string& name, sf::Texture& t,
	size_t frameCount, size_t speed)
	:m_name(name), 
	m_sprite(t), 
	m_frameCount(frameCount), 
	m_speed(speed),
	m_texture(&t)
{
	m_size = Vec2f((float)t.getSize().x / frameCount, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	
	sf::IntRect spriteRect = m_sprite.getTextureRect();
	spriteRect.width /= m_frameCount;
	m_sprite.setTextureRect(spriteRect);
	
}

void Animation::update()
{
	if (m_speed > 0)
	{
		m_currentFrame++;

		size_t currentAnimationFrame = (m_currentFrame / m_speed) % m_frameCount;

		m_sprite.setTextureRect(sf::IntRect(
			m_size.x * currentAnimationFrame,
			0,
			m_size.x,
			m_size.y
		));
	}
}

bool Animation::hasEnded() const
{
	return (m_currentFrame >= m_speed * m_frameCount );
}

const std::string& Animation::getName() const
{
	return m_name;
}

const Vec2f& Animation::getSize() const
{
	return m_size;
}

sf::Texture& Animation::getTexture()
{
	return *m_texture;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}
