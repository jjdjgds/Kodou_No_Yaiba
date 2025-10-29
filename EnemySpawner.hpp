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
public:
	EnemySpawner() = default;
	~EnemySpawner() { delete m_boss; }

	void spawnBoss(const Vec2& pos);
	void loadFromMap(const Array<Block>& blocks, double chipWidth, double chipHeight);
	void update(Player& player, Game_Map& map);
	void draw(const Game_Map& map) const;

	Enemy_Boss* getBoss() const { return m_boss; }
};

