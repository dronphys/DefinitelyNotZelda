#include "GameEngine.h"
#include "Scene_Play.h"
#include "Scene_Menu.h"
#include <memory>
#include <iostream>
#include "imgui.h"
#include "imgui-SFML.h"

GameEngine::GameEngine(std::string path)
{
	init(path);
}

void GameEngine::init(std::string configPath)
{
	// Loading assets  from path
	m_assets = std::make_shared<Assets>();
	m_assets->loadFromFile(configPath);


	m_window.create(sf::VideoMode(1280,768), "Definitely Not Zelda");
	m_window.setFramerateLimit(60);
	//std::string path = "config/level1.txt";
	changeScene("MENU", std::make_shared<Scene_Menu>(this));

	//changeScene("PLAY", std::make_shared<Scene_Play>(this, path));

	if(!ImGui::SFML::Init(m_window))
	{
		std::cerr << "Couldn't initialize ImGui SFML\n";
	}
 
}

void GameEngine::run()
{
	
	sf::Clock deltaClock;
	while (m_window.isOpen())
	{
		sUserInput();
		ImGui::SFML::Update(m_window, deltaClock.restart());
		update();

		ImGui::SFML::Render(m_window);
		m_window.display();

	}
	ImGui::SFML::Shutdown();
}

void GameEngine::sUserInput()
{
	sf::Event event{};
	while (m_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(m_window, event);

		if (event.type == sf::Event::Closed)
			quit();

		if (event.type == sf::Event::KeyPressed ||
			event.type == sf::Event::KeyReleased) 
		{
			// if the current scene does not have an action associated with
			// this key, skip the event
			if (currentScene()->getActionMap().find(event.key.code)
				== currentScene()->getActionMap().end()) 
			{
				continue;
			}
			// determine start or end action by whether it was key press or release
			const std::string actionType =
				(event.type == sf::Event::KeyPressed) ? "START" : "END";

			// std::cout << actionType << std::endl;
			// look up the action and send the action to the scene
			currentScene()->doAction(
				Action(currentScene()->getActionMap().at(event.key.code)
					,actionType));
			
		}
		// this line ignores further code if the mouse is inside ImGui
		if (ImGui::GetIO().WantCaptureMouse) { continue; }

		if (event.type == sf::Event::MouseButtonPressed ||
			event.type == sf::Event::MouseButtonReleased)
		{
			auto pos = sf::Mouse::getPosition(m_window);
			Vec2f mpos(pos.x, pos.y);
			const std::string actionType =
				(event.type == sf::Event::MouseButtonPressed) ? "START" : "END";
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left :
				currentScene()->doAction(Action("LEFT_CLICK", actionType, mpos));
				break;
			case sf::Mouse::Right:
				currentScene()->doAction(Action("RIGHT_CLICK", actionType, mpos));
				break;
			case sf::Mouse::Middle:
				currentScene()->doAction(Action("MIDDLE_CLICK", actionType, mpos));
				break;
			default: break;
			}
		}
		if (event.type == sf::Event::MouseMoved)
		{
			auto pos = sf::Mouse::getPosition(m_window);
			Vec2f mpos(pos.x, pos.y);
			currentScene()->doAction(Action("MOUSE_MOVE", "START", mpos));
		}
	}

}
void GameEngine::quit()
{
	m_window.close();
	m_running = false;
}

std::shared_ptr<Scene>  GameEngine::currentScene()
{
	return m_scenes.at(m_currentScene);
}

void GameEngine::changeScene(std::string sceneName,
	std::shared_ptr<Scene> scene)
{
	m_scenes[sceneName] = scene;
	m_currentScene = sceneName;
	
}

std::shared_ptr<Assets> GameEngine::getAssets()
{
	return m_assets;
}

void GameEngine::update() 
{
	
	currentScene()->update();
}

sf::RenderWindow& GameEngine::window() 
{
	return m_window;
}

