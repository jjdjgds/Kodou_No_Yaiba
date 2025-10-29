#pragma once
#include <Siv3D.hpp>
#include "Enemy_Boss.hpp"
#include "Game_Map.hpp"
#include "Enemy_1.hpp"
#include "Enemy_2.hpp"


struct SpawnPoint
{
	Vec2 position;
	int type;
	double stride;
	bool faceRight;
	bool used = false;
};


class EnemySpawner
{
private:
	Enemy_Boss* m_boss = nullptr;
	Enemy_1* m_enemy1 = nullptr;
	Enemy_2* m_enemy2 = nullptr;
public:
	EnemySpawner() = default;
	~EnemySpawner() {
		delete m_enemy1;
		delete m_enemy2;
		delete m_boss; }

	void spawnBoss(const Vec2& pos, double stride = 0.0, bool faceRight = true, Vec2 scale = Vec2{ 1,1 });
	void  spawnEnemy_1(const Vec2& pos, double stride, bool faceRight, Vec2 scale);
	void  spawnEnemy_2(const Vec2& pos, double stride, bool faceRight, Vec2 scale);
	void loadFromMap(const Array<Block>& blocks, double chipWidth, double chipHeight);
	void update(Player& player, Game_Map& map);
	void draw(const Game_Map& map) const;

	Enemy_Boss* getBoss() const { return m_boss; }
};

