#include "stdafx.h"
#include "Enemy_Boss.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "Game_Map.hpp"


void Enemy_Boss::update(Player& player, Game_Map& map)
{
	const double dt = Scene::DeltaTime() * TimeStopManager::GetEnemyScale();
	const Vec2 camPos = map.getCameraPos();
	Vec2 playerPos = player.GetPlayerPosition();

	m_FaceRight = (playerPos.x >= m_boss_pos.x);

	float dx = playerPos.x - m_boss_pos.x;
	float dy = playerPos.y - m_boss_pos.y;
	float distSq = dx * dx + dy * dy;

	// Avoid divide-by-zero
	dist = 0.0f;
	if (distSq > 0.0001f)
		dist = std::sqrt(distSq);

	// --- Check for death sequence ---
	if (m_boss_hp <= 0 && !m_isDying)
	{
		m_isDying = true;
		m_behavior = Boss_Behavior::Attack;
		m_attackTimer = m_attackCooldown;
		m_deathPatternCounter = 0;
	}

	switch (m_behavior)
	{
	case Boss_Behavior::idle:
		m_vel.x = 0.0f;

		if (!m_isDying && dist < chaseRange)
		{
			m_behavior = Boss_Behavior::Chase;
		}
		break;
	case Boss_Behavior::Chase:
		if (dist < chaseRange)
		{
			m_vel.x = (dx / dist) * m_boss_speed;
			
			if (dist < m_boss_range)
			{
				m_behavior = Boss_Behavior::Attack;
			}
		}
		else
		{
			m_behavior = Boss_Behavior::idle;
			m_vel.x = 0.0f;
		}
		break;
	case Boss_Behavior::Attack:
		m_vel.x = 0.0f;
		m_attackTimer += dt;
		if (m_attackTimer >= m_attackCooldown)
		{
			// Perform current attack pattern
			handleAttackPattern(player, map);
			m_attackTimer = 0.0; // Reset cooldown
		}
		// Return to chase if player moves out of attack range
		if (!m_isDying && dist > m_boss_range)
		{
			m_behavior = Boss_Behavior::Chase;
		}
		break;
	default:
		break;
	}

	// --- Gravity ---
	m_vel.y += m_gravity * dt;

	// --- X Collision ---

	Vec2 tryPosX = m_boss_pos;
	tryPosX.x += m_vel.x * dt;

	RectF bossRectX(
		(tryPosX.x - m_hitBox.x / 2) - camPos.x,
		(tryPosX.y - m_hitBox.y / 2 + tex_offsetY) - camPos.y,
		m_hitBox.x,
		m_hitBox.y
	);
	
	if (!map.CheckCollision_RecF(bossRectX))
	{
		m_boss_pos.x = tryPosX.x;
	}
	else
	{
		m_vel.x = 0.0f; // stop when hitting wall
	}

	// --- Y Collision ---
	Vec2 tryPosY = m_boss_pos;
	tryPosY.y += m_vel.y * dt;

	RectF bossRectY(
	(tryPosY.x - m_hitBox.x / 2) - camPos.x,
	(tryPosY.y - m_hitBox.y / 2 + tex_offsetY) - camPos.y,
	m_hitBox.x,
	m_hitBox.y
	);

	if (!map.CheckCollision_RecF(bossRectY))
	{
		m_boss_pos.y = tryPosY.y;
	}
	else
	{
		m_vel.y = 0.0f; // stop falling when hitting the ground
	}

	// --- Player collision ---
	RectF bossRect(
		(tryPosX.x - m_hitBox.x / 2) - camPos.x,
		(tryPosX.y - m_hitBox.y / 2 + tex_offsetY) - camPos.y,
		m_hitBox.x,
		m_hitBox.y
	);

	const RectF playerRect = player.getHitRect(camPos);

	if (bossRect.intersects(playerRect))
	{
		// Get actual coordinates from the Line
		float bossLeft = bossRect.left().begin.x;
		float bossRight = bossRect.right().begin.x;
		float bossTop = bossRect.top().begin.y;
		float bossBottom = bossRect.bottom().begin.y;

		float playerLeft = playerRect.left().begin.x;
		float playerRight = playerRect.right().begin.x;
		float playerTop = playerRect.top().begin.y;
		float playerBottom = playerRect.bottom().begin.y;

		float overlapX = std::min(bossRight, playerRight) - std::max(bossLeft, playerLeft);
		float overlapY = std::min(bossBottom, playerBottom) - std::max(bossTop, playerTop);

		if (overlapX < overlapY)
		{
			// Push along X
			if (m_FaceRight)
				player.SetPlayerPosition(player.GetPlayerPosition() + Vec2(overlapX, 0.0f));
			else
				player.SetPlayerPosition(player.GetPlayerPosition() - Vec2(overlapX, 0.0f));
		}
		else
		{
			// Push along Y
			player.SetPlayerPosition(player.GetPlayerPosition() - Vec2(0.0f, overlapY));
		}


		// Optional: deal damage
		//player.TakeDamage(m_boss_atk);
	}
}

void Enemy_Boss::draw(Vec2 pos, Vec2 size) const
{
	Vec2 screenPos = m_boss_pos - pos;// Apply camera offset only for drawing

	const auto& A = m_anims.at(m_state);
	const Texture& tex = TextureAsset(A.asset);

	const int32 frameW = tex.width() / A.frames;
	const int32 frameH = tex.height();
	const int32 sx = m_frameIndex * frameW;

	auto reg = tex(sx, 0, frameW, frameH);
	(m_FaceRight ? reg : reg.mirrored())
		.resized(size)
		.drawAt(screenPos);

	RectF (
	screenPos.x - m_hitBox.x / 2,
	screenPos.y - m_hitBox.y / 2 + tex_offsetY,
	m_hitBox.x,
	m_hitBox.y
	).drawFrame(2, Palette::Blue); // 🔵 screen-space hitbox (for visual alignment)
}

void Enemy_Boss::handleAttackPattern(Player& player, Game_Map& map)
{
	if (!m_isDying)
	{
		executePattern(player, map, m_pattern);
	}
	else
	{
		const Boss_Pattern deathSequence[3] = {
			Boss_Pattern::PATTERN_1,
			Boss_Pattern::PATTERN_2,
			Boss_Pattern::PATTERN_5
		};
		// Select current pattern in death sequence
		m_pattern = deathSequence[m_deathPatternCounter % 3];

		// Execute the same logic as normal patterns
		executePattern(player, map, m_pattern);

		m_deathPatternCounter++;

		// After 3 full cycles (3 * 3 = 9 patterns), boss is dead
		if (m_deathPatternCounter >= 9)
		{
			m_behavior = Boss_Behavior::idle;

			Print << U"Boss is dead!";
			// Set flag for removal or death animation here
		}

		return;
	}

	int next = static_cast<int>(m_pattern) + 1;

	if (next >= static_cast<int>(Boss_Pattern::PATTERN_MAX))
	{
		next = 0; // loop back to PATTERN_1
	}

	m_pattern = static_cast<Boss_Pattern>(next);
}

void Enemy_Boss::executePattern(Player& player, Game_Map& map, Boss_Pattern pattern)
{
	switch (pattern)
	{
	case Boss_Pattern::PATTERN_1:
		// Example: simple melee attack
		Print << U"Boss uses PATTERN_1 (melee slash)";
		break;

	case Boss_Pattern::PATTERN_2:
		// Example: jump attack
		Print << U"Boss uses PATTERN_2 (jump attack)";
		break;

	case Boss_Pattern::PATTERN_3:
		// Example: projectile attack
		Print << U"Boss uses PATTERN_3 (projectile)";
		break;

	case Boss_Pattern::PATTERN_4:
		// Example: area of effect
		Print << U"Boss uses PATTERN_4 (AOE)";
		break;

	case Boss_Pattern::PATTERN_5:
		// Example: charge toward player
		Print << U"Boss uses PATTERN_5 (charge)";
		break;

	case Boss_Pattern::PATTERN_6:
		// Example: summon minions
		Print << U"Boss uses PATTERN_6 (summon)";
		break;

	default:
		break;
	}
}
