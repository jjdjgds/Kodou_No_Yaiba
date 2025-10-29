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

	if (m_isAttacking) {
		return; // Exit early to prevent state change
	}

	switch (m_behavior)
	{
	case Boss_Behavior::idle:
		Print << U"Idle";
		m_vel.x = 0.0f;

		if (!m_isDying && dist < chaseRange)
		{
			m_behavior = Boss_Behavior::Chase;
		}
		break;
	case Boss_Behavior::Chase:
		Print << U"Chase";
		m_vel.x = (dx / dist) * m_boss_speed;

		if (dist < m_boss_range)
		{
			m_behavior = Boss_Behavior::Attack;
		}

		break;
	case Boss_Behavior::Attack:
		Print << U"Attack";
		m_vel.x = 0.0f;
		m_attackTimer += dt;
		if (m_attackTimer >= m_attackCooldown)
		{
			// Perform current attack pattern
			//handleAttackPattern(player, map);
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

	const auto& A = m_anims.at(m_state);
	const Texture& tex = TextureAsset(A.asset);

	const int32 frameW = tex.width() / A.frames;
	const int32 frameH = tex.height();
	const int32 sx = m_frameIndex * frameW;

	auto reg = tex(sx, 0, frameW, frameH);
	(m_FaceRight ? reg : reg.mirrored())
		.resized(size)
		.drawAt(pos);

	RectF (
	pos.x - m_hitBox.x / 2,
	pos.y - m_hitBox.y / 2 + tex_offsetY,
	m_hitBox.x,
	m_hitBox.y
	).drawFrame(2, Palette::Blue); // 🔵 screen-space hitbox (for visual alignment)
}

void Enemy_Boss::handleAttackPattern(Player& player, Game_Map& map)
{
	if (!m_isDying)
	{
		executePattern(player, map, Boss_Pattern::PATTERN_5);
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
		Print << U"Boss uses PATTERN_5 ";
		Pattern_5(player,map.getCameraPos());
		break;

	case Boss_Pattern::PATTERN_6:
		// Example: summon minions
		Print << U"Boss uses PATTERN_6 (summon)";
		break;

	default:
		break;
	}
}

void Enemy_Boss::Pattern_5(Player& player, Vec2 cam_pos)
{
	m_isAttacking = true;

	Vec2 dir = player.GetPlayerPosition() - (m_boss_pos - cam_pos);
	float dist = dir.length();
	dir /= dist;
	// Face the player
	m_FaceRight = (dir.x >= 0.0f);

	float dashStopDistance = 100.0f;
	float dashDistance = std::max(dist - dashStopDistance, 0.0f);

	Vec2 dashPosition = m_boss_pos;
	dashPosition.x += dir.x * dashDistance;  // Only move along the X axis

	m_boss_pos = dashPosition;

	// Attack hitbox range (half of the width of the boss hitbox)
	float attackHitboxRange = m_hitBox.x / 2.0f;  // Adjust if needed

	RectF attackHitbox;

	// If the boss is moving towards the right, create the hitbox to the right of the boss
	if (m_FaceRight)
	{
		attackHitbox = RectF(
			m_boss_pos.x + m_hitBox.x / 2.0f - cam_pos.x, // Right of the boss
			m_boss_pos.y - m_hitBox.y / 2.0f + tex_offsetY - cam_pos.y, // Vertical position
			m_hitBox.x, // Width of the attack hitbox
			m_hitBox.y  // Height of the attack hitbox
		);
	}
	else
	{
		attackHitbox = RectF(
			m_boss_pos.x - m_hitBox.x / 2.0f - m_hitBox.x - cam_pos.x, // Left of the boss
			m_boss_pos.y - m_hitBox.y / 2.0f + tex_offsetY - cam_pos.y, // Vertical position
			m_hitBox.x, // Width of the attack hitbox
			m_hitBox.y  // Height of the attack hitbox
		);
	}
	// --- Visualize the Attack Hitbox ---
	attackHitbox.drawFrame(2, Palette::Black);  // Red frame to visualize the hitbox

	// Check if the player's hitbox intersects with the attack hitbox
	const RectF playerRect = player.getHitRect(cam_pos);

	if (attackHitbox.intersects(playerRect))
	{
		Print << U"PATTERN_5 Hit";
	}

	m_isAttacking = false;
}
