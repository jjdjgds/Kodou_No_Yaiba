#pragma once
#include "Common.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Game_Map.hpp"
class Game : public App::Scene
{
public:
	Game(const InitData& init);

	void update() override;
	void draw() const override;

private:
	Player player;  // ← これがないと Game.cpp 内で player が使えない！
	Array<Enemy> m_enemies;//敵キャラクターの配列
	Game_Map map;
};
