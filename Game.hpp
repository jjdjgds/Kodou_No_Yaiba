#pragma once
#include "Common.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Game_Map.hpp"
#include "Game_UI.hpp"
#include "Game_BG.hpp"
#include "TimeStopManager.h"
#include "EnemySpawner.hpp"

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
	Game_UI Ui;
	Game_BG bg;
	EnemySpawner Boss_spawner;
	TimeStopManager tsm;
};
