#include "stdafx.h"
#include "EnemySpawner.hpp"


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

// Load all spawns from the map's blocks
void  EnemySpawner::loadFromMap(const Array<Block>& blocks, double chipWidth, double chipHeight)
{
	delete m_boss;
	m_boss = nullptr;

	for (const auto& block : blocks)
	{
		Vec2 pos = block.GetPos() + Vec2(chipWidth / 2, chipHeight / 2); // center in tile

		switch (block.getType())
		{
		case BLOCK_BOSS:
			spawnBoss(pos);
			break;
		default:
			break;
		}
	}
}

// Update all enemies and boss
void  EnemySpawner::update(Player& player, Game_Map& map)
{
	if (m_boss)
		m_boss->update(player, map);
}

// Draw all enemies and boss
void  EnemySpawner::draw(const Game_Map& map) const
{
	if (m_boss)
		m_boss->draw(map);

}

