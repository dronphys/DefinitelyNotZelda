#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include <queue>
#include <ParticleSystem.h>


enum class PlayerState {
	ATTACK = 1 << 1,
	STAND = 1 << 2,   
	MOVE = 1 << 3,
};


PlayerState operator|(PlayerState a, PlayerState b);
PlayerState operator&(PlayerState a, PlayerState b);



class Component
{
public:
	bool has = false;
};


class CTransform : public Component
{
public:
	CTransform() = default;
	CTransform(Vec2f pos_, Vec2f velocity_, Vec2f scale_, float angle_)
		:pos(pos_), prevPos(pos), velocity(velocity_), scale(scale_), angle(angle_)
	{}

	CTransform (Vec2f pos_)
		:CTransform(pos_, Vec2f(), Vec2f(), 0) 
	{}
	// direction of entity
	// +-x - right or left
	// +-y up or down
	Vec2f dir { 0, 0 };
	Vec2f pos { 0, 0 };
	Vec2f prevPos;
	Vec2f velocity{ 0, 0 };
	Vec2f scale{ 1, 1 };
	float angle = 0;
};


class CShape : public Component
{
public:
	sf::CircleShape circle;
	CShape() = default;
	CShape(float radius, int points, const sf::Color& fill,
		const sf::Color& outline, float thickness)
		:circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

class CInput : public Component
{
public:
	bool up	      = false;
	bool right    = false;
	bool left     = false;
	bool down     = false;
	bool space    = false;
	bool canJumpHeigher = false;

};
class CDraggable : public Component
{
public:
	Vec2f offset{ 0,0 };
	bool dragging = false;
	CDraggable() = default;
	CDraggable(const Vec2f& off)
		:offset(off) {}
};

class CScore : public Component
{
public:
	int score = 0;
	CScore() = default;
	CScore(int score)
		:score(score){}
};

class CColision : public Component
{
public:
	CColision() = default;
	CColision(float R)
		:radius(R) {}

	float radius = 0;
};

class CLifeSpan : public Component
{
public:
	CLifeSpan() = default;
	CLifeSpan(int tot)
	:remaining(tot), total(tot) {}

	int remaining = 0;
	int total = 0;
};

class CText : public Component
{
public:
	CText() = default;
	sf::Text text;
	CText(std::string str, sf::Font& f, int s) {
		text = sf::Text(str, f, s);
	}
};

class CBoundingBox : public Component
{
public:
	bool bVision = false;
	bool bMovement = true;
	Vec2f size;
	Vec2f halfSize;
	CBoundingBox() = default;
	CBoundingBox(const Vec2f& s)
		: size(s), halfSize(s.x / 2.0, s.y / 2.0) {}
};

class CGravity : public Component
{
public:
	float g = 0;
	CGravity() = default;
	CGravity(float g_): g(g_) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() = default;
	CAnimation(const Animation& animation, bool r)
		: animation(animation), repeat(r) {}
};

class CState : public Component
{
public:
	PlayerState state;
	bool changeAnimate = false;
	CState() = default;
	CState(const PlayerState s) : state(s) {}
};

class CDamage : public Component
{
public:
	int dmg = 1;
	CDamage() = default;
	CDamage(int d) : dmg(d) {}
};

class CHealth : public Component
{
public:
	int maxHealth = 1;
	int currentHealth = 1;
	CHealth() = default;
	CHealth(int h) : maxHealth(h), currentHealth(h) {}
};


// this component should be used to transform the 
// position of the entity that has this component
// in the same way as position of it's host
// example of usage: sword has player link
class Entity;
class CLink : public Component
{
public:
	Entity* hostID;
	Vec2f offset = Vec2f();
	CLink() = default;
	CLink(Entity* ID, Vec2f v = Vec2f() ) 
		: hostID(ID), offset(v) {}
};

class CInvincibility :public Component
{
public:
	int iframes = 1;
	CInvincibility() = default;
	CInvincibility(int f) : iframes(f) {};
};

class CColorModifier :public Component
{
public:
	int frames;
	sf::Color color;
	CColorModifier() = default;
	CColorModifier(sf::Color c, int f = INT_MAX) : color(c), frames(f) {};
};

class CPatrol : public Component
{
public:

	std::vector<Vec2f> positions{};
	size_t currentPosition = 0;
	float speed = 0;
	CPatrol() = default;
	CPatrol(const std::vector<Vec2f>& pos, float s) : positions(pos), speed(s) {}
};

class CPathFind : public Component
{
public:
	std::queue<Vec2f> positions{};
	size_t currentPosition = 0;
	float speed = 0;
	CPathFind() = default;
	CPathFind(const std::deque<Vec2f>& pos, float s) : positions(pos), speed(s) {}
	CPathFind( float s) : speed(s) {}
};

// the components marks the entity as prototype
// To mark that it shouldn'e be used in any systems
class CPrototype : public Component
{
public:
	CPrototype() = default;
};

class CParticleEmitter : public Component
{
	
public:
	ParticleSystem particleSystem;
	CParticleEmitter() = default;
	CParticleEmitter(const Vec2f& pos)
	{
		particleSystem.init(pos);
	}
};