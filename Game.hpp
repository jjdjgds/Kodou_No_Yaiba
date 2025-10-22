#pragma once
#include "Common.hpp"
#include "Player.hpp"

class Game : public App::Scene
{
public:
	Game(const InitData& init);

	void update() override;
	void draw() const override;

private:
	Player player;  // ← これがないと Game.cpp 内で player が使えない！
	//Enemy enemy;
};
