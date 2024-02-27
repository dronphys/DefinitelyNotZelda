#include "Physics.h"
#include <cstdlib>

Vec2f Physics::GetOverlap(Entity a, Entity b) const
{
    if (!a.has<CBoundingBox>() || !a.has<CBoundingBox>())
        return Vec2f();

    const Vec2f& aHSize = a.getComponent<CBoundingBox>().halfSize;
    const Vec2f& aPos   = a.getComponent<CTransform>().pos;
    const Vec2f& bHSize = b.getComponent<CBoundingBox>().halfSize;
    const Vec2f& bPos   = b.getComponent<CTransform>().pos;

    Vec2f delta = Vec2f(abs(aPos.x - bPos.x), abs(aPos.y - bPos.y));
    Vec2f overlap;
    overlap.x = aHSize.x + bHSize.x - delta.x;
    overlap.y = aHSize.y + bHSize.y - delta.y;
    return overlap;
    
}

bool Physics::Overlap(Entity a, Entity b) const
{
    Vec2f overlap = GetOverlap(a, b);
    return (overlap.x > 0) && (overlap.y > 0);
}

Vec2f Physics::GetPreviousOverlap(Entity a, Entity b) const
{
    if (!a.has<CBoundingBox>() || !a.has<CBoundingBox>())
        return Vec2f();

    const Vec2f& aHSize = a.getComponent<CBoundingBox>().halfSize;
    const Vec2f& aPos   = a.getComponent<CTransform>().prevPos;
    const Vec2f& bHSize = b.getComponent<CBoundingBox>().halfSize;
    const Vec2f& bPos   = b.getComponent<CTransform>().prevPos;

    Vec2f delta = Vec2f(abs(aPos.x - bPos.x), abs(aPos.y - bPos.y));
    Vec2f overlap;
    overlap.x = aHSize.x + bHSize.x - delta.x;
    overlap.y = aHSize.y + bHSize.y - delta.y;
    return overlap;
    
}

/*This function return position of entity a 
  in respect to entity b if the overlap 
*/
POSITION Physics::GetOverlapPos(Entity a, Entity b) const
{
    const Vec2f& prevOverlap = GetPreviousOverlap(a, b);
    const Vec2f& aPos = a.getComponent<CTransform>().pos;
    const Vec2f& bPos = b.getComponent<CTransform>().pos;

    // collision from right or left
    if (prevOverlap.y > 0)
    {
        if (aPos.x > bPos.x) return POSITION::RIGHT;
        
        return POSITION::LEFT;
    }
    else
    {
        if (aPos.y > bPos.y) return POSITION::DOWN;

        return POSITION::UP;
    }
}

bool Physics::IsInside(const Vec2f& pos, Entity e)
{
	auto ePos = e.getComponent<CTransform>().pos;
	auto size = e.getComponent<CAnimation>().animation.getSize();
	auto scale = e.getComponent<CTransform>().scale;
	size.x *= fabs(scale.x);
	size.y *= fabs(scale.y);

	if (pos.x >= ePos.x - size.x / 2.0f && pos.x <= ePos.x + size.x / 2.0f)
	{
		if (pos.y >= ePos.y - size.y / 2.0f && pos.y <= ePos.y + size.y / 2.0f)
		{
			return true;
		}
	}
	return false;
}

Intersect Physics::LineInIntersect(
    const Vec2f& a,
	const Vec2f& b,
	const Vec2f& c,
	const Vec2f& d
)
{
    Vec2f r = (b - a);
    Vec2f s = (d - c);
    float rxs = Vec2f::cross(r, s);
    Vec2f cma = (c - a);
    float t = Vec2f::cross(cma, s) / rxs;
    float u = Vec2f::cross(cma, r) / rxs;
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        return { true, Vec2f(a.x + t * r.x, a.y + t * r.y) };
    } 
    return { false, Vec2f(0,0) };
}

// checks intersection with 4 lines of the entity rectangle
bool Physics::EntityIntersect(const Vec2f& a, const Vec2f& b, Entity e)
{
    Vec2f ePos = e.getComponent<CTransform>().pos;
    Vec2f eSize = e.getComponent<CAnimation>().animation.getSize();
    // points, up left, up right, bottom left, bottom right
    Vec2f ul = ePos - (eSize / 2.0f);
    Vec2f br = ePos + (eSize / 2);
    Vec2f ur = Vec2f(ePos.x + eSize.x / 2, ePos.y - eSize.y / 2);
    Vec2f bl = Vec2f(ePos.x - eSize.x / 2, ePos.y + eSize.y / 2);
    
	Intersect inter1 = LineInIntersect(a, b, ul, ur);//top
	Intersect inter2 = LineInIntersect(a, b, ul, bl);//left
	Intersect inter3 = LineInIntersect(a, b, bl, br);//bot
	Intersect inter4 = LineInIntersect(a, b, br, ur);//right
    
    return inter1.result || inter2.result ||
        inter3.result || inter4.result;
    
}
