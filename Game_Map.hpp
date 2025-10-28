#pragma once
#include <Siv3D.hpp>
#include "Block.hpp"

class Enemy_Boss;

class Game_Map
{
private:
	Array<Block> m_blocks;
	int m_width = 0;
	int m_height = 0;
	double m_chipWidth = 100.0;
	double m_chipHeight = 100.0;

	Vec2 m_cameraPos = Vec2(0, 0);
	Enemy_Boss* m_boss = nullptr; // pointer
public:
	Game_Map();
	~Game_Map();
	bool loadStageFromFile(const FilePath& path);
	void loadNextStage();
	void update();
	void draw() const;

	void updateCamera(const Vec2& playerPos);
	Vec2 getCameraPos() const { return m_cameraPos; }

	bool CheckCollision(const RectF& rect) ;
	bool CheckCollision_Line(const Line& line);//敵の地面判定用
	bool CheckCollision_RecF(const RectF& rect);
};

