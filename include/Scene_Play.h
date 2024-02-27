#pragma once
#include "Scene.h"
#include "Physics.h"
#include "PathFinding.h"
#include <ParticleSystem.h>
//static elements, such as tiles, pipes, etc.
struct LevelPart
{
    std::string type = "";
    std::string aniName = "";
    int rX = 0; // room X can be negative
    int rY = 0; // room Y
    size_t gX = 0; // grid X
    size_t gY = 0; // grid Y
    bool bVision; // if tiles block vision
    bool bMovement; // if tiles block movement
};

struct PlayerConfig
{
    float x, y, bx, by, SPEED, MAXHEALTH;
    std::string WEAPON;
};

struct NPCConfig
{
    std::string TEXTURE;
    int rX, rY, x, y, MAXHEALTH, DMG;
    float SPEED;
    bool bVision, bMovement;
    std::string AI;
    int NPATROL; // number of patrol positions
    std::vector<Vec2f> patrolCoords;
};



class Scene_Play : public Scene
{

protected:
	bool                    m_paused = false;
	bool                    m_drawBoundingBoxes = false;
	bool                    m_drawGrid = false;
    float                   offset = 0;
    sf::Clock               clock;
    size_t                  m_score = 0;
    sf::Text                m_text;
    std::vector <LevelPart> m_level;
    std::vector <NPCConfig> m_NPCConfigs;
    PlayerConfig            m_playerConfig;
    Physics                 m_worldPhysics;
    Entity                  m_player;
    std::string             m_levelPath;
    Vec2f                   m_mPos;
    bool                    m_drawTextures = true;
    const Vec2f             m_gridSize = { 64, 64 };
    sf::Text                m_gridText;
    pf::SquareGrid          m_gridPF;
    sf::View                m_defaultView;
    int                     m_chosenTile = INT_MAX;
    // room - false, free- true
    bool                    m_freeCamera = false;
    bool                    m_drawNPCVision = false;
    void init(const std::string&);
    //Vec2f gridToMidPixel(float, float, std::shared_ptr<Entity>) const;

	Vec2f gridToMidPixel(float gridX, float gridY,
        Entity entity, int roomX = 0, int roomY = 0 ) const;

    Vec2f pixelToGrid(Entity);
    void loadLevelFromFile(const std::string&);
    void loadLevel();
    void spawnPlayer();
    void spawnNPC();

    // Path finding
    void makePFGrid();
    void calcPathToPlayer(Entity e);
    void calcPathToPlayerForAll();
    void drawNPCsGoal();
    // end for path finding

    // vision system for npcs
    void sNPCVision();
    bool NPCSeePlayer(Entity npc);

    void sColorModifier(Entity e);
    void drawHealthBars(Entity e);
    void spawnSword();
    void sMovement();
    void sDamage();
    void sLink();
    void sLifespan();
    void sAI();
    void updateAndDrawParticles();
    // return current player room
    Vec2i currentPlayerRoom();
    // must be called before render
    void sCamera();

    void sCollision();
    void sAnimation();
    void sRender()  override;
    void onEnd()    override;
    void sDoAction(const Action& action);
    void sPlayerStateHandler();
    void changePlayerStateTo(PlayerState s, Vec2f dir);
    void sPlayerCollisions();
    void destroyEndedAnimations();
    void drawBoundingBoxes();
    void drawLevelWalls();
    void drawGrid();
	void spawnAddScore(int score, const Vec2f& position);
	void sUpdateSpritePositions();
    void sScore();
    void sGUI();
    void sDragAndDrop();
public:
    Scene_Play(GameEngine* gameEngine, const std::string& path);
    void update() override;

   
};