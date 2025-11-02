#include "stdafx.h"

#include "Game_Map.hpp"
#include "MapLoader.hpp"
#include "Game_BG.hpp"
Game_BG Map_bg;

Game_Map::Game_Map()
{
	Block::LoadTextures(); // load all textures once
}

Game_Map::~Game_Map()
{
}

bool Game_Map::loadStageFromFile(const FilePath& path,const int stage)
{
	Array<int> mapData;
	int width, height;

	if (!MapLoader::LoadMap(path, mapData, width, height))
		return false;

	m_width = width;
	m_height = height;

	const Size screenSize = Scene::Size();
	//m_chipWidth = static_cast<double>(screenSize.x) / m_width;
	//m_chipHeight = static_cast<double>(screenSize.y) / m_height;

	setCurrentStage(stage);
	//map height to screen and let width scroll
	m_chipHeight = 100.0f;
	m_chipWidth  = 100.0f;


	m_blocks.clear();
	m_blocks.reserve(width * height);

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int type = mapData[y * m_width + x];

			Block block;
			block.SetBlock(Vec2(x * m_chipWidth, y * m_chipHeight),
				  Vec2(m_chipWidth, m_chipHeight),
				  type);
			m_blocks << block;
		}
	}

	/*const Size worldPx{
		static_cast<int>(m_width * m_chipWidth),
		static_cast<int>(m_height * m_chipHeight)
	};*/
	const Size worldPx{
		static_cast<int>(m_width * m_chipWidth),
		static_cast<int>(m_height * m_chipHeight)
	};

	Map_bg.setSize(Scene::Size());
	Map_bg.setScrollSpeed(Vec2{ 2, 0 }); // 需要滚动就 >0；纯静态背景可设 (0,0)
	Map_bg.setLoop(false);               // 需要滚动改为 true
	Map_bg.resetOffset();

	const FilePath bgPath = U"example/Map/map" + Format(m_currentStage) + U".png";
	Map_bg.setAsset(bgPath, Color{ 24, 40, 56 });
	Map_bg.setMode(BgMode::WorldLocked);   // ✅ 跟方块一起动
	Map_bg.setWorldSize(worldPx);
	Map_bg.setLoop(false);

	return true;
}

void Game_Map::loadNextStage()
{
	m_currentStage++;

	FilePath nextPath = U"example/Map/stage" + Format(m_currentStage) + U".txt";

	if (!FileSystem::Exists(nextPath))
	{
		Print << U"🎉 All stages cleared!";
		return;
	}

	// Reset camera and load the next stage
	m_cameraPos = Vec2{ 0, 0 };

	if (loadStageFromFile(nextPath, m_currentStage))
	{
		Print << U"✅ Loaded next stage: " << nextPath;
	}
	else
	{
		Print << U"❌ Failed to load next stage: " << nextPath;
	}


}

void Game_Map::update()
{
	for (auto& block : m_blocks)
	{
		block.UpdateBlock();

	}
	Map_bg.syncWithCamera(m_cameraPos);
	Map_bg.update();
	
}

void Game_Map::draw() const
{
	Map_bg.draw();
	for (const auto& block : m_blocks)
	{
		if (!block.IsUsed()) continue;

		Vec2 drawPos = block.GetPos() - m_cameraPos;
		const Vec2 size = Vec2(m_chipWidth, m_chipHeight);

		switch (block.getType())
		{
		case BLOCK_EMPTY:
			// nothing
			break;

		case BLOCK_SOLID:
			TextureAsset(U"Block").resized(size).draw(drawPos);
			break;

		case BLOCK_GOAL:
			RectF(drawPos, size).draw(ColorF(0.8, 0.2, 0.2));
			break;
		default:
			break;
		}
	}
}

void Game_Map::updateCamera(const Vec2& playerPos)
{
	Vec2 screenSize = Scene::Size();
	const double mapWidthPx = m_width * m_chipWidth;
	const double mapHeightPx = m_height * m_chipHeight;

	Vec2 desiredCameraPos = playerPos - screenSize / 2;

	double maxX = m_width * m_chipWidth - screenSize.x;
	double maxY = m_height * m_chipHeight - screenSize.y;

	desiredCameraPos.x = Clamp(desiredCameraPos.x, 0.0, Max(0.0, maxX));
	desiredCameraPos.y = Clamp(desiredCameraPos.y, 0.0, Max(0.0, maxY));

	m_cameraPos = desiredCameraPos;
}


RectF Game_Map::worldBounds() const {
	const double W = m_width * m_chipWidth;
	const double H = m_height * m_chipHeight;
	return RectF{ 0, 0, W, H };
}

bool Game_Map::CheckCollision(const RectF& rect)
{
	const RectF bounds = worldBounds();

	if (!bounds.contains(rect)) {
		return true;
	}


	for (const auto& block : m_blocks)
	{
		switch (block.getType())
		{
		case BLOCK_EMPTY:
			
		break;
		case BLOCK_SOLID:
			if (rect.intersects(block.GetRect()))
			{
				return true;
			}
		break; 
		default:
		break;
		}

	}
	return false;
}



bool Game_Map::intersectsGoal(const RectF& rect) const
{
	const RectF bounds = worldBounds();

	if (!bounds.contains(rect)) {
		return true;
	}


	for (const auto& block : m_blocks)
	{
		if (block.getType() == BLOCK_GOAL && rect.intersects(block.GetRect()))
			return true;
	}
	return false;
}

std::optional<Vec2> Game_Map::findPlayerSpawn() const
{
	for (const auto& block : m_blocks)
	{
		if (block.getType() == BLOCK_PLAYER)
		{
			return block.GetRect().center();
		}
	}
	return std::nullopt;
}


bool Game_Map::CheckCollision_Line(const Line& line) const
{
	for (const auto& block : m_blocks)
	{
		if (block.getType() == BLOCK_SOLID)
		{
			if (line.intersectsAt(block.GetRect()))
			{
				return true;
			}
		}
	}
	return false;
}

bool Game_Map::CheckCollision_RecF(const RectF& rect) const
{

	const RectF bounds = worldBounds();

	// A. 出界：直接当作碰撞
	if (!bounds.contains(rect)) {
		return true;
	}

	for (const auto& block : m_blocks)
	{
		if (block.getType() == BLOCK_SOLID)
		{
			if (rect.intersectsAt(block.GetRect()))
			{
				return true;
			}
		}
	}
	return false;
}

/*
利用方法
Game_Map map;
if (!map.loadStageFromFile(FileSystem::CurrentDirectory() +U"example/Map/stage1.txt"))
	{
		Print << U"Failed to load stage1";
		return;
	}

	while (System::Update())
	{
		if (Key1.down())
		{
			map.loadStageFromFile(U"Map/stage1.txt");
		}
		if (Key2.down())
		{
			 map.loadStageFromFile(U"Map/stage2.txt");
		}

		//Update camera based on player position
		map.updateCamera(playerPos + playerSize / 2);
		map.update();
		map.draw();
		RectF(playerPos - gameMap.getCameraPos(), playerSize).draw(player);
	}

	//当たり判定
		if (input != Vec2(0, 0))　//　移動する場合
		{
			input = input.normalized();
			Vec2 nextPos = playerPos + input * playerSpeed * deltaTime;
			RectF playerRect(nextPos, playerSize);
			if (!gameMap.CheckCollision(playerRect))
			{
				playerPos = nextPos;
			}
			else
			{
				//handle collision response
			}
		}
*/
