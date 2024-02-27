#pragma once
#include "SFML\Graphics.hpp"

template <typename T>
class Vec2
{
public:
	T x = 0, y = 0;
	Vec2() = default;
	Vec2(sf::Vector2i vec);
	Vec2(T xin, T yin);
	Vec2 absval() const;
	void normalize();
	float lenght();
	bool operator== (const Vec2& rhs) const;
	Vec2 operator+  (const Vec2& rhs) const;
	Vec2 operator-  (const Vec2& rhs) const;
	Vec2 operator-  (T i)			  const;
	Vec2 operator*  (const Vec2& rhs) const;
	Vec2 operator/  (const T f);
	void operator+= (const Vec2& rhs);
	void operator-= (const Vec2& rhs);
	void operator*= (const Vec2& rhs);
	// value of cross product of this and rhs;
	static float cross(const Vec2& lhs, const Vec2& rhs);
	//friend bool operator!=(const Vec2& lhs, const Vec2& rhs);
	friend bool operator!=(const Vec2<T>& lhs, const Vec2<T>& rhs)
	{
		return lhs.x != rhs.x || lhs.y != rhs.y;
	}
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

template <typename T>
Vec2<T> operator*  (const T x, const Vec2<T>& vec);
template <typename T>
Vec2<T> operator*  (const Vec2<T>& vec, const T x);

template <typename T>
Vec2<T>::Vec2(sf::Vector2i vec)
	:x((T)vec.x), y((T)vec.y) {}

template <typename T>
Vec2<T>::Vec2(T xin, T yin)
	: x(xin), y(yin) {};

template <typename T>
Vec2<T> Vec2<T>::absval() const
{
	return Vec2(static_cast<T>(fabs(x)),
		static_cast<T>(fabs(y)));
}

template <typename T>
void Vec2<T>::normalize()
{
	T lenght = static_cast<T>(sqrtf(x * x + y * y));
	if (lenght < 0.000001f) // tolerence
	{
		x = 0;
		y = 0;
	}
	else
	{
		x = x / lenght;
		y = y / lenght;
	}
}

template <typename T>
float Vec2<T>::lenght()
{
	return fabs(x * x + y * y);
}
template <typename T>
bool Vec2<T>::operator==(const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

template <typename T>
Vec2<T> Vec2<T>::operator+(const Vec2<T>& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}
template <typename T>
Vec2<T> Vec2<T>::operator-(const Vec2<T>& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

template <typename T>
Vec2<T> Vec2<T>::operator-(T i) const
{
	return Vec2(x - i, y - i);
}

template <typename T>
Vec2<T> Vec2<T>::operator*(const Vec2& rhs) const
{
	return Vec2(x * rhs.x, y * rhs.y);
}

template <typename T>
Vec2<T> Vec2<T>::operator/(const T f)
{
	return Vec2(x / f, y / f);
}

template <typename T>
void Vec2<T>::operator+=(const Vec2& rhs)
{
	x += rhs.x; y += rhs.y;
}

template <typename T>
void Vec2<T>::operator-=(const Vec2<T>& rhs)
{
	x -= rhs.x; y -= rhs.y;
}

template <typename T>
void Vec2<T>::operator*=(const Vec2<T>& rhs)
{
	x *= rhs.x; y *= rhs.y;
}

template <typename T>
float Vec2<T>::cross(const Vec2<T>& lhs, const Vec2<T>& rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

//template <typename T>
// bool operator!=(const Vec2<T>& lhs, const Vec2<T>& rhs)
//{
//	return lhs.x != rhs.x || lhs.y != rhs.y;
//}

template <typename T>
Vec2<T> operator*(const T x, const Vec2<T>& vec)
{
	return Vec2(vec.x * x, vec.y * x);
}

template <typename T>
Vec2<T> operator*(const Vec2<T>& vec, const T x)
{
	return Vec2(vec.x * x, vec.y * x);
}

namespace std
{
	template <>
	struct hash<Vec2i>
	{
		std::size_t operator()(const Vec2i& id) const
		{
			// Correct: Directly define the operator() without re-specifying the class name.
			return std::hash<int>()(id.x ^ (id.y << 16));
		}
	};
}