#pragma once
#include <Siv3D.hpp>
#include "Block.hpp"

class Game_Map
{
private:
	Array<Block> m_blocks;
	int m_width = 0;
	int m_height = 0;
	double m_chipWidth = 100.0;
	double m_chipHeight = 100.0;

	Vec2 m_cameraPos = Vec2(0, 0);
public:
	Game_Map();
	~Game_Map() = default;
	bool loadStageFromFile(const FilePath& path);
	void update();
	void draw() const;

	void updateCamera(const Vec2& playerPos);
	Vec2 getCameraPos() const { return m_cameraPos; }

	bool CheckCollision(const RectF& rect) const;
	bool CheckCollision_Line(const Line& line) const;//敵の地面判定用
};

