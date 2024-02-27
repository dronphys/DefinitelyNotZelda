#pragma once

#include <memory>
#include "Entity.h"
#include "Vec2.h"

struct Intersect {
	bool result;
	Vec2f pos;
};

enum class POSITION
{
    LEFT, 
    RIGHT,
    UP,
    DOWN
};

class Physics
{
public:
    Vec2f GetOverlap(
        Entity a,
        Entity b
    ) const;

    bool Overlap(
        Entity a,
        Entity b) const;

    Vec2f GetPreviousOverlap(
        Entity a,
        Entity b
    ) const;
    //return the position of first argument in respect to the second one
    POSITION GetOverlapPos(
		Entity a,
		Entity b
    ) const;

	bool IsInside(const Vec2f& pos, Entity e);

    // does line a-b intersect line c-d
    // if yes where
	Intersect LineInIntersect(
		const Vec2f& a,
		const Vec2f& b,
		const Vec2f& c,
		const Vec2f& d
	);
    // does the line a-b intersect
    // with entity e
	bool EntityIntersect(
		const Vec2f& a,
		const Vec2f& b,
		Entity e
	);
};
