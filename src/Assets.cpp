#include "Assets.h"
#include <fstream>
#include <iostream>
#include "Profiler.h"



void Assets::addTexture(
	const std::string& name, const std::string& path)
{
	sf::Texture texture;
	if (!texture.loadFromFile(path))
	{
		std::cerr << "Error loading texture from file: " << path << '\n';
	}
	m_textures[name] = texture;
}

void Assets::addAnimation(const std::string& name, Animation&& animation)
{
	m_animations[name] = std::move(animation);
}

void Assets::addSound(const std::string& name, const std::string& path) {
    PROFILE_FUNCTION();
	sf::SoundBuffer sb;
	if (!sb.loadFromFile(path)) {
		std::cerr << "Could not load sound from the path!" << path << "\n";
		exit(-1);
	}
	std::cout << "Loaded Sound: " << name << " from " << path << std::endl;
	m_soundBuffers[name] = sb;
    //m_sounds.try_emplace(name, sf::Sound(m_soundBuffers.at(name)));
	m_sounds[name] = sf::Sound(m_soundBuffers.at(name));
    m_sounds[name].setVolume(5);
}

sf::Sound& Assets::getSound(const std::string& name) 
{
    return m_sounds[name];
}

void Assets::addFont(const std::string& name, const std::string& path)
{
	sf::Font font;
	if (!font.loadFromFile(path))
	{
		std::cerr << "Error in Loading font: " << path << '\n';
	}
	m_fonts[name] = font;
}

const Animation& Assets::getAnimation(const std::string& name) const
{
    return m_animations.at(name);
}

const sf::Texture& Assets::getTexture(const std::string& name) const
{
	return m_textures.at(name);
}

 sf::Texture& Assets::getTexture(const std::string& name) 
{
	return m_textures.at(name);
}

const sf::Font& Assets::getFont(const std::string& name) const
{
	return m_fonts.at(name);
}

sf::Font& Assets::getFont(std::string&& name) 
{
	return m_fonts.at(name);
}

void Assets::loadFromFile(const std::string& path) {

    PROFILE_FUNCTION();

    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not load config.txt file!\n";
        exit(-1);
    }
    std::string head;
    while (file >> head) 
    {
        if (head == "Font") 
        {
            std::string font_name;
            std::string font_path;
            file >> font_name >> font_path;
            addFont(font_name, font_path);
        }
        else if (head == "Texture") 
        {
            std::string name;
            std::string path;
            file >> name >> path;
            addTexture(name, path);
        }
        else if (head == "Animation") 
        {
            std::string aniName;
            std::string texName;
            int frames, speed;
            file >> aniName >> texName >> frames >> speed;
            sf::Texture& tex = getTexture(texName);
            addAnimation(
                aniName,
                Animation(aniName, tex, frames, speed)
            );
        }
		else if (head == "Sound")
		{
			std::string soundName;
			std::string soundPath;
			file >> soundName >> soundPath;
            addSound(soundName, soundPath);
		}
        else 
        {
            std::cerr << "head to " << head << "\n";
            std::cerr << "The config file format is incorrect!\n";
            exit(-1);
        }
    }
}

