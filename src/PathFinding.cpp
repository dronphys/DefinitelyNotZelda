#include "PathFinding.h"


namespace pf
{
	std::array<Vec2i, 4> SquareGrid::DIRS = {
	Vec2i{1, 0}, Vec2i{-1, 0},
	Vec2i{0, -1}, Vec2i{0, 1}
	};
}



pf::SquareGrid::SquareGrid(Vec2i br, Vec2i tl)
	:bottomRight(br), topLeft(tl) {}

void pf::SquareGrid::addWall(Vec2i p)
{
	walls.insert(p);
}

bool pf::SquareGrid::inBounds(Vec2i p) const
{
	return (topLeft.x <= p.x) && (p.x < bottomRight.x)
		&& (topLeft.y <= p.y) && (p.y < bottomRight.y);
}
bool pf::SquareGrid::isPassable(Vec2i p) const
{
	return (walls.find(p) == walls.end());
}

std::vector<Vec2i> pf::SquareGrid::neighbors(Vec2i p) const
{
	std::vector<Vec2i> result;
	for (auto& dir : DIRS)
	{
		Vec2i neighbor{ p.x + dir.x, p.y + dir.y };
		if (inBounds(neighbor) && isPassable(neighbor))
		{
			result.push_back(neighbor);
		}
	}

	if ((p.x + p.y) % 2 == 0) {
		
		std::reverse(result.begin(), result.end());
	}

	return result;
}

void pf::SquareGrid::printGrid()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			char c = isPassable({ x,y }) ? '.' : '#';
			std::cout << c << ' ';
		}
		std::cout << '\n';
	}
}

namespace pf
{
	std::unordered_map<Vec2i, Vec2i>
		breadth_frst_search(const SquareGrid& grid ,Vec2i start, Vec2i goal)
	{
		// the front where search propagates
		std::queue<Vec2i> frontier;
		frontier.push(start);



		// cleaning parents in case there is something from prev. time
		std::unordered_map<Vec2i, Vec2i> parents;

		parents[start] = start;

		while (!frontier.empty())
		{
			Vec2i current = frontier.front();
			frontier.pop();

			if (current == goal)
			{
				return parents;
			}

			// neighbors returned by neighbors fn
			// are always valid and passable
			for (auto& neighbor : grid.neighbors(current))
			{
				if (parents.find(neighbor) == parents.end())
				{
					frontier.push(neighbor);
					parents[neighbor] = current;
				}
			}
		}
		return parents;
	}

	std::vector<Vec2i> 
		SquareGrid::getPath(Vec2i start, Vec2i goal)
	{
		auto parents = breadth_frst_search(*this, start, goal);
		std::vector<Vec2i> path;
		Vec2i current = goal;
		path.push_back(current);

		while (parents.find(current) != parents.end())
		{
			current = parents.at(current);
			path.push_back(current);
			if (current == start)
			{
				break;
			}
		}
		std::reverse(path.begin(), path.end());
		if (path[0] != start)
		{
			
			return std::vector<Vec2i>();
		}
		return path;
	}
}