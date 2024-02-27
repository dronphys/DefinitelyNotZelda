#pragma once
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <memory>
#include "Assets.h"
class Scene;



typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine
{
private:
	SceneMap						  m_scenes;
	sf::RenderWindow				  m_window;
	std::shared_ptr<Assets>			  m_assets;
	std::string					      m_currentScene;
	bool						      m_running;
public:
	GameEngine() = default;
	GameEngine(std::string path);
	void			        init(std::string path);
	std::shared_ptr<Scene>  currentScene();
	void			        run();
	void			        update();
	void			        quit();
	void			        changeScene(std::string sceneName,
						    			std::shared_ptr<Scene> scene);
	std::shared_ptr<Assets>	getAssets();
	sf::RenderWindow&       window();
	void			        sUserInput();


};

