#include "stdafx.h"
#include "EnemySpawner.hpp"

void EnemySpawner::clear() {
	if (m_boss) { delete m_boss; m_boss = nullptr; }
	m_enemy1.clear();
	m_enemy2.clear();
}

// Spawn the boss at a position
void  EnemySpawner::spawnBoss(const Vec2& pos)
{
	if (!m_boss)
	{
		m_boss = new Enemy_Boss(pos);
	}
	else
	{
		m_boss->SetPosition(pos); // move existing boss
	}
}

bool EnemySpawner::areAllCleared() const {
	auto noneAlive1 = std::none_of(m_enemy1.begin(), m_enemy1.end(),
		[](auto const& p) { return p && !p->pendingRemoval(); });
	auto noneAlive2 = std::none_of(m_enemy2.begin(), m_enemy2.end(),
		[](auto const& p) { return p && !p->pendingRemoval(); });
	return noneAlive1 && noneAlive2;
}

void EnemySpawner::sweep() {
	m_enemy1.remove_if([](auto const& p) { return !p || p->pendingRemoval(); });
	m_enemy2.remove_if([](auto const& p) { return !p || p->pendingRemoval(); });

}


// Load all spawns from the map's blocks
void  EnemySpawner::loadFromMap(const Array<Block>& blocks, double chipWidth, double chipHeight)
{
	clear();

	for (const auto& block : blocks)
	{
		if (!block.IsUsed()) continue;

		const int type = block.getType();
		const Vec2 pos = centerOfTile(block, chipWidth, chipHeight); // center in tile

		switch (type)
		{
		case BLOCK_BOSS:
			spawnBoss(pos);
			break;
		case BLOCK_ENEMY_1:
		{
			auto e = std::make_unique<Enemy_1>(pos, 400.0);
			m_enemy1 << std::move(e);
			break;
		}
		case BLOCK_ENEMY_2:
		{
			auto e = std::make_unique<Enemy_2>(pos, 300.0);
			m_enemy2 << std::move(e);
			break;
		}
		default:
			break;
		}
	}
}

// Update all enemies and boss
void  EnemySpawner::update(Player& player, Game_Map& map,AllEffect& ae)
{
	if (m_boss)
		m_boss->update(player, map,ae);

	for (auto& e : m_enemy1)  if (e)e->update(player, map,ae);
	for (auto& e : m_enemy2) if (e) e->update(player, map,ae);

	// ★ 死亡动画结束 → 自动从容器移除
	m_enemy1.remove_if([](const std::unique_ptr<Enemy_1>& e) {
		return !e || e->pendingRemoval();
   });
	m_enemy2.remove_if([](const std::unique_ptr<Enemy_2>& e) {
		return !e || e->pendingRemoval();
   });
}

// Draw all enemies and boss
void  EnemySpawner::draw(const Game_Map& map) const
{
	if (m_boss)
		m_boss->draw(map);

	for (const auto& e : m_enemy1) { if (e) e->draw(map); }
	for (const auto& e : m_enemy2) { if (e) e->draw(map); }
}

