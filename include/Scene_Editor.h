#pragma once
#include "Scene.h"
#include "Physics.h"
#include <windows.h>
#include <commdlg.h>
#include "Scene_Play.h"


class Scene_Editor : public Scene
{
	std::string m_tilesPath;
	Vec2f m_gridSize = Vec2f(64, 64);
	int m_chosenTile = INT_MAX;
	Vec2f m_mPos = Vec2f(0, 0);
	std::vector <NPCConfig> m_NPCConfigs;
	Physics     m_worldPhysics;
	bool		m_freeCamera = true;
	bool		m_shiftPressed = false;
	bool		m_currenlyDragging = false;
	bool 	    m_lctrlPressed = false;
	Entity		m_camera;
	std::vector <LevelPart> m_level;
	std::vector<Entity> m_selectedEntities;

	void sCamera();
	void sRender()  override;
	void onEnd()    override;
	void sDoAction(const Action& action);
	void sDragAndDrop();
	void init(const std::string& path);
	Vec2f gridToMidPixel(float gridX, float gridY,
		Entity entity, int roomX = 0, int roomY = 0) const;

	void LMClickHandler();
	void RMClickHandler();
	void spawnNPCs();
	void sGUI();
	void loadLevelFromFile(const std::string& path);
	void loadLevel();
	void tileMapTab();
	void sUpdateSpritePositions();
	Vec2f pixelToGrid(Entity);
	Vec2f pixelToGrid(Vec2f pos);
	void drawGrid();
	void saveLevel();
	void sColorModifier(Entity e);
	void removeOverlappingEntities();
	bool mouseOnEntity(const Entity& e);
	void sCameraMovement();
	Vec2i entityRoom(Entity e);
	Vec2i windowGridSize();

	void writeTileToFile(std::ofstream& file, Entity e);
	void writeNPCToFile(std::ofstream& file, Entity npc);
public:
	Scene_Editor(GameEngine* gameEngine, const std::string& path = "");
	void update() override;
};
