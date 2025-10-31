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

	void spawnBoss(const Vec2& pos);
	void  spawnEnemy_1(const Vec2& pos, double stride, bool faceRight, Vec2 scale);
	void  spawnEnemy_2(const Vec2& pos, double stride, bool faceRight, Vec2 scale);
	void loadFromMap(const Array<Block>& blocks, double chipWidth, double chipHeight);
	void update(Player& player, Game_Map& map);
	void draw(const Game_Map& map) const;

	void clear();

	Enemy_Boss* getBoss() const { return m_boss; }
};

