#include "stdafx.h"
#include "EnemySpawner.hpp"


// Spawn the boss at a position
void  EnemySpawner::spawnBoss(const Vec2& pos, double stride, bool faceRight, Vec2 scale)
{
	if (!m_boss)
	{
		m_boss = new Enemy_Boss(pos, stride, faceRight, scale);
	}
	else
	{
		m_boss->SetPosition(pos); // move existing boss
		m_boss->SetScale(scale);
	}
}

void  EnemySpawner::spawnEnemy_1(const Vec2& pos, double stride, bool faceRight, Vec2 scale)
{
	if (!m_enemy1)
	{
		m_enemy1 = new Enemy_1(pos, stride);
	}
	else
	{
		m_enemy1->setPosition(pos); // move existing boss

	}
}


void  EnemySpawner::spawnEnemy_2(const Vec2& pos, double stride, bool faceRight, Vec2 scale)
{
	if (!m_enemy2)
	{
		m_enemy2 = new Enemy_2(pos, stride);
	}
	else
	{
		m_enemy2->setPosition(pos); // move existing boss

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
		case BLOCK_ENEMY_1:
			spawnBoss(pos);
			break;
		case BLOCK_ENEMY_2:
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
		m_boss->draw(m_boss->GetPosition() - map.getCameraPos(), { map.getChipWidth() * 3.5,map.getChipHeight() * 3.5 });
}

