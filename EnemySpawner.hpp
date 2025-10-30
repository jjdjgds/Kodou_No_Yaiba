#pragma once
#include <Siv3D.hpp>
#include "Enemy_Boss.hpp"
#include "Game_Map.hpp"

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
<<<<<<< HEAD

	Array<std::unique_ptr<Enemy_1>> m_enemy1;
	Array<std::unique_ptr<Enemy_2>> m_enemy2;

	static Vec2 centerOfTile(const Block& b, double chipW, double chipH) {
		return b.GetPos() + Vec2(chipW * 0.5, chipH * 0.5);
	}
public:
	EnemySpawner() = default;
	~EnemySpawner() {
		clear();
		 }
=======
public:
	EnemySpawner() = default;
	~EnemySpawner() { delete m_boss; }
>>>>>>> f43e84f90f86a93fec393bdf9d1b45c4ec9c3251

	void spawnBoss(const Vec2& pos);
	void loadFromMap(const Array<Block>& blocks, double chipWidth, double chipHeight);
	void update(Player& player, Game_Map& map);
	void draw(const Game_Map& map) const;

	void clear();

	Enemy_Boss* getBoss() const { return m_boss; }
};

