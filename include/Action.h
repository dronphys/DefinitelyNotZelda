#pragma once
#include <string>
class Action
{
private:
	std::string m_name = "NONE";
	std::string m_type = "NONE";
	Vec2f        m_pos = Vec2f(0, 0);

public:
	Action() = default;
	Action(const std::string& name, const std::string& type)
		:m_name(name), m_type(type) {}
	Action(const std::string& name, const std::string& type, const Vec2f& pos)
		:m_name(name), m_type(type), m_pos(pos) {}
	const std::string& name() const { return m_name; }
	const std::string& type() const { return m_type; }
	const	   Vec2f&    pos() const { return m_pos;  }
};