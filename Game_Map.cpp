#include "stdafx.h"

#include "Game_Map.hpp"
#include "MapLoader.hpp"

Game_Map::Game_Map()
{
	Block::LoadTextures(); // load all textures once
}

Game_Map::~Game_Map()
{
}

bool Game_Map::loadStageFromFile(const FilePath& path)
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


	//map height to screen and let width scroll
	m_chipHeight = 75.0f;
	m_chipWidth  = 75.0f;


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
	return true;
}

void Game_Map::loadNextStage()
{
	int currentStage = 2;
	currentStage++;

	FilePath nextPath = U"example/Map/stage" + Format(currentStage) + U".txt";

	if (!FileSystem::Exists(nextPath))
	{
		Print << U"🎉 All stages cleared!";
		return;
	}

	// Reset camera and load the next stage
	m_cameraPos = Vec2{ 0, 0 };

	if (loadStageFromFile(nextPath))
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

}

void Game_Map::draw() const
{
	
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
			TextureAsset(U"Wall").resized(size).draw(drawPos);
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

bool Game_Map::CheckCollision(const RectF& rect)
{
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
		case BLOCK_GOAL:
			if (rect.intersects(block.GetRect()))
			{
				loadNextStage();
				return true;
			}
		break;
		default:
		break;
		}

	}
	return false;
}

bool Game_Map::CheckCollision_Line(const Line& line)
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

bool Game_Map::CheckCollision_RecF(const RectF& rect)
{
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
