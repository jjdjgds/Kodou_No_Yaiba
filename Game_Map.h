#pragma once
#include <Siv3D.hpp>
#include "Block.hpp"

class Game_Map
{
private:
	Array<Block> m_blocks;
	int m_width = 0;
	int m_height = 0;
	double m_chipWidth = 0.0;
	double m_chipHeight = 0.0;

public:
	Game_Map() = default;
	~Game_Map() = default;
	bool loadStageFromFile(const FilePath& path);
	void update();
	void draw() const;

};

