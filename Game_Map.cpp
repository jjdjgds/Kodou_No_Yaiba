#include "stdafx.h"
#include "Game_Map.h"

const int Stage1_Map[MAP_CHIP_NUM_Y][MAP_CHIP_NUM_X]
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

};

static const int* getStageMap(int stageNumber)
{
	switch (stageNumber)
	{
	case 1: return &Stage1_Map[0][0];
	//case 2: return &Stage2_Map[0][0];
	default: return &Stage1_Map[0][0];
	}
}

Game_Map::Game_Map()
{
	Block::LoadTextures(); // load all textures once
	loadStage(1); // Load stage 1 initially

}

void Game_Map::loadStage(int stageNumber)
{
	currentStage = stageNumber;
	m_blocks.clear();
	m_blocks.reserve(BLOCK_MAX);

	// Pointer to selected map data
	const int* mapData = getStageMap(stageNumber);

	for (int y = 0; y < MAP_CHIP_NUM_Y; y++)
	{
		for (int x = 0; x < MAP_CHIP_NUM_X; x++)
		{
			int type = *(mapData + y * MAP_CHIP_NUM_X + x);
			Block block;
			block.SetBlock(Vec2(x * MAP_CHIP_WIDTH, y * MAP_CHIP_HEIGHT),
				  Vec2(MAP_CHIP_WIDTH, MAP_CHIP_HEIGHT),
				  type);
			m_blocks << block;
		}
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
		block.DrawBlock();
	}
}


/*
利用方法
Game_Map map;

	while (System::Update())
	{
		if (Key1.down())
		{
			map.loadStage(1);
		}
		if (Key2.down())
		{
			map.loadStage(2);
		}

		map.update();
		map.draw();
	}
*/
