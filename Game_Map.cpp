#include "stdafx.h"
#include "Game_Map.h"
#include "MapLoader.hpp"


Game_Map::Game_Map()
{
	Block::LoadTextures(); // load all textures once
}

bool Game_Map::loadStageFromFile(const FilePath& path)
{
	Array<int> mapData;
	int width, height;

	if (!MapLoader::LoadMap(path, mapData, width, height))
		return false;

	m_width = width;
	m_height = height;

	//const Size screenSize = Scene::Size();
	//m_chipWidth = static_cast<double>(screenSize.x) / m_width;
	//m_chipHeight = static_cast<double>(screenSize.y) / m_height;


	// map height to screen and let width scroll
	//m_chipHeight = static_cast<double>(screenSize.y) / m_height;
	//m_chipWidth  = m_chipHeight;


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
		block.DrawBlock();
	}
}


/*
利用方法
Game_Map map;
if (!map.loadStageFromFile(FileSystem::CurrentDirectory() +U"Map/stage1.txt"))
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

		map.update();
		map.draw();
	}
*/
