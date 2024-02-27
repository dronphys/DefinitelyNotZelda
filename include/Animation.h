#pragma once
#include "SFML/Graphics/Sprite.hpp"
#include "Vec2.h"
#include <SFML/Graphics.hpp>

class Animation
{
private:
    sf::Sprite m_sprite;
    size_t m_frameCount = 1; // total number of frames of animation
    size_t m_currentFrame = 1; // the current frame of animation being played
    size_t m_speed = 0; // the speed to play this animation
    Vec2f m_size = { 1, 1 }; // size of the animation frame
    size_t m_animationFrame = 0; // current frame of the animation total m_FrameCount
    std::string m_name = "none";

    // this is needed to possibly change the texture later in the code
    // unfortunately sf::Sprite, only gives access to const Texture and it's impossible
    // to change it later
    sf::Texture* m_texture = nullptr;

public:

    Animation() = default;
    Animation(const std::string& name, sf::Texture& t);
    Animation(
        const std::string& name,
        sf::Texture& t,
        size_t frameCount,
        size_t speed
    );

    void update();
    bool hasEnded() const;
    const std::string& getName() const;
    const Vec2f& getSize() const;
    sf::Texture& getTexture();
    sf::Sprite& getSprite();
};