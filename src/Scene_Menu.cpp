#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "SFML/Graphics/Text.hpp"
#include <iostream>
#include "Scene_Editor.h"

void Scene_Menu::init() {



    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "PLAY");
    registerAction(sf::Keyboard::Escape, "QUIT");

    // addFont
    m_game->getAssets()->addFont("mario", MY_PROJECT_PATH"/assets/fonts/mario.ttf");

    sf::Font& menuFont = m_game->getAssets()->getFont("mario");

    m_title = "I DON't KNOW WHAT IS ZELDA";
    int titleSize = 30;
    m_menuText.setString(m_title);
    m_menuText.setFont(menuFont);
    m_menuText.setCharacterSize(titleSize);
    m_menuText.setFillColor(sf::Color::Black);
    m_menuText.setPosition(
        m_game->window().getSize().x / 2.0
        - titleSize * (m_title.length() + 1) / 2.0,
        titleSize * 3
    );
    m_menuStrings.push_back("Level 1");
    m_menuStrings.push_back("Level 2");
    m_menuStrings.push_back("Level 3");
    m_menuStrings.push_back("Level editor");
    m_levelPaths.push_back(MY_PROJECT_PATH"/config/level1.txt");
    m_levelPaths.push_back(MY_PROJECT_PATH"/config/level2.txt");
    m_levelPaths.push_back(MY_PROJECT_PATH"/config/level3.txt");

    m_titleMusic = m_game->getAssets()->getSound("STitleTheme");
    m_titleMusic.play();



    for (int i = 0; i < m_menuStrings.size(); i++) {
        sf::Text text(
            m_menuStrings[i],
            m_game->getAssets()->getFont("mario"), 26
        );
        if (i != m_selectedMenuIndex) {
            text.setFillColor(sf::Color::Black);
        }
        text.setPosition(
            m_game->window().getSize().x / 2.0
            - 26 * (m_menuStrings[i].length() + 1) / 2.0,
            m_menuText.getGlobalBounds().top + 10 + 30 * (i + 1)
        );
        m_menuItems.push_back(text);
    }
}

void Scene_Menu::update() {
    sRender();
}

void Scene_Menu::onEnd() {
    m_game->quit();
}

void Scene_Menu::sDoAction(const Action& action) {
    if (action.type() == "START") {
        if (action.name() == "UP") {
            if (m_selectedMenuIndex > 0) 
            {
                m_selectedMenuIndex--;
            }
            else {
                m_selectedMenuIndex = m_menuStrings.size() - 1;
            }
        }
        else if (action.name() == "DOWN") 
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1)
                % m_menuStrings.size();
        }
        else if (action.name() == "PLAY") 
        {
            m_titleMusic.stop();
            if (m_selectedMenuIndex == 3)
            {
				m_game->changeScene("PLAY",
					std::make_shared<Scene_Editor>(
						m_game, MY_PROJECT_PATH"/config/entities.txt"
					)
				);
                return;
            }
            m_game->changeScene("PLAY",
                    std::make_shared<Scene_Play>(
                        m_game, m_levelPaths[m_selectedMenuIndex]
                    )
                );
        }
        else if (action.name() == "QUIT") {
            onEnd();
        }
    }
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine) : Scene(gameEngine) {
    init();
}

void Scene_Menu::sRender() {


	sf::View view = m_game->window().getDefaultView();
	auto wSize = m_game->window().getSize();
	view.setCenter(wSize.x / 2, wSize.y / 2);
	m_game->window().setView(view);

    // set menu background
    m_game->window().clear(sf::Color(100, 100, 255));

    // draw title
    m_game->window().draw(m_menuText);

    // draw menu items
    for (int i = 0; i < m_menuStrings.size(); i++) {
        if (i != m_selectedMenuIndex) {
            m_menuItems[i].setFillColor(sf::Color::Black);
        }
        else {
            m_menuItems[i].setFillColor(sf::Color::White);
        }

        m_menuItems[i].setPosition(
            m_game->window().getSize().x / 2.0
            - 26 * (m_menuStrings[i].length() + 1) / 2.0,
            m_menuText.getGlobalBounds().top + 10 + 30 * (i + 1)
        );
        m_game->window().draw(m_menuItems[i]);
    }

    // draw help
    sf::Text help("W:UP S:DOWN D: SELECT ESC:BACK/QUIT",
        m_game->getAssets()->getFont("mario"),
        26
    );
    help.setFillColor(sf::Color::Black);
    help.setPosition(
        m_game->window().getSize().x / 2.0
        - 26 * (help.getString().getSize() + 1) / 2.0,
        m_game->window().getSize().y - 30 * 2
    );
    m_game->window().draw(help);

}