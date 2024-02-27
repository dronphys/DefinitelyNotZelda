#pragma once
#include <map>
#include <memory>
#include <string>
#include "EntityManager.h"
#include "Action.h"
#include "GameEngine.h"


typedef std::map<int, std::string> ActionMap;

class Scene
{
protected:
	GameEngine* 				m_game;
	EntityManager				m_entities;
	ActionMap					m_actionMap;
	bool						m_paused;
	bool						m_hasEnded;
	size_t						m_currentFrame;

	void setPaused(bool paused);
	virtual  void onEnd() = 0;
	
public:
	
	Scene() = default;
	Scene(GameEngine* gameEngine);
	virtual ~Scene();

	virtual void doAction(const Action& action);
	//void simulate(const size_t frames);
	void registerAction(int inputKey, const std::string& actionName);
	void drawLine(const Vec2f& p1, const Vec2f& p2);

	size_t width() const;
	size_t height() const;
	//size_t currentFrame() const;

	bool hasEnded() const;
	ActionMap& getActionMap();
	


	virtual void update()						 = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender()						 = 0;


	Vec2f windowToWorld(const Vec2f& mWindowPos) const;
};

