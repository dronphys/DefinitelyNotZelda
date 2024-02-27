#pragma once

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Texture.hpp"
#include <map>
#include <string>
#include "Animation.h"
#include "SFML/Audio/Sound.hpp"
#include <SFML/Audio.hpp>


class Assets
{
    std::map<std::string, sf::Texture>     m_textures;
	std::map<std::string, Animation>       m_animations;
	std::map<std::string, sf::Font>        m_fonts;
	std::map<std::string, sf::Sound>       m_sounds;
    std::map<std::string, sf::SoundBuffer> m_soundBuffers;

public:

    void addTexture(const std::string& name, const std::string& path);
    void addFont(const std::string& name, const std::string& path);
    void addAnimation(const std::string& name, Animation&& animation);
    void addSound(const std::string& name, const std::string& path);
    sf::Sound& getSound(const std::string& name);
    const Animation& getAnimation(const std::string& name) const;
    const sf::Texture& getTexture(const std::string& name) const;
    sf::Texture& getTexture(const std::string& name);
    const sf::Font& getFont(const std::string& name) const;
    sf::Font& getFont(std::string&& name);
    void loadFromFile(const std::string& path);
};