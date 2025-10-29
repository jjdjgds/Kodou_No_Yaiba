#include "stdafx.h"
#include "Enemy_Boss.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "Game_Map.hpp"
#include "Collision.hpp"


using namespace Collision;

RectF Enemy_Boss::BossRect(const Vec2& cam) const
{
	const double forwardOffset = m_hitBox.x * -0.2; // 調整可
	const double xOffset = (m_FaceRight ? +forwardOffset : -forwardOffset);
	return RectF{ Arg::center = m_boss_pos.movedBy(xOffset, m_hitOffsetY) - cam, m_hitBox };
}

RectF Enemy_Boss::BossRectAt(const Vec2& pos) const
{
	const double forwardOffset = m_hitBox.x * 1; // 調整可
	const double xOffset = (m_FaceRight ? +forwardOffset : -forwardOffset);
	return RectF{ Arg::center = pos.movedBy(xOffset , m_hitOffsetY), m_hitBox };
}

RectF Enemy_Boss::attackRect(const Vec2& cam) const
{
	// Base size of the boss hitbox
	const double baseW = m_hitBox.x;
	const double baseH = m_hitBox.y;

	// Direction the boss is facing
	const int dir = (m_FaceRight ? +1 : -1);

	// Compute the rectangle's world-space center
	Vec2 worldCenter = m_boss_pos;
	worldCenter.x += dir * (baseW * 0.5 + m_boss_range * 0.5); // offset forward
	worldCenter.y += m_hitOffsetY;

	// Rectangle size (boss width + attack range)
	const SizeF sz{ baseW + m_boss_range, baseH };

	// Convert to screen space
	return RectF{ Arg::center = worldCenter - cam, sz };
}
RectF Enemy_Boss::chaseRect(const Vec2& cam) const
{
	// Base size of boss
	const double baseW = m_hitBox.x;
	const double baseH = m_hitBox.y;

	// Expand forward in facing direction
	const double halfWidth = baseW * 0.5;
	const int dir = (m_FaceRight ? +1 : -1);

	// Calculate rectangle center in world space
	Vec2 worldCenter = m_boss_pos;              // start at boss position
	worldCenter.x += dir * (halfWidth + chaseRange * 0.5); // extend forward
	worldCenter.y += m_hitOffsetY;

	// Rectangle size (extends in facing direction)
	const SizeF sz{ baseW + chaseRange, baseH };

	// Convert to screen space (subtract camera)
	return RectF{ Arg::center = worldCenter - cam, sz };
}


void Enemy_Boss::update(Player& player, Game_Map& map)
{
	const double dt = Scene::DeltaTime() * TimeStopManager::GetEnemyScale();
	const Vec2 camPos = map.getCameraPos();
	Vec2 playerPos = player.GetPlayerPosition();

	const RectF eHurtBox = BossRect(camPos);
	const RectF eAttackBox = attackRect(camPos);                           // 敵の攻撃矩形（前方オフセット）
	const RectF eChaseBox = chaseRect(camPos);// プレイヤー追跡矩形（広域前方オフセット）
	const RectF pHitBox(Arg::center = player.GetPlayerPosition() - camPos, player.GetPlayerHitBox()); // プレイヤー本体
	const RectF pAttackBox = player.getAttackRect(camPos); // プレイヤーの攻撃矩形（Player の関数利用）

	const bool playerInChase = RectToRect(eChaseBox, pHitBox);// プレイヤーが追跡矩形内にいるか
	const bool playerInAttack = RectToRect(eAttackBox, pHitBox);// プレイヤーが攻撃矩形内にいるか

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
		//Print << U"Idle";
		m_vel.x = 0.0f;
		setState(AnimState_Boss::Idle);
		if (!m_isDying && dist < chaseRange)
		{
			m_behavior = Boss_Behavior::Chase;
		}
		break;
	case Boss_Behavior::Chase:
		//Print << U"Chase";
		m_vel.x = (dx / dist) * m_boss_speed;
		setState(AnimState_Boss::Run);
		if (dist < m_boss_range)
		{
			m_behavior = Boss_Behavior::Attack;
		}

		break;
	case Boss_Behavior::Attack:
		//Print << U"Attack";
		m_vel.x = 0.0f;
		m_attackTimer += dt;
		if (m_attackTimer >= m_attackCooldown)
		{
			// Perform current attack pattern
			handleAttackPattern(player, map);
			//setState(AnimState_Boss::Attack);
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

	// --- Animation Timer ---
	const AnimDesc_Boss& anim = m_anims[m_state];
	m_time += Scene::DeltaTime();

	if (m_time >= anim.frameTime)
	{
		m_time -= anim.frameTime;
		m_frameIndex++;

		if (m_frameIndex >= anim.frames)
		{
			if (anim.loop)
			{
				m_frameIndex = 0;
			}
			else
			{
				m_frameIndex = anim.frames - 1;

				// Attack finished
				if (m_state == AnimState_Boss::Attack)
				{
					m_isAttacking = false;
					setState(AnimState_Boss::Idle);
				}
			}
		}
	}

	// --- Gravity ---
	m_vel.y += m_gravity * dt;

	// --- X Collision ---

	Vec2 probeX = m_boss_pos;
	probeX.x += m_vel.x * dt;

	RectF boxX = BossRectAt(probeX - map.getCameraPos()); // world → screen adjustment for collision
	
	if (!map.CheckCollision_RecF(boxX))
	{
		m_boss_pos.x = probeX.x;
	}
	else
	{
		m_vel.x = 0.0; // stop horizontal movement on collision
	}

	// --- Y Collision ---
	Vec2 probeY = m_boss_pos;
	probeY.y += m_vel.y * dt;

	RectF boxY = BossRectAt(probeY - map.getCameraPos());

	if (!map.CheckCollision_RecF(boxY))
	{
		m_boss_pos.y = probeY.y;
	}
	else
	{
		m_vel.y = 0.0; // stop vertical movement when hitting floor/ceiling
	}
	// ----------------------------
	// --- Final Foot Stabilization
	// ----------------------------
	const RectF bossRect = BossRect(map.getCameraPos());
	const RectF playerRect = player.getHitRect(map.getCameraPos());


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
			// Push player away
			if (overlapX < overlapY)
			{
				// Horizontal push
				const Vec2 pushDir = (m_FaceRight ? Vec2{ +1, 0 } : Vec2{ -1, 0 });
				player.SetPlayerPosition(player.GetPlayerPosition() + pushDir * overlapX);
			}
			else
			{
				// Vertical push
				player.SetPlayerPosition(player.GetPlayerPosition() - Vec2{ 0, overlapY });
			}

		}
	}
}

void Enemy_Boss::draw(const Game_Map& map) const
{
	constexpr int TEX_COLS = 5;
	constexpr int TEX_ROWS = 5;
	const Texture& tex = TextureAsset(U"Enemy1");
	const Size c = { tex.width() / TEX_COLS, tex.height() / TEX_ROWS };

	const auto& R = m_anims.at(m_state);
	int linear = R.start + m_frameIndex;
	int row = R.row + linear / TEX_COLS;
	int col = linear % TEX_COLS;

	row = Clamp(row, 0, TEX_ROWS - 1);
	col = Clamp(col, 0, TEX_COLS - 1);

	const int32 sx = col * c.x;
	const int32 sy = row * c.y;
	const auto  reg = tex(sx, sy, c);

	double sxScale = m_boss_scale.x / c.x;
	double syScale = m_boss_scale.y / c.y;

	Vec2 center = m_boss_pos - map.getCameraPos();
	const double visualH = c.y * syScale;
	center.y -= (visualH * 0.5 - m_hitBox.y * 0.5) - m_hitOffsetY;
	(m_FaceRight ? reg : reg.mirrored())
		.scaled(sxScale, syScale)
		.drawAt(center);

	
	if (m_debugDraw) {

		BossRect(map.getCameraPos()).drawFrame(2.0, Palette::Red);
		attackRect(map.getCameraPos()).drawFrame(2.0, Palette::Blue);
		chaseRect(map.getCameraPos()).drawFrame(2.0, Palette::White);
	}
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
	Print << U"[Pattern_5]";

	const double dt = Scene::DeltaTime();

	const double windupTime = 0.5;   // Pre-attack delay
	const double dashTime = 0.3;   // Dash movement duration
	const double attackTime = 0.25;  // Attack animation/hit duration
	const double cooldownTime = 0.6;   // Recovery delay after attack

	if (!m_isAttacking)
	{
		m_isAttacking = true;
		m_pattern5Phase = 0;
		m_pattern5Timer = 0.0;
		//setState(AnimState_Boss::Attack); // trigger going to dash animation
	}
	Print << U"[Pattern_5]->1";
	Vec2 playerPos = player.GetPlayerPosition();
	Vec2 dir = playerPos - m_boss_pos;
	float dist = dir.length();
	if (dist > 0.0001f)
	{
		dir /= dist;
	}
	else
	{
		dir = Vec2{ (m_FaceRight ? 1.0 : -1.0), 0.0 };
	}

	// Face the player
	m_FaceRight = (dir.x >= 0.0f);

	m_pattern5Timer += dt;
	Print << U"[Pattern_5]->2";
	switch (m_pattern5Phase)
	{
	case 0:
	{
		Print << U"[Pattern_5]->3";

		// Boss prepares to dash (can flash, shake, or play sound)
		if (m_pattern5Timer >= windupTime)
		{
			
			m_pattern5Timer = 0.0;
			m_pattern5Phase = 1;
			Print << U"[Boss] Wind-up complete → Dash!";
		}
		break;
	}
	case 1:
	{
		const float dashSpeed = 600.0f; // adjust speed here
		m_boss_pos.x += dir.x * dashSpeed * dt;

		if (m_pattern5Timer >= dashTime)
		{
			m_pattern5Phase = 2;
			m_pattern5Timer = 0.0;
			Print << U"[Boss] Dash complete → Attack!";
		}
		break;
	}
	case 2:
	{
		// Boss performs an actual attack hit check
		const float hitW = m_hitBox.x;
		const float hitH = m_hitBox.y;
		const float forwardOffset = hitW * 0.5f;

		Vec2 hitboxCenter = m_boss_pos;
		hitboxCenter.x += dir.x * forwardOffset;
		hitboxCenter.y += m_hitOffsetY;

		RectF attackHitbox(Arg::center = hitboxCenter - cam_pos, SizeF{ hitW, hitH });
		attackHitbox.drawFrame(3, Palette::Red); // Red = active attack phase

		const RectF playerRect = player.getHitRect(cam_pos);
		if (attackHitbox.intersects(playerRect))
		{
			Print << U"[Boss] Pattern 5 Attack HIT!";
			// TODO: player.TakeDamage(m_boss_atk);
		}
		if (m_pattern5Timer >= attackTime)
		{
			m_pattern5Phase = 3;
			m_pattern5Timer = 0.0;
			Print << U"[Boss] Attack complete → Cooldown.";
		}
		break;
	}
	case 3:
	{
		// Boss is vulnerable or catching breath
		if (m_pattern5Timer >= cooldownTime)
		{
			m_isAttacking = false;
			setState(AnimState_Boss::Idle);
			m_pattern5Timer = 0.0;
			m_pattern5Phase = 0;
			Print << U"[Boss] Cooldown done → Back to idle.";
			return; // pattern finished
		}
		break;
	}
	}
}
