#include "Scene_Play.h"
#include <fstream>
#include <iostream>
#include "imgui.h"
#include "imgui-SFML.h"
#include <algorithm>
#include <Scene_Menu.h>


#define PI 3.145



Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), m_levelPath(levelPath)
{
	// resisting keys
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Space, "SHOOT");
	registerAction(sf::Keyboard::Escape, "CLOSE");

	// loading levels
	init(levelPath);
}


void Scene_Play::init(const std::string& path)
{
	loadLevelFromFile(path);
	loadLevel();
	m_text.setFont(m_game->getAssets()->getFont("Mario"));

	m_defaultView = m_game->window().getView();
}

void Scene_Play::loadLevel()
{
	// reloading entities with loading new level
	m_entities = EntityManager();
	for (auto& part : m_level)
	{
		if (part.type == "Tile")
		{
			if (part.aniName == "TileHeart")
			{
				Entity heart = m_entities.addEntity("heart");
				heart.addComponent<CAnimation>(
					m_game->getAssets()->getAnimation(part.aniName), true);

				heart.addComponent<CTransform>(
					Vec2f(0, 0),
					Vec2f(),
					Vec2f(1, 1),
					0);

				heart.getComponent<CTransform>().pos
					= gridToMidPixel(part.gX, part.gY, heart, part.rX, part.rY);
				heart.addComponent<CDraggable>();
				heart.addComponent<CBoundingBox>(
					heart.getComponent<CAnimation>().animation.getSize()
				);
			}
			else
			{
				auto block = m_entities.addEntity("tile");
				block.addComponent<CAnimation>(
					m_game->getAssets()->getAnimation(part.aniName), true);

				block.addComponent<CTransform>(
					Vec2f(0, 0),
					Vec2f(),
					Vec2f(1, 1),
					0);

				block.getComponent<CTransform>().pos
					= gridToMidPixel(part.gX, part.gY, block, part.rX, part.rY);
				block.addComponent<CDraggable>();

				if (part.bMovement)
				{
					block.addComponent<CBoundingBox>(
						block.getComponent<CAnimation>().animation.getSize()
					);
				}
				if (part.bVision)
				{
					block.getComponent<CBoundingBox>().bVision = true;
				}
			}
		}
	}
	m_entities.update();
	makePFGrid();
	spawnNPC();
	spawnPlayer();
}

// Function returns the position of the middle of the sprite
// for entity e for given gridCoordinate
// *IMPORTANT* This function requirs entity to have animation
Vec2f Scene_Play::gridToMidPixel(float gridX,
	float gridY,
	Entity entity,
	int roomX,
	int roomY
) const
{
	if (!entity.has<CAnimation>())
	{
		std::cerr << "Entity " << entity.tag() << " has no animation\n";
	}
	auto roomSize = m_game->window().getSize();
	Vec2f spriteSize = entity.getComponent<CAnimation>().animation.getSize();
	const Vec2f spriteScale = entity.getComponent<CTransform>().scale;
	spriteSize *= spriteScale;
	const sf::Vector2u windowSize = m_game->window().getSize();
	Vec2f result;
	result.x = (roomX * (int)roomSize.x) +
		gridX * m_gridSize.x + spriteSize.x / 2;

	result.y = (roomY * (int)roomSize.y) + 
		gridY * m_gridSize.y + spriteSize.y / 2;
	return result;
}

Vec2f Scene_Play::pixelToGrid(Entity e)
{
	const auto& ePos = e.getComponent<CTransform>().pos;
	Vec2f result;
	result.x = std::floor(ePos.x / m_gridSize.x);
	result.y = std::floor(ePos.y / m_gridSize.y);
	return result;
}

void Scene_Play::loadLevelFromFile(const std::string& levelPath)
{
	std::ifstream file;
	file.open(levelPath);
	if (!file.is_open())
		std::cerr << "Couldn't open the file\n";

	LevelPart part;
	while (file >> part.type)
	{
		if (part.type == "Tile")
		{
			file >> part.aniName >> part.rX >> part.rY
				>> part.gX >> part.gY
				>> part.bMovement >> part.bVision;
			m_level.push_back(part);
		}
		if (part.type == "Player")
		{
			float GX, GY;
			file >> m_playerConfig.x >> m_playerConfig.y >>
				m_playerConfig.bx >> m_playerConfig.by >>
				m_playerConfig.SPEED >> m_playerConfig.MAXHEALTH;
		}
		if (part.type == "NPC")
		{
			NPCConfig config;
			file >> config.TEXTURE >> config.rX >> config.rY
				>> config.x >> config.y >> config.bMovement
				>> config.bVision >> config.MAXHEALTH >> config.DMG
				>> config.AI >> config.SPEED;
			if (config.AI == "Patrol")
			{
				file >> config.NPATROL;

				for (size_t i = 0; i< config.NPATROL; i++)
				{
					Vec2f point;
					file >> point.x >> point.y;
					config.patrolCoords.push_back(point);
				}
			}

			m_NPCConfigs.push_back(config);
		}
	}
}

void Scene_Play::spawnPlayer()
{
	m_score = 0;
	m_player = m_entities.addEntity("player");
	m_player.addComponent<CAnimation>(
		m_game->getAssets()->getAnimation("LinkStandDown"), true);


	m_player.addComponent<CTransform>(
		gridToMidPixel(5, 5, m_player),
		Vec2f(0.0f, 0.0f),
		Vec2f(1.0, 1.0), 1.0f
	);
	m_player.addComponent<CInput>();
	m_player.addComponent<CBoundingBox>(Vec2f(m_playerConfig.bx, m_playerConfig.by));
	m_player.addComponent<CState>(PlayerState::STAND);
	m_player.getComponent<CTransform>().dir = Vec2f(0, -1);
	m_player.addComponent<CHealth>(4);

}

void Scene_Play::spawnNPC()
{
	// TODO ADD EFFECT OF different rooms
	// TODO add block vision and block movement effects
	// TODO add AI
	for (const auto& config : m_NPCConfigs)
	{
		auto npc = m_entities.addEntity("npc");
		npc.addComponent<CAnimation>(
			m_game->getAssets()->getAnimation(config.TEXTURE), true
		);

		npc.addComponent<CTransform>();
		npc.getComponent<CTransform>().pos =
			gridToMidPixel(config.x, config.y, npc, config.rX, config.rY);

		npc.addComponent<CDamage>(config.DMG);
		npc.addComponent<CHealth>(config.MAXHEALTH);
		npc.addComponent<CBoundingBox>(
			npc.getComponent<CAnimation>().animation.getSize() - 5
		);
		if (config.AI == "Patrol")
		{
			std::vector<Vec2f> patrolCoordsInPixel;
			for (auto& gridPos : config.patrolCoords)
			{
				patrolCoordsInPixel.push_back(gridToMidPixel(gridPos.x, gridPos.y, npc, config.rX, config.rY));
			}
			npc.addComponent<CPatrol>(
				patrolCoordsInPixel, config.SPEED);
		}
		if (config.AI == "Follow")
		{
			npc.addComponent<CPathFind>(config.SPEED);
		}
	}
}

// For path finding
void Scene_Play::makePFGrid()
{
	int nGridX, nGridY;
	nGridX = m_game->window().getSize().x / m_gridSize.x;
	nGridY = m_game->window().getSize().y / m_gridSize.y;
	m_gridPF = pf::SquareGrid({ nGridX, 2*nGridY }, { -nGridX, -nGridY});
	for (auto e : m_entities.getEntities("tile"))
	{
		if (e.tag() == "TileHeart") { continue; }
		m_gridPF.addWall({ (int)pixelToGrid(e).x, (int)pixelToGrid(e).y });
	}	
}

void Scene_Play::calcPathToPlayer(Entity e)
{
	if (!e.has<CPathFind>()) { return; }

	e.getComponent<CPathFind>().currentPosition = 0;

	Vec2f eGridPos = pixelToGrid(e);
	Vec2f playerGridPos = pixelToGrid(m_player);

	Vec2i ePos = { (int)eGridPos.x, (int)eGridPos.y };
	Vec2i pPos = { (int)playerGridPos.x, (int)playerGridPos.y };

	std::vector<Vec2i> path = m_gridPF.getPath(ePos, pPos);


	auto& ePath = e.getComponent<CPathFind>().positions;
	std::queue<Vec2f> empty;
	std::swap(ePath, empty);

	for (auto& point : path)
	{
		ePath.push(gridToMidPixel(point.x, point.y, e));
	}
	// delete the very first element so they don't jiggle on the same spot
	if (!ePath.empty())
		ePath.pop();
}

void Scene_Play::calcPathToPlayerForAll()
{
	for (auto& e : m_entities.getEntities("npc"))
	{
		if (e.has<CPathFind>())
		{
			
			if (NPCSeePlayer(e)){ calcPathToPlayer(e); }
			
		}
	}
}

void Scene_Play::drawNPCsGoal()
{
	for (auto& e : m_entities.getEntities("npc"))
	{
		if (!e.has<CPathFind>()) { continue; }

		if (!e.getComponent<CPathFind>().positions.empty())
		{
			auto goal = e.getComponent<CPathFind>().positions.front();
			drawLine(e.getComponent<CTransform>().pos, goal);
		}

	}
}
//end for path finding

void Scene_Play::sNPCVision()
{
	//TODO Implement check
	if (!m_player.isAlive()) { return; }

	for (auto& npc : m_entities.getEntities("npc"))
	{
		if (!npc.has<CPathFind>()) { continue; }
		// line between player and npc
		Vec2f npcPos = npc.getComponent<CTransform>().pos;
		Vec2f pPos = m_player.getComponent<CTransform>().pos;

		auto seenEntity = m_player;
		float distance = (pPos - npcPos).lenght();

		for (auto& e : m_entities.getEntities())
		{
			if (!e.has<CBoundingBox>() ) { continue; }
			if (e == npc) { continue; }
			if (!e.getComponent<CBoundingBox>().bVision) { continue; }
			if (m_worldPhysics.EntityIntersect(npcPos, pPos, e))
			{
				auto ePos = e.getComponent<CTransform>().pos;
				float distanceToEntity = (npcPos - ePos).lenght();
				if (distanceToEntity < distance)
				{
					distance = distanceToEntity;
					seenEntity = e;
				}
			}
		}

		drawLine(seenEntity.getComponent<CTransform>().pos, npcPos);
	}
}

bool Scene_Play::NPCSeePlayer(Entity npc)
{
	
	if (!m_player.isAlive()) { return false; }

	// line between player and npc
	Vec2f npcPos = npc.getComponent<CTransform>().pos;
	Vec2f pPos = m_player.getComponent<CTransform>().pos;

	auto seenEntity = m_player;
	float distance = (pPos - npcPos).lenght();

	for (auto& e : m_entities.getEntities())
	{
		if (!e.has<CBoundingBox>()) { continue; }
		if (e == npc) { continue; }
		if (!e.getComponent<CBoundingBox>().bVision) { continue; }
		if (m_worldPhysics.EntityIntersect(npcPos, pPos, e))
		{
			auto ePos = e.getComponent<CTransform>().pos;
			float distanceToEntity = (npcPos - ePos).lenght();
			if (distanceToEntity < distance)
			{
				distance = distanceToEntity;
				//TODO Think how to do this
				seenEntity = e;
			}
		}
	}	
	return (seenEntity == m_player);
}

void Scene_Play::sColorModifier(Entity e)
{
	// if there is no color just return from the function
	if (!e.has<CColorModifier>()) { return; } 

	const sf::Color& color = e.getComponent<CColorModifier>().color;
	e.getComponent<CAnimation>().animation.getSprite().setColor(color);

	// if the frames ended of color modification
	// set back to blank white and remove component from the entity
	if (e.getComponent<CColorModifier>().frames-- <= 0)
	{
		e.getComponent<CAnimation>().animation.getSprite().setColor(
			sf::Color(255, 255, 255)
		);
		e.removeComonent<CColorModifier>();
	}
}

void Scene_Play::drawHealthBars(Entity e)
{
	if (!e.has<CHealth>()) { return; }
	const Vec2f& eSize = e.getComponent<CBoundingBox>().size;
	const Vec2f& ePos = e.getComponent<CTransform>().pos;

	int maxHealth = e.getComponent<CHealth>().maxHealth;
	int currentHealth = e.getComponent<CHealth>().currentHealth;

	// I want total size of all health bars be 50, 5
	sf::Vector2f healthBarSize(10, 5);
	sf::RectangleShape healthBar(healthBarSize);
	healthBar.setOrigin(sf::Vector2f(
		healthBarSize.x / 2.0f,
		healthBarSize.y / 2.0f
		));
	
	healthBar.setOutlineThickness(2.0f);
	healthBar.setOutlineColor(sf::Color::Black);
	for (int i = 0; i < maxHealth; i++)
	{
		float xPos = ePos.x - healthBarSize.x / 2.0f * (maxHealth / 2 + 1);
		xPos += i * (healthBarSize.x + 2.0f);
		float yPos = ePos.y - eSize.y / 2.0f - 10;
		healthBar.setPosition(sf::Vector2f(xPos, yPos));
		healthBar.setFillColor(sf::Color(105, 105, 105));
		if (i < currentHealth)
		{
			healthBar.setFillColor(sf::Color::Red);
		}
		
		m_game->window().draw(healthBar);
		
	}

	
	

}

void Scene_Play::spawnSword()
{
	if (!m_entities.getEntities("sword").empty()) { return; }
	const Vec2f& pPos = m_player.getComponent<CTransform>().pos;
	const Vec2f& pDir = m_player.getComponent<CTransform>().dir;

	auto sword = m_entities.addEntity("sword");
	sword.addComponent<CDamage>(1);
	sword.addComponent<CLifeSpan>(10);
	sword.addComponent<CTransform>(Vec2f(), Vec2f(), Vec2f(1, 1), 0);
	Vec2f swordSize = Vec2f(20, 20) + Vec2f(10, 10) * pDir.absval();
	sword.addComponent<CBoundingBox>(swordSize);
	//Vec2f& swordPos = sword.getComponent<CTransform>().pos;
	Vec2f swordOffset = pDir * Vec2f(50, 50);
	//swordPos = pPos + swordOffset;

	sword.addComponent<CLink>(&m_player, swordOffset);
	auto& sound = m_game->getAssets()->getSound("SSwordSlash");
	sound.play();

	sword.addComponent<CParticleEmitter>(pPos + swordOffset);
}

void Scene_Play::sDoAction(const Action& action)
{
	if (!m_player.isAlive()) { return; } // if player is dead quit

	if (action.name() == "RIGHT")
	{
		m_player.getComponent<CInput>().right =
			(action.type() == "START") ? true : false;
	}
	else if (action.name() == "UP")
	{
		m_player.getComponent<CInput>().up =
			(action.type() == "START") ? true : false;
	}
	else if (action.name() == "LEFT")
	{
		m_player.getComponent<CInput>().left =
			(action.type() == "START") ? true : false;
	}
	else if (action.name() == "DOWN")
	{
		m_player.getComponent<CInput>().down =
			(action.type() == "START") ? true : false;
	}
	else if (action.name() == "SHOOT")
	{
		m_player.getComponent<CInput>().space =
			(action.type() == "START") ? true : false;

		if (action.type() == "START")
		{
			spawnSword();
		}
	}
	else if (action.name() == "CLOSE")
	{
		onEnd();
	}
	else if (action.name() == "LEFT_CLICK" && action.type() == "START")
	{
		m_mPos = action.pos();
		for (auto& e : m_entities.getEntities())
		{
			if (m_worldPhysics.IsInside(windowToWorld(m_mPos), e) && e.has<CDraggable>())
			{
				// if im currently dragging this thing
				// and want to release it so it stays in grid
				if (e.getComponent<CDraggable>().dragging)
				{
					Vec2f eGridPos = pixelToGrid(e);
					e.getComponent<CTransform>().pos =
						gridToMidPixel(eGridPos.x, eGridPos.y, e);
				}
				e.getComponent<CDraggable>().dragging = !e.getComponent<CDraggable>().dragging;
			}
		}
	}
	else if (action.name() == "MOUSE_MOVE")
	{
		m_mPos = action.pos();
	}
}

void Scene_Play::sPlayerStateHandler(void)
{
	if (!m_player.isAlive()) { return; } // if player is dead


	// current state
	PlayerState pState = m_player.getComponent<CState>().state;
	Vec2f pDir = m_player.getComponent<CTransform>().dir;

	// if player is currently attacking 
	// we should wait for the animation to finish
	if (pState == PlayerState::ATTACK)
	{
		if (!m_player.getComponent<CAnimation>().animation.hasEnded())
		{
			changePlayerStateTo(pState, pDir); // we don't change state
			return; // we quit this function and don't change anything.
		}
	}

	// if no button was pressed by default player stays
	pState = PlayerState::STAND;

	if (m_player.getComponent<CInput>().right)
	{
		pDir = Vec2f(1, 0);
		pState = PlayerState::MOVE;
	}
	if (m_player.getComponent<CInput>().left)
	{
		pDir = Vec2f(-1, 0);
		pState = PlayerState::MOVE;
	}
	if (m_player.getComponent<CInput>().up)
	{
		pDir = Vec2f(0, -1);
		pState = PlayerState::MOVE;
	}
	if (m_player.getComponent<CInput>().down)
	{
		pDir = Vec2f(0, 1);
		pState = PlayerState::MOVE;
	}
	if (m_player.getComponent<CInput>().space)
	{
		pState = PlayerState::ATTACK;
	}

	changePlayerStateTo(pState, pDir);
}

void Scene_Play::changePlayerStateTo(PlayerState s, Vec2f dir)
{
	PlayerState& pState = m_player.getComponent<CState>().state;
	Vec2f& pDir = m_player.getComponent<CTransform>().dir;
	if (pState != s || (pDir != dir))
	{
		pState = s;
		pDir = dir;
		m_player.getComponent<CState>().changeAnimate = true;
	}

	else
		m_player.getComponent<CState>().changeAnimate = false;
}

void Scene_Play::onEnd()
{	
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Play::update()
{

	if (!m_paused)
	{
		m_currentFrame++;
		m_entities.update();
		sPlayerStateHandler();

		sAnimation();
		sMovement();
		sLifespan();
	}
	if (m_player.getComponent<CTransform>().prevPos
		!= m_player.getComponent<CTransform>().pos)
	{
		calcPathToPlayerForAll();
	}
	
	sDamage();
	sDragAndDrop();
	sCamera();
	sRender();
	if (m_drawNPCVision)
	{
		drawNPCsGoal();
		sNPCVision();
	}

	sCollision();
	sScore();
	sGUI();
	updateAndDrawParticles();
}

void Scene_Play::sLifespan()
{
	//destroyEndedAnimations();

	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CLifeSpan>())
		{
			int totalLifeTime = e.getComponent<CLifeSpan>().total;
			int remaining = e.getComponent<CLifeSpan>().remaining;

			if (e.getComponent<CLifeSpan>().remaining-- <= 0)
			{
				e.destroy();
			}
		}
	}
}

void Scene_Play::sAI()
{
	for (auto& npc : m_entities.getEntities("npc"))
	{
		if (npc.has<CPatrol>())
		{
			const std::vector<Vec2f>& positions = npc.getComponent<CPatrol>().positions;
			size_t& currentPos = npc.getComponent<CPatrol>().currentPosition;
			const Vec2f& ePos = npc.getComponent<CTransform>().pos;
			Vec2f goal = positions[currentPos];
			// tolerance that counts that npc reached goal
			float r = 5; 
			// if npc has reached it's goal point change goal
			// it's necessary to check it here, otherwise
			// dir vector can be 0 and normalizing will break;
			if ((ePos.x - goal.x) * (ePos.x - goal.x)
				+ (ePos.y - goal.y) * (ePos.y - goal.y) < r * r)
			{
				currentPos = (currentPos + 1) % positions.size();
				// setting new goal if reached the prev one
				goal = positions[currentPos];
			}

			Vec2f& eVel = npc.getComponent<CTransform>().velocity;
			float speed = npc.getComponent<CPatrol>().speed;
			Vec2f dir = goal - ePos; dir.normalize();
			eVel = speed * dir;

		}
		else if (npc.has<CPathFind>() 
			&& !npc.getComponent<CPathFind>().positions.empty() )
		{
			auto& positions = npc.getComponent<CPathFind>().positions;

			size_t& currentPos = npc.getComponent<CPatrol>().currentPosition;
			const Vec2f& ePos = npc.getComponent<CTransform>().pos;
			Vec2f goal = positions.front();
			float r = 5;

			// if npc has reached it's goal point change goal
			// it's necessary to check it here, otherwise
			// dir vector can be 0 and normalizing will break;
			if ((ePos.x - goal.x) * (ePos.x - goal.x)
				+ (ePos.y - goal.y) * (ePos.y - goal.y) < r * r)
			{
				positions.pop();
			}

			Vec2f& eVel = npc.getComponent<CTransform>().velocity;
			float speed = npc.getComponent<CPathFind>().speed;
			Vec2f dir = goal - ePos; dir.normalize();
			eVel = speed * dir;
		}
		else
		{
			npc.getComponent<CTransform>().velocity = Vec2f(0,0);
		}
	}
}

void Scene_Play::updateAndDrawParticles()
{
	for (auto e : m_entities.getEntities())
	{
		if (e.has<CParticleEmitter>())
		{
			e.getComponent<CParticleEmitter>().particleSystem.update();
			e.getComponent<CParticleEmitter>().particleSystem.draw(m_game->window());
		}
	}
}

void Scene_Play::sPlayerCollisions()
{
	if (!m_player.isAlive()) { return; } // if player is dead

	// collisions with tiles
	for (auto& e : m_entities.getEntities("tile"))
	{
		const Vec2f& overlap = m_worldPhysics.GetOverlap(m_player, e);
		std::string texName;
		if (overlap.x > 0 && overlap.y > 0) // if overlapping with tile
		{
			POSITION pColidePos = m_worldPhysics.GetOverlapPos(m_player, e);
			Vec2f prevOverlap = m_worldPhysics.GetPreviousOverlap(m_player, e);
			Vec2f eSize = e.getComponent<CBoundingBox>().size;
			switch (pColidePos)
			{
			case POSITION::RIGHT:
				
				m_player.getComponent<CTransform>().pos.x += overlap.x;
				break;

			case POSITION::LEFT:
				m_player.getComponent<CTransform>().pos.x -= overlap.x;
				break;

			case POSITION::DOWN:
				m_player.getComponent<CTransform>().pos.y += overlap.y;
				break;

			case POSITION::UP:
				m_player.getComponent<CTransform>().pos.y -= overlap.y;
				break;
			}
		}
	}
}

void Scene_Play::destroyEndedAnimations(void)
{
	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CAnimation>())
		{
			if (!e.getComponent<CAnimation>().repeat &&
				e.getComponent<CAnimation>().animation.hasEnded())
			{
				e.destroy();
			}
		}
	}
}


void Scene_Play::drawGrid(void)
{
	sf::Text m_gridText;
	m_gridText.setCharacterSize(9);
	m_gridText.setFont(m_game->getAssets()->getFont("Mario"));


	const Vec2f& pPos = m_player.getComponent<CTransform>().pos;
	auto wSize = m_game->window().getSize();


	int j = 0;

	// The total amount of the grid cells that fits into the screen
	int totalGridsY = static_cast<int>(wSize.y / m_gridSize.y);
	int totalGridsX = static_cast<int>(wSize.x / m_gridSize.x);

	const Vec2f& pGridPos = pixelToGrid(m_player);

	// Start and End position to generate vertical lines
	float startPointY = ((int)pGridPos.y - totalGridsY / 2 + 1) * m_gridSize.y;
	float endPointY = ((int)pGridPos.y + totalGridsY / 2 + 1) * m_gridSize.y;

	// loop to generate vertical lines
	for (float currentPointY = startPointY;
	     currentPointY < endPointY;
	     currentPointY += m_gridSize.y)
	{
		// drawing a little bit longer than needed,
		// so the player doesn't see the edges of the lines
		drawLine(
			Vec2f(pPos.x - wSize.x, currentPointY),
			Vec2f(wSize.x + pPos.x, currentPointY)
		);
	}

	// Start and End position to generate horizontal lines lines
	float startPointX = ((int)pGridPos.x - totalGridsX / 2 + 1) * m_gridSize.x;
	float endPointX = ((int)pGridPos.x + totalGridsX / 2 + 1) * m_gridSize.x;

	for (float currentPointX = startPointX;
	     currentPointX < endPointX;
	     currentPointX += m_gridSize.x)
	{
		drawLine(
			Vec2f(currentPointX, pPos.y - wSize.y),
			Vec2f(currentPointX, pPos.y + wSize.y)
		);
	}
}

void Scene_Play::drawBoundingBoxes(void)
{
	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CBoundingBox>())
		{
			const Vec2f& eSize = e.getComponent<CBoundingBox>().size;
			const Vec2f& ePos = e.getComponent<CTransform>().pos;
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(eSize.x, eSize.y));
			rect.setOrigin(sf::Vector2f(eSize.x / 2, eSize.y / 2));
			rect.setPosition(sf::Vector2f(ePos.x, ePos.y));
			rect.setFillColor(sf::Color(0, 0, 0, 0));
			rect.setOutlineColor(sf::Color::Red);
			rect.setOutlineThickness(1);
			m_game->window().draw(rect);
		}
	}
}


void Scene_Play::drawLevelWalls()
{
	auto tile  = m_entities.getEntities("tile")[0];
	for (auto wall : m_gridPF.walls)
	{
		Vec2f tilePos = gridToMidPixel(wall.x, wall.y, tile);
		Vec2f tileSize = tile.getComponent<CBoundingBox>().size;
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(tileSize.x, tileSize.y));
		rect.setOrigin(sf::Vector2f(tileSize.x / 2, tileSize.y / 2));
		rect.setPosition(sf::Vector2f(tilePos.x, tilePos.y));
		rect.setFillColor(sf::Color(0, 0, 0, 0));
		rect.setOutlineColor(sf::Color::Green);
		rect.setOutlineThickness(1);
		m_game->window().draw(rect);
	}
}


void Scene_Play::sCollision()
{
	// First check collisions of player with tiles
	sPlayerCollisions();

	for (auto& npc : m_entities.getEntities("npc"))
	{
		for (auto& e : m_entities.getEntities())
		{

			if (!e.has<CBoundingBox>() || npc==e) {continue;} 
			if (e.tag()=="sword" || e.tag() == "heart") { continue; }
			const Vec2f& overlap = m_worldPhysics.GetOverlap(npc, e);
			std::string texName;
			if (overlap.x > 0 && overlap.y > 0) // if overlapping with tile
			{
				POSITION pColidePos = m_worldPhysics.GetOverlapPos(npc, e);

				switch (pColidePos)
				{
				case POSITION::RIGHT:
					npc.getComponent<CTransform>().pos.x += overlap.x;
					break;

				case POSITION::LEFT:
					npc.getComponent<CTransform>().pos.x -= overlap.x;
					break;

				case POSITION::DOWN:
					npc.getComponent<CTransform>().pos.y += overlap.y;
					break;

				case POSITION::UP:
					npc.getComponent<CTransform>().pos.y -= overlap.y;
					break;
				}
			}
		}
	}





	for (auto& heart : m_entities.getEntities("heart"))
	{
		if (m_worldPhysics.Overlap(heart, m_player))
		{
			m_player.getComponent<CHealth>().currentHealth
				= m_player.getComponent<CHealth>().maxHealth;
			heart.destroy();
			auto& sound = m_game->getAssets()->getSound("SLinkPickupHeart");
			sound.play();
			break;
			
		}
	}

	// restoring npc hp if overlaping with heart
	for (auto& npc : m_entities.getEntities("npc"))
	{
		for (auto& heart : m_entities.getEntities("heart"))
		{
			if (m_worldPhysics.Overlap(heart, npc))
			{
				npc.getComponent<CHealth>().currentHealth
					= npc.getComponent<CHealth>().maxHealth;
				heart.destroy();
				break;
			}
		}
	}
	
}

void Scene_Play::sAnimation()
{
	if (!m_player.isAlive()) { return; }

	if (m_player.getComponent<CState>().changeAnimate)
	{
		const PlayerState pState = m_player.getComponent<CState>().state;
		std::string aniName;

		const Vec2f& pDir = m_player.getComponent<CTransform>().dir;

		if (pState == PlayerState::MOVE)
		{
			if (pDir.x != 0)
			{
				aniName = "LinkMoveRight";
			}
			if (pDir.y < 0)
			{
				aniName = "LinkMoveUp";
			}
			if (pDir.y > 0)
			{
				aniName = "LinkMoveDown";
			}
			m_player.addComponent<CAnimation>(
				m_game->getAssets()->getAnimation(aniName), true);
		}

		if (pState == PlayerState::STAND)
		{
			if (pDir.x != 0)
			{
				aniName = "LinkStandRight";
			}
			if (pDir.y < 0)
			{
				aniName = "LinkStandUp";
			}
			if (pDir.y > 0)
			{
				aniName = "LinkStandDown";
			}

			m_player.addComponent<CAnimation>(
				m_game->getAssets()->getAnimation(aniName), true);
		}
		if (pState == PlayerState::ATTACK)
		{
			if (static_cast<int>(pDir.x) != 0)
			{
				aniName = "LinkAtkRight";
			}
			if (pDir.y < 0)
			{
				aniName = "LinkAtkUp";
			}
			if (pDir.y > 0)
			{
				aniName = "LinkAtkDown";
			}
			m_player.addComponent<CAnimation>(
				m_game->getAssets()->getAnimation(aniName), false);
		}
	}

	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CAnimation>())
		{
			e.getComponent<CAnimation>().animation.update();
		}
	}
}

void Scene_Play::spawnAddScore(int score, const Vec2f& position)
{
	//std::cout << "dasdsad";
	//sf::Font font = m_game->getAssets()->getFont("Mario");
	auto addScore = m_entities.addEntity("text");
	addScore.addComponent<CTransform>(position, Vec2f(0, 0), Vec2f(1, 1), 0);
	addScore.addComponent<CText>("+" + std::to_string(score),
	                              m_game->getAssets()->getFont("Mario"), 15);
	addScore.addComponent<CLifeSpan>(45);
	addScore.getComponent<CText>().text.setColor(sf::Color(255, 255, 255));
}


Vec2i Scene_Play::currentPlayerRoom()
{
	Vec2f pPos = m_player.getComponent<CTransform>().pos;
	auto wSize = m_game->window().getSize();
	int rX = std::floor(pPos.x / static_cast<float>(wSize.x));
	int rY = std::floor(pPos.y / static_cast<float>(wSize.y));
	return { rX,rY };
}

void Scene_Play::sCamera()
{
	if (!m_freeCamera)
	{
		//Room locked camera
		auto wSize = m_game->window().getSize();
		auto [rX, rY] = currentPlayerRoom();
		sf::View roomView = m_defaultView;
		roomView.setCenter( wSize.x * (1.0f/2.0f + 1.0f* rX ), 
			wSize.y * (1.0f / 2.0f + 1.0f * rY));
		m_game->window().setView(roomView);
	}

	// free camera movement
	// camera follows player
	if (m_freeCamera)
	{
		auto pPos = m_player.getComponent<CTransform>().pos;
		auto wSize = m_game->window().getSize();
		sf::View freeView = m_defaultView;
		freeView.setCenter(pPos.x, pPos.y);
		m_game->window().setView(freeView);
	}
}

void Scene_Play::sRender()
{
	if (!m_player.isAlive()) { return; } // if player is dead

	m_game->window().clear(sf::Color(100, 100, 255)); // clearing the window

	sUpdateSpritePositions();




	auto& pPos = m_player.getComponent<CTransform>().pos;
	float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);


	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CText>())
		{
			m_game->window().draw(e.getComponent<CText>().text);
		}
		if (e.has<CAnimation>())
		{
			// modify entity color if needed
			drawHealthBars(e);
			sColorModifier(e);
			m_game->window().draw(e.getComponent<CAnimation>().animation.getSprite());
		}
	}



	if (m_drawGrid)
	{
		drawGrid();
	}

	if (m_drawBoundingBoxes)
	{
		//drawLevelWalls();
		drawBoundingBoxes();
	}
}

// updating positions of the sprites from CTransform component before rendering
void Scene_Play::sUpdateSpritePositions()
{
	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CTransform>())
		{
			const Vec2f& pos = e.getComponent<CTransform>().pos;
			const Vec2f& scale = e.getComponent<CTransform>().scale;
			if (e.has<CText>())
			{
				e.getComponent<CText>().text.setPosition(pos.x, pos.y);
			}
			if (e.has<CAnimation>())
			{
				e.getComponent<CAnimation>().animation.getSprite().setPosition(pos.x, pos.y);
				e.getComponent<CAnimation>().animation.getSprite().setScale(scale.x, scale.y);
			}
		}
	}
}

void Scene_Play::sScore()
{
	for (auto& e : m_entities.getEntities())
	{
		if (!e.isAlive())
		{
			if (e.has<CScore>())
			{
				auto score = e.getComponent<CScore>().score;
				spawnAddScore(score, e.getComponent<CTransform>().pos);
				m_score += score;
			}
		}
	}
}

void Scene_Play::sMovement()
{
	if (!m_player.isAlive()) { return; }
	float playerSpeed = 5;

	m_player.getComponent<CTransform>().velocity.x = 0;
	m_player.getComponent<CTransform>().velocity.y = 0;

	// small initial jump after touching the key
	if (m_player.getComponent<CInput>().up)
	{
		m_player.getComponent<CTransform>().velocity.y = -playerSpeed;
	}
	else if (m_player.getComponent<CInput>().right)
	{
		m_player.getComponent<CTransform>().velocity.x = playerSpeed;
		m_player.getComponent<CTransform>().scale.x = 1;
	}

	else if (m_player.getComponent<CInput>().left)
	{
		m_player.getComponent<CTransform>().velocity.x = -playerSpeed;
		m_player.getComponent<CTransform>().scale.x = -1;
	}

	else if (m_player.getComponent<CInput>().down)
	{
		m_player.getComponent<CTransform>().velocity.y = playerSpeed;
	}

	if (m_player.getComponent<CInput>().right && m_player.getComponent<CInput>().left)
	{
		m_player.getComponent<CTransform>().velocity.x = 0.0f;
	}

	if (m_player.getComponent<CInput>().up && m_player.getComponent<CInput>().down)
	{
		m_player.getComponent<CTransform>().velocity.y = 0.0f;
	}
	if (m_player.getComponent<CInput>().down && m_player.getComponent<CInput>().right)
	{
		m_player.getComponent<CTransform>().velocity.x = 0;
		m_player.getComponent<CTransform>().velocity.y = playerSpeed;
	}
	if (m_player.getComponent<CInput>().down && m_player.getComponent<CInput>().left)
	{
		m_player.getComponent<CTransform>().velocity.x = 0;
		m_player.getComponent<CTransform>().velocity.y = playerSpeed;
	}

	sAI();

	// update position of entities that are linked to other entities
	sLink();
	// update entity positions
	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CTransform>())
		{
			e.getComponent<CTransform>().prevPos = e.getComponent<CTransform>().pos;
			e.getComponent<CTransform>().pos += e.getComponent<CTransform>().velocity;
		}
	}
	
}

void Scene_Play::sDamage()
{
	if (!m_player.isAlive()) { return; }

	// checking collision of sword with enemies
	// originally it happens only once since there can be only one sword
	for (auto& sword : m_entities.getEntities("sword"))
	{
		// if dmg component was removed from the sword already
		if (!sword.has<CDamage>()) { continue; }

		for (auto& npc : m_entities.getEntities("npc"))
		{
			// sword hit npc
			if (m_worldPhysics.Overlap(sword, npc))
			{
				npc.getComponent<CHealth>().currentHealth -=
					sword.getComponent<CDamage>().dmg;
				sword.removeComonent<CDamage>();
				auto& sound = m_game->getAssets()->getSound("SEnemyDamaged");
				sound.play();
				npc.addComponent<CColorModifier>(
					sf::Color(255, 100, 100, 255), 10);

				if (npc.getComponent<CHealth>().currentHealth <= 0)
				{
					auto& sound = m_game->getAssets()->getSound("SEnemyDied");
					sound.play();
					npc.destroy();
				}
			}
		}
	}


	// count iframes of invincibility
	if (m_player.has<CInvincibility>())
	{
		auto& iframes = m_player.getComponent<CInvincibility>().iframes;
		iframes--;
		if (iframes <= 0)
		{
			m_player.removeComonent<CInvincibility>();
		}
	}

	//TODO implement it later
	//checking collisions of enemies with player
	for (const auto& npc : m_entities.getEntities("npc"))
	{
		
		if (m_worldPhysics.Overlap(npc, m_player))
		{
			// if player has no invincibility
			// can be attacked and take damage
			if (!m_player.has<CInvincibility>())
			{
				m_player.getComponent<CHealth>().currentHealth--;
				m_player.addComponent<CInvincibility>(40);
				auto& sound = m_game->getAssets()->getSound("SLinkDamaged");
				sound.play();
				m_player.addComponent<CColorModifier>(
					sf::Color(255, 255, 255, 150), 40);
			}

			if (m_player.getComponent<CHealth>().currentHealth <= 0)
			{
				auto& sound = m_game->getAssets()->getSound("SLinkDied");
				sound.play();
				onEnd();
			}
		}
	}
}

void Scene_Play::sLink()
{
	//TODO DO it correctly somehow
	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CLink>())
		{
			e.getComponent<CTransform>().pos =
				e.getComponent<CLink>().hostID->getComponent<CTransform>().pos
				+ e.getComponent<CLink>().offset;
		}
	}
}

void Scene_Play::sGUI()
{
	ImGui::Begin("Window Tittle");


	if (ImGui::BeginTabBar("configTabBar"))
	{
		if (ImGui::BeginTabItem("Systems"))
		{
			ImGui::Checkbox("Paused", &m_paused);
			ImGui::Checkbox("BoundingBox", &m_drawBoundingBoxes);
			ImGui::Checkbox("Draw NPC vision", &m_drawNPCVision);
			ImGui::Checkbox("Grid", &m_drawGrid);
			ImGui::Checkbox("FreeCamera", &m_freeCamera);
			if (ImGui::Button("Add path to all"))
			{
				calcPathToPlayerForAll();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Entity Manager"))
		{
			static ImGuiTreeNodeFlags tflags = ImGuiTreeNodeFlags_Framed;

			if (ImGui::TreeNodeEx("Entities by tag", tflags))
			{
				for (auto& [tag, eVec] : m_entities.getEntityMap())
				{
					if (ImGui::TreeNodeEx(tag.c_str(), tflags))
					{
						for (auto& e : eVec)
						{
							std::string btnLabel = "D##" + std::to_string(e.id());
							if (ImGui::SmallButton(btnLabel.c_str()))
							{
								e.destroy();
							}
							ImGui::SameLine();
							ImGui::Text("%s", std::to_string(e.id()).c_str());
							ImGui::SameLine();
							ImGui::Text("%s", e.tag().c_str());
							ImGui::SameLine();
							ImGui::Text("(%.f, %.f)",
							            e.getComponent<CTransform>().pos.x,
							            e.getComponent<CTransform>().pos.y);
						}

						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}


			// ALl entities
			if (ImGui::TreeNodeEx("All Entities", tflags))
			{
				for (auto& e : m_entities.getEntities())
				{
					std::string btnLabel = "D##" + std::to_string(e.id());
					if (ImGui::SmallButton(btnLabel.c_str()))
					{
						e.destroy();
					}
					ImGui::SameLine();
					ImGui::Text("%s", std::to_string(e.id()).c_str());
					ImGui::SameLine();
					ImGui::Text("%s", e.tag().c_str());
					ImGui::SameLine();
					ImGui::Text("(%.f, %.f)",
					            e.getComponent<CTransform>().pos.x,
					            e.getComponent<CTransform>().pos.y);
				}

				ImGui::TreePop();
			}
			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}

	if (!m_player.isAlive())
	{
		//m_player.reset();
		//m_score = 0;
	}
	
	//ImGui::Image(m_player.getComponent<CAnimation>().animation.getSprite()
	//	,sf::Color(255,255,255)
	//	,sf::Color(255, 0, 0));


	ImGui::End();
}

void Scene_Play::sDragAndDrop()
{
	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CDraggable>())
		{
			if (e.getComponent<CDraggable>().dragging)
			{
				e.getComponent<CTransform>().pos = windowToWorld(m_mPos);
			}
		}
	}
}
