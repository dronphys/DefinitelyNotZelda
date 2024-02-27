#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include <functional> // For std::hash
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <queue>
#include "Vec2.h"



namespace pf
{
	class SquareGrid
	{
	private:
		Vec2i bottomRight = Vec2i(0, 0);
		Vec2i topLeft = Vec2i(0,0);
		int width = 0;
		int height = 0;
		
		// [key, value] value shows where is key from
		// by accessing the value by key you are shown where to go
		std::unordered_map<Vec2i, Vec2i> parents;
	public:
		std::unordered_set<Vec2i> walls;
		static std::array<Vec2i, 4> DIRS;
		SquareGrid() = default;
		SquareGrid(Vec2i br, Vec2i tL = Vec2(0,0));

		void addWall(Vec2i p);

		bool inBounds(Vec2i p) const;

		bool isPassable(Vec2i p) const;

		// returns only valid and passable neighbors
		std::vector<Vec2i> neighbors(Vec2i p) const ;

		void printGrid();

		// SEARCH FUNCTION
		

		std::vector<Vec2i> getPath(Vec2i start, Vec2i goal);
	};


	std::unordered_map<Vec2i, Vec2i> 
		breadth_frst_search(const SquareGrid& grid, Vec2i start, 
			Vec2i goal = Vec2i(INT_MAX, INT_MAX));




	
	
}