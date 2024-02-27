#include "Scene.h"

Scene::Scene(GameEngine* gameEngine)
	:m_game(gameEngine) {}

Scene::~Scene() {}

bool Scene::hasEnded() const
{
	return m_hasEnded;
}

ActionMap& Scene::getActionMap()
{
	return m_actionMap;
}

void Scene::registerAction(int inputKey, const std::string& actionName) {
	m_actionMap[inputKey] = actionName;
}


void Scene::drawLine(const Vec2f& p1, const Vec2f& p2)
{
	sf::Vertex line[] =
	{
		sf::Vertex(sf::Vector2f(p1.x, p1.y)),
		sf::Vertex(sf::Vector2f(p2.x, p2.y))
	};
	
	m_game->window().draw(line,2, sf::Lines);
}

void Scene::doAction(const Action& action)
{
	sDoAction(action);
}

size_t Scene::width() const {
	return m_game->window().getSize().x;
}

size_t Scene::height() const {
	return m_game->window().getSize().y;
}

Vec2f Scene::windowToWorld(const Vec2f& mWindowPos) const
{
	const auto center = m_game->window().getView().getCenter();
	auto wSize = m_game->window().getSize();
	return Vec2f(mWindowPos.x + center.x - wSize.x / 2.0f, mWindowPos.y + center.y - wSize.y / 2.0f);
}