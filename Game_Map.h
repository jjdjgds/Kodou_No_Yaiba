#pragma once
#include <Siv3D.hpp>
#include "Block.hpp"

#define MAP_CHIP_NUM_X (20)
#define MAP_CHIP_NUM_Y (12)
#define MAP_CHIP_WIDTH (100.0f)
#define MAP_CHIP_HEIGHT (100.0f)

#define BLOCK_MAX (MAP_CHIP_NUM_X * MAP_CHIP_NUM_Y)

class Game_Map
{
private:
	Array<Block> m_blocks;
	int currentStage = 0;

public:
	Game_Map();
	~Game_Map() = default;
	void loadStage(int stageNumber);
	void update();
	void draw() const;
};

