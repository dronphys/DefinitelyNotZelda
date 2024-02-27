#include "Scene_Editor.h"
#include <iostream>
#include <fstream>
#include "imgui.h"
#include "imgui-SFML.h"
#include "Scene_Menu.h"
#include "fileSaveDialog.h"

Scene_Editor::Scene_Editor(GameEngine* gameEngine, const std::string& path)
	:Scene(gameEngine), m_tilesPath(path)
{
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::LShift, "SHIFT");
	registerAction(sf::Keyboard::LControl, "LCTRL");
	registerAction(sf::Keyboard::Escape, "CLOSE");
	init(m_tilesPath);
}

void Scene_Editor::spawnNPCs()
{
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
			npc.addComponent<CPathFind>(1);
		}
		npc.addComponent<CDraggable>();
	}
}

void Scene_Editor::loadLevelFromFile(const std::string& levelPath)
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

				for (size_t i = 0; i < config.NPATROL; i++)
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

void Scene_Editor::loadLevel()
{
	// reloading entities with loading new level
	m_entities = EntityManager();

	init(MY_PROJECT_PATH"/config/entities.txt");

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
}


void Scene_Editor::sRender()
{
	m_game->window().clear(sf::Color(100, 100, 255)); 
	drawGrid();
	sUpdateSpritePositions();
	for (auto e : m_entities.getEntities())
	{
		if (e.has<CPrototype>()) { continue; }
		sColorModifier(e);
		m_game->window().draw(e.getComponent<CAnimation>().animation.getSprite());
	}
}

void Scene_Editor::onEnd()
{

	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
	
}

// removes entities from the map that overlap with selected entities
void Scene_Editor::removeOverlappingEntities()
{
	for (auto& sEntity : m_selectedEntities)
	{
		for (auto& e : m_entities.getEntities())
		{	
			if (e== sEntity) { continue; }
			if (e.has<CPrototype>()) { continue; }
			if (m_worldPhysics.IsInside(sEntity.getComponent<CTransform>().pos, e))
			{
				e.destroy();
			}
		}
	}
}

bool Scene_Editor::mouseOnEntity(const Entity& e)
{
	// you can select prototypes
	if (e.has<CPrototype>()) { return false; }
	return m_worldPhysics.IsInside(windowToWorld(m_mPos), e);
}

void Scene_Editor::LMClickHandler()
{
	// if ctrl is pressed, we are selecting entities
	if (m_lctrlPressed && !m_currenlyDragging) 
	{
		for (auto& e : m_entities.getEntities())
		{
			if (mouseOnEntity(e) && e.has<CDraggable>())
			{
				if (std::find(m_selectedEntities.begin(), m_selectedEntities.end(), e) 
					!= m_selectedEntities.end())
				{
					std::erase(m_selectedEntities, e);
					e.getComponent<CColorModifier>().frames = 1;
					return;
				}
				m_selectedEntities.push_back(e);
				e.addComponent<CColorModifier>(sf::Color(40,255,255));
			}
		}
		return;
	}
	//If I clicked on selected entities, I want to drag them
	if (m_selectedEntities.size() > 0 && !m_currenlyDragging)
	{
		for (auto e : m_selectedEntities)
		{
			if (mouseOnEntity(e) && e.has<CDraggable>())
			{
				for (auto e : m_selectedEntities)
				{
				    Vec2f offset = e.getComponent<CTransform>().pos - windowToWorld(m_mPos);
					e.addComponent<CDraggable>(offset);
					e.getComponent<CDraggable>().dragging = true;

				}
				m_currenlyDragging = true;
				return;
			}
		}
	}
	// if shift is pressed we need to copy all entities that are selected
	if (m_shiftPressed && m_currenlyDragging)
	{
		removeOverlappingEntities();
		for (auto e : m_selectedEntities)
		{
			Vec2f eGridPos = pixelToGrid(e);
			Entity newEntity = m_entities.addCopy(e);
			newEntity.getComponent<CTransform>().pos =
				gridToMidPixel(eGridPos.x, eGridPos.y, newEntity);
			newEntity.getComponent<CDraggable>().dragging = false;
			newEntity.getComponent<CDraggable>().offset = Vec2f(0,0);
			newEntity.getComponent<CColorModifier>().frames = 1;
		
		}
	}
	// if shift is not pressed, just place selected entities
	if (!m_shiftPressed && m_currenlyDragging)
	{
		removeOverlappingEntities();
		for (auto e : m_selectedEntities)
		{
			Vec2f eGridPos = pixelToGrid(e);
			e.getComponent<CTransform>().pos =
				gridToMidPixel(eGridPos.x, eGridPos.y, e);
			e.getComponent<CDraggable>().dragging = false;
			e.getComponent<CDraggable>().offset = Vec2f(0, 0);
			e.getComponent<CColorModifier>().frames = 1;
		}
		m_selectedEntities.clear();
		m_currenlyDragging = false;
		return;
	}

	if (!m_currenlyDragging && !m_lctrlPressed)
	{
		for (auto& e : m_entities.getEntities())
		{
			if (e.has<CPrototype>()) { continue; }
			if (mouseOnEntity(e))
			{
				m_selectedEntities.push_back(e);
				m_currenlyDragging = true;
				e.getComponent<CDraggable>().dragging = true;
				return;
			}
		}
	}
	
}

void Scene_Editor::RMClickHandler()
{
	if (m_currenlyDragging)
	{
		for (auto e : m_selectedEntities)
		{
			e.destroy();
		}
		m_selectedEntities.clear();
		m_currenlyDragging = false;
		return;
	}

	for (auto& e : m_entities.getEntities())
	{
		if (m_worldPhysics.IsInside(windowToWorld(m_mPos), e) && e.has<CDraggable>())
		{
			e.destroy();
			return;
		}
	}
}



void Scene_Editor::sDoAction(const Action& action)
{

	if (action.name() == "RIGHT")
	{
		m_camera.getComponent<CInput>().right =
			(action.type() == "START") ? true : false;
	}
	else if (action.name() == "UP")
	{
		m_camera.getComponent<CInput>().up =
			(action.type() == "START") ? true : false;
	}
	else if (action.name() == "LEFT")
	{
		m_camera.getComponent<CInput>().left =
			(action.type() == "START") ? true : false;
	}
	else if (action.name() == "DOWN")
	{
		m_camera.getComponent<CInput>().down =
			(action.type() == "START") ? true : false;
	}

	if (action.name() == "RIGHT_CLICK" && action.type() == "START")
	{
		RMClickHandler();
	}

	if (action.name() == "LEFT_CLICK" && action.type() == "START")
	{
		LMClickHandler();
	}
	if (action.name() == "SHIFT" && action.type() == "START")
	{
		m_shiftPressed = true;
	}
	if (action.name() == "SHIFT" && action.type() == "END")
	{
		m_shiftPressed = false;
	}

	if (action.name() == "LCTRL" && action.type() == "START")
	{
		m_lctrlPressed = true;
	}	
	if (action.name() == "LCTRL" && action.type() == "END")
	{
		m_lctrlPressed = false;
	}

	if (action.name() == "MOUSE_MOVE")
	{
		m_mPos = action.pos();
	}
	else if (action.name() == "CLOSE")
	{
		onEnd();
	}
}


void Scene_Editor::init(const std::string& path)
{
	std::ifstream file;
	file.open(path);
	if (!file.is_open())
		std::cerr << "Couldn't open the file\n";
	std::string type;
	std::string aniName;
	bool blockVision, blockMovement;
	while (file >> type)
	{
		if (type == "Tile")
		{
			file >> aniName >> blockMovement >> blockVision;
			Entity prototyp =  m_entities.addEntity("tile");
			prototyp.addComponent<CPrototype>();
			prototyp.addComponent<CAnimation>(
				m_game->getAssets()->getAnimation(aniName), true);

			prototyp.addComponent<CTransform>(
				Vec2f(0, 0),
				Vec2f(),
				Vec2f(1, 1),
				0);
			if (blockMovement)
			{
				prototyp.addComponent<CBoundingBox>(
					prototyp.getComponent<CAnimation>().animation.getSize()
				);
			}
			if (blockVision)
			{
				prototyp.getComponent<CBoundingBox>().bVision = true;
			}
		}
		if (type == "NPC")
		{
			file >> aniName;
			Entity npcPrototyp = m_entities.addEntity("npc");
			npcPrototyp.addComponent<CPrototype>();
			npcPrototyp.addComponent<CAnimation>(
				m_game->getAssets()->getAnimation(aniName), true);
			npcPrototyp.addComponent<CTransform>(
				Vec2f(0, 0),
				Vec2f(),
				Vec2f(1, 1),
				0);
			npcPrototyp.addComponent<CBoundingBox>(
				npcPrototyp.getComponent<CAnimation>().animation.getSize()
			);
		}
	}
	std::cout << "Loaded prototypes\n";

	m_camera = m_entities.addEntity("camera");
	m_camera.addComponent<CTransform>(Vec2f(width()/2, height()/2), Vec2f(), Vec2f(1, 1), 0);
	m_camera.addComponent<CInput>();
	spawnNPCs();
}

Vec2f Scene_Editor::gridToMidPixel(float gridX,
	float gridY,
	Entity entity,
	int roomX,
	int roomY) const
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


void Scene_Editor::update()
{
	m_entities.update();
	sDragAndDrop();
	sCamera();
	sRender();
	sGUI();
}

void Scene_Editor::sGUI()
{
	ImGui::Begin("Window Tittle");
	//ImGui::Checkbox("Currently Dragging", &m_currenlyDragging);
	//ImGui::Checkbox("CntrlClicked", &m_lctrlPressed);
	if (ImGui::Button("Save level"))
	{
		saveLevel();
	}
	if (ImGui::Button("Load Level"))
	{
		char filePath[MAX_PATH];

		if (OpenFileDialog(filePath, MAX_PATH))
		{
			loadLevelFromFile(filePath);
			loadLevel();
		}
	}
	if (ImGui::BeginTabBar("configTabBar"))
	{
		if (ImGui::BeginTabItem("TileMap"))
		{

			tileMapTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Entity Manager"))
		{
			static ImGuiTreeNodeFlags tflags = ImGuiTreeNodeFlags_Framed;
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
	

	ImGui::End();
}



void Scene_Editor::sUpdateSpritePositions()
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

void Scene_Editor::tileMapTab()
{
	int i = 1;
	for (auto entity : m_entities.getEntities())
	{
		// if entity is not a prototyp, it shouldn't be showed here
		if (!entity.has<CPrototype>()) { continue; }
		
		ImGui::PushID(i);
		sf::Color backColor = sf::Color::Black;
		if (ImGui::ImageButton(entity.getComponent<CAnimation>().animation.getSprite(), 1, backColor))
		{
			auto newEntity = m_entities.addCopy(entity);
			newEntity.addComponent<CDraggable>();
			newEntity.getComponent<CDraggable>().dragging = true;
			newEntity.removeComonent<CPrototype>();

			if (m_currenlyDragging) { m_selectedEntities.clear(); }

			m_selectedEntities.push_back(newEntity);
			//currentlyDragged = newEntity;
			m_currenlyDragging = true;
		}

		if (i % 4 != 0)
			ImGui::SameLine();
		i++;
		ImGui::PopID();
	}
}

void Scene_Editor::sDragAndDrop()
{
	for (auto& e : m_entities.getEntities())
	{
		if (e.has<CDraggable>())
		{
			if (e.getComponent<CDraggable>().dragging)
			{
				e.getComponent<CTransform>().pos = windowToWorld(m_mPos) 
					+ e.getComponent<CDraggable>().offset;
			}
		}
	}
}



Vec2f Scene_Editor::pixelToGrid(Entity e)
{
	const auto& ePos = e.getComponent<CTransform>().pos;
	Vec2f result;
	result.x = std::floor(ePos.x / m_gridSize.x);
	result.y = std::floor(ePos.y / m_gridSize.y);
	return result;
}

Vec2f Scene_Editor::pixelToGrid(Vec2f pos)
{
	Vec2f result;
	result.x = std::floor(pos.x / m_gridSize.x);
	result.y = std::floor(pos.y / m_gridSize.y);
	return result;
}


void Scene_Editor::drawGrid(void)
{
	sf::Text m_gridText;
	m_gridText.setCharacterSize(9);
	m_gridText.setFont(m_game->getAssets()->getFont("Mario"));

	Vec2f cameraPos = m_camera.getComponent<CTransform>().pos;
	
	auto wSize = m_game->window().getSize();


	int j = 0;

	// The total amount of the grid cells that fits into the screen
	int totalGridsY = static_cast<int>(wSize.y / m_gridSize.y);
	int totalGridsX = static_cast<int>(wSize.x / m_gridSize.x);

	Vec2f gridPos = pixelToGrid(cameraPos);

	// Start and End position to generate vertical lines
	float startPointY = ((int)gridPos.y - totalGridsY / 2 + 1) * m_gridSize.y;
	float endPointY = ((int)gridPos.y + totalGridsY / 2 + 1) * m_gridSize.y;

	// loop to generate vertical lines
	for (float currentPointY = startPointY;
		currentPointY < endPointY;
		currentPointY += m_gridSize.y)
	{
		// drawing a little bit longer than needed,
		// so the player doesn't see the edges of the lines
		drawLine(
			Vec2f(cameraPos.x - wSize.x, currentPointY),
			Vec2f(wSize.x + cameraPos.x, currentPointY)
		);
	}

	// Start and End position to generate horizontal lines lines
	float startPointX = ((int)gridPos.x - totalGridsX / 2 + 1) * m_gridSize.x;
	float endPointX = ((int)gridPos.x + totalGridsX / 2 + 1) * m_gridSize.x;

	for (float currentPointX = startPointX;
		currentPointX < endPointX;
		currentPointX += m_gridSize.x)
	{
		drawLine(
			Vec2f(currentPointX, cameraPos.y - wSize.y),
			Vec2f(currentPointX, cameraPos.y + wSize.y)
		);
	}
}

void Scene_Editor::sCamera()
{
	sCameraMovement();
	if (!m_freeCamera)
	{

	}

	// free camera movement
	// camera follows player
	if (m_freeCamera)
	{
		Vec2f cameraPos = m_camera.getComponent<CTransform>().pos;
		auto wSize = m_game->window().getSize();
		sf::View freeView = m_game->window().getView();
		freeView.setCenter(cameraPos.x, cameraPos.y);
		m_game->window().setView(freeView);
	}
}

void Scene_Editor::writeTileToFile(std::ofstream& file, Entity e)
{
	Vec2f ePos = e.getComponent<CTransform>().pos;
	std::string tag = e.tag();
	Vec2f gridPos = pixelToGrid(e);
	auto [roomX, roomY] = entityRoom(e);
	Vec2i wGSize = windowGridSize();
	int gridPosX = (int)gridPos.x - roomX * windowGridSize().x;
	int gridPosY = (int)gridPos.y - roomY * windowGridSize().y;
	std::string aniName = e.getComponent<CAnimation>().animation.getName();
	file << "Tile " << aniName << ' '
		<< roomX << ' ' << roomY << ' '
		<< gridPosX << ' ' << gridPosY << ' ';
	if (e.has<CBoundingBox>()) { file << 1; }
	else { file << 0; }
	file << ' ';
	if (e.getComponent<CBoundingBox>().bVision) { file << 1; }
	else { file << 0; }

	file << '\n';
}

void Scene_Editor::writeNPCToFile(std::ofstream& file, Entity npc)
{
	Vec2f ePos = npc.getComponent<CTransform>().pos;
	std::string tag = npc.tag();
	Vec2f gridPos = pixelToGrid(npc);
	auto [roomX, roomY] = entityRoom(npc);
	Vec2i wGSize = windowGridSize();
	int gridPosX = (int)gridPos.x - roomX * windowGridSize().x;
	int gridPosY = (int)gridPos.y - roomY * windowGridSize().y;
	std::string aniName = npc.getComponent<CAnimation>().animation.getName();
	file << "NPC " << aniName << ' '
		<< roomX << ' ' << roomY << ' '
		<< gridPosX << ' ' << gridPosY << ' ';
	// block movement and vision
	file << 1 << ' ' << 1 << ' ';
	file << npc.addComponent<CHealth>().maxHealth << ' ';
	file << npc.getComponent<CDamage>().dmg << ' ';
	if (npc.has<CPathFind>())
	{
		file << "Follow " << 5;

	}
	if (npc.has<CPatrol>())
	{
		file << "Patrol ";
		file << npc.getComponent<CPatrol>().speed << ' ';	
		file << npc.getComponent<CPatrol>().positions.size();
		for (auto pos : npc.getComponent<CPatrol>().positions)
		{
			auto gridPos = pixelToGrid(pos);
			file << ' ' << gridPos.x << ' ' << gridPos.y;
		}
	}
	file << '\n';
}

void Scene_Editor::saveLevel()
{
	char filePath[MAX_PATH];

	if (SaveFileDialog(filePath, MAX_PATH))
	{

		std::ofstream file(filePath);
		file << "Player 608 352 63 63 5 7\n";
		for (auto e : m_entities.getEntities())
		{
			
			if (e.has<CPrototype>()) { continue; }
			if (e.tag() == "camera") { continue; }

			if (e.tag() == "tile")
			{
				writeTileToFile(file, e);
			}
			if (e.tag() == "npc")
			{
				writeNPCToFile(file, e);
			}
		}
		file.close();
	}
	else {
		std::cout << "Dialog canceled.\n";
	}
}

void Scene_Editor::sColorModifier(Entity e)
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

void Scene_Editor::sCameraMovement()
{
	float cameraSpeed = 8;

	m_camera.getComponent<CTransform>().velocity.x = 0;
	m_camera.getComponent<CTransform>().velocity.y = 0;

	if (m_camera.getComponent<CInput>().up)
	{
		m_camera.getComponent<CTransform>().velocity.y = -cameraSpeed;
	}
	else if (m_camera.getComponent<CInput>().right)
	{
		m_camera.getComponent<CTransform>().velocity.x = cameraSpeed;
	}

	else if (m_camera.getComponent<CInput>().left)
	{
		m_camera.getComponent<CTransform>().velocity.x = -cameraSpeed;
	}

	else if (m_camera.getComponent<CInput>().down)
	{
		m_camera.getComponent<CTransform>().velocity.y = cameraSpeed;
	}


	m_camera.getComponent<CTransform>().pos += m_camera.getComponent<CTransform>().velocity;
}

Vec2i Scene_Editor::entityRoom(Entity e)
{
	Vec2f pPos = e.getComponent<CTransform>().pos;
	auto wSize = m_game->window().getSize();
	int rX = std::floor(pPos.x / static_cast<float>(wSize.x));
	int rY = std::floor(pPos.y / static_cast<float>(wSize.y));
	return { rX, rY };
}

Vec2i Scene_Editor::windowGridSize()
{
	Vec2i result;

	result.x = static_cast<int>(width() / m_gridSize.x);
	result.y = static_cast<int>(height() / m_gridSize.y);
	return result;
}
