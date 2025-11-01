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

	if (!m_isAttacking && !m_deathanimation)
	{
		m_FaceRight = (playerPos.x >= m_boss_pos.x);
	}

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
		Print << U"[Boss] dead";

	}

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

	if (bossRect.intersects(pAttackBox))
	{
		if (player.IsPlayerAttacking() && !m_hasTakenHit)
		{
			m_hasTakenHit = true; // Prevent repeated hits during same attack
			m_boss_hp -= 1;
		}
		
	}
	else
	{
		m_hasTakenHit = false;
	}


	if (bossRect.intersects(playerRect))
	{
		// Boss rectangle edges
		float bossLeft = bossRect.left().begin.x;
		float bossRight = bossRect.right().begin.x;
		float bossTop = bossRect.top().begin.y;
		float bossBottom = bossRect.bottom().begin.y;

		// Player rectangle edges
		float playerLeft = playerRect.left().begin.x;
		float playerRight = playerRect.right().begin.x;
		float playerTop = playerRect.top().begin.y;
		float playerBottom = playerRect.bottom().begin.y;

		// Centers for push direction calculation
		float bossCenterX = (bossLeft + bossRight) / 2.0f;
		float bossCenterY = (bossTop + bossBottom) / 2.0f;
		float playerCenterX = (playerLeft + playerRight) / 2.0f;
		float playerCenterY = (playerTop + playerBottom) / 2.0f;

		// Calculate overlap
		float overlapX = std::min(bossRight, playerRight) - std::max(bossLeft, playerLeft);
		float overlapY = std::min(bossBottom, playerBottom) - std::max(bossTop, playerTop);

		// Resolve collision along the smaller overlap
		if (overlapX < overlapY)
		{
			// Horizontal push
			if (playerCenterX < bossCenterX)
				player.SetPlayerPosition(player.GetPlayerPosition() - Vec2{ overlapX, 0 }); // push left
			else
				player.SetPlayerPosition(player.GetPlayerPosition() + Vec2{ overlapX, 0 }); // push right
		}
		else
		{
			// Vertical push
			if (playerCenterY < bossCenterY)
				player.SetPlayerPosition(player.GetPlayerPosition() - Vec2{ 0, overlapY }); // push up
			else
				player.SetPlayerPosition(player.GetPlayerPosition() + Vec2{ 0, overlapY }); // push down
		}
	}

	if (m_OverBPM)
	{
		// Keep the boss completely still
		m_vel = Vec2{ 0, 0 };
		setState(AnimState_Boss::Idle);
		m_OverBPMTimer += Scene::DeltaTime();
		// Optional: effects or debug message
		Print << U"[Boss] Overheated... Cooling down!";

		// Wait for 2 seconds before resuming
		if (m_OverBPMTimer >= 2.0f)
		{
			// Boss cools down and resumes action
			m_OverBPMTimer = 0.0f;
			m_boss_bpm = 100;
			m_OverBPM = false;
			m_isAttacking = false;
			setState(AnimState_Boss::Battle_Idle);
			Print << U"[Boss] Cooldown finished, resuming combat!";
		}

		return; // stop rest of update while overheated
	}
	updateSpeedByBPM();
	// --- Animation Timer ---
	const auto& anim = m_anims[m_state];
	m_time += Scene::DeltaTime() * (m_boss_speed / m_base_speed);
	while (m_time >= anim.frameTime)
	{
		m_time -= anim.frameTime;

		if (anim.loop)
		{
			m_frameIndex = (m_frameIndex + 1) % anim.frames;
		}
		else
		{
			if (m_frameIndex < (anim.frames - 1))
			{
				++m_frameIndex;
			}

		}
	}

	if (m_isAttacking)
	{
		executePattern(player, map, m_pattern , dt);

		return;
	}

	switch (m_behavior)
	{
	case Boss_Behavior::idle:
		//Print << U"Idle";
		m_vel.x = 0.0f;
		if (!m_isDying && dist < chaseRange)
		{
			m_behavior = Boss_Behavior::Chase;
		}
		else if (m_isDying)
		{
			m_deathanimation = true;
			setState(AnimState_Boss::Dead);
		}
		break;
	case Boss_Behavior::Chase:
	{
		m_vel.x = (dx / dist) * m_boss_speed;
		setState(AnimState_Boss::Battle_Idle);

		bool playerInAttackRange = (dist < m_boss_range);
		if (playerInAttackRange )
		{
			m_behavior = Boss_Behavior::Attack;
		}
		break;
	}
	case Boss_Behavior::Attack:
	{
		//Print << U"Attack";
		m_vel.x = 0.0f;
		m_attackTimer += dt;

		bool playerInAttackRange = (dist <= m_boss_range);

		if (m_attackTimer >= m_attackCooldown)
		{
			if (m_isDying || playerInAttackRange )
			{
				// Perform current attack pattern
				handleAttackPattern(player, map , dt);
				//setState(AnimState_Boss::Attack);
			}
			m_attackTimer = 0.0; // Reset cooldown
		}
		// Return to chase if player moves out of attack range
		if (!m_isDying && !playerInAttackRange )
		{
			m_behavior = Boss_Behavior::Chase;
		}
		break;
	}
	default:
		break;
	};

	// --- Gravity ---
	m_vel.y += m_gravity * dt;

	// Update smoke independently (it persists)
	UpdateSmoke(map.getCameraPos(), player);
}

void Enemy_Boss::draw(const Game_Map& map) const
{
	constexpr int TEX_COLS = 8;
	constexpr int TEX_ROWS = 8;
	const Texture& tex = TextureAsset(U"Boss");
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
		const Vec2 cam = map.getCameraPos();

		// Boss world → screen position
		Vec2 screenPos = m_boss_pos - cam;

		// The same direction logic used in BossRect (the "real" forward)
		Vec2 forwardDir = (m_FaceRight ? Vec2{ +1.0, 0.0 } : Vec2{ -1.0, 0.0 });

		// Line length in pixels
		const double lineLen = 100.0;

		// Draw forward line
		Line(screenPos, screenPos + forwardDir * lineLen)
			.draw(4, ColorF(0.2, 1.0, 0.2)); // green line = forward

		// Small origin marker
		Circle(screenPos, 6).draw(ColorF(1.0, 0.2, 0.2)); // red = boss center

		BossRect(map.getCameraPos()).drawFrame(2.0, Palette::Red);
		attackRect(map.getCameraPos()).drawFrame(2.0, Palette::Blue);
		chaseRect(map.getCameraPos()).drawFrame(2.0, Palette::White);

	}
}

void Enemy_Boss::handleAttackPattern(Player& player, Game_Map& map , double dt)
{
	if (!m_isDying)
	{
		// ─────────────
		// NORMAL PATTERNS (1–6)
		// ─────────────
		if (m_isAttacking)
		{
			// Still performing the current pattern
			executePattern(player, map, m_pattern , dt);
			return;
		}
		else
		{
			// Current pattern finished → move to next one
			int next = static_cast<int>(m_pattern) + 1;
			if (next > static_cast<int>(Boss_Pattern::PATTERN_6))
				next = static_cast<int>(Boss_Pattern::PATTERN_0);

			m_pattern = static_cast<Boss_Pattern>(next);
			// reset pattern flags
			executePattern(player, map, m_pattern , dt);
			m_isAttacking = true; // Start new pattern
			m_pattern3Done = false;

			//Print << U"[Boss] → Switched to new pattern: " << static_cast<int>(m_pattern);
			return;
		}
	}
	else
	{
		// ─────────────
		// DEATH SEQUENCE
		// ─────────────
		const Boss_Pattern deathSequence[3] = {
			Boss_Pattern::PATTERN_1,
			Boss_Pattern::PATTERN_2,
			Boss_Pattern::PATTERN_5
		};

		m_pattern = deathSequence[m_deathPatternCounter % 3];
		executePattern(player, map, m_pattern , dt);
		m_deathPatternCounter++;

		if (m_deathPatternCounter >= 9)
		{
			m_behavior = Boss_Behavior::idle;
			//Print << U"Boss is dead!";
		
		}
	}
}

void Enemy_Boss::executePattern(Player& player, Game_Map& map, Boss_Pattern pattern, double dt)
{
	switch (pattern)
	{
	case Boss_Pattern::PATTERN_0:
		//Print << U"Boss uses PATTERN_0";
		m_isAttacking = false;
		break;

	case Boss_Pattern::PATTERN_1:
		//Print << U"Boss uses PATTERN_1";
		Pattern_1(player, map.getCameraPos(),dt);
		break;

	case Boss_Pattern::PATTERN_2:
		//Print << U"Boss uses PATTERN_2";
		Pattern_2(player, map.getCameraPos(),dt);
		break;

	case Boss_Pattern::PATTERN_3:
		//Print << U"Boss uses PATTERN_3";
		Pattern_3(player, map.getCameraPos());
		break;

	case Boss_Pattern::PATTERN_4:
		//Print << U"Boss uses PATTERN_4";
		Pattern_4(player, map.getCameraPos());
		break;

	case Boss_Pattern::PATTERN_5:
		//Print << U"Boss uses PATTERN_5 ";
		Pattern_5(player, map.getCameraPos(),dt);
		break;

	case Boss_Pattern::PATTERN_6:
		//Print << U"Boss uses PATTERN_6";
		Pattern_6(player, map.getCameraPos());
		break;

	default:
		break;
	}
}

void Enemy_Boss::Pattern_1(Player& player, Vec2 cam_pos , double dt_enemy)
{
	const double tScale = GetTimeScale();
	const double moveSpeed = 800.0 * dt_enemy;
	const double projectileSpeed = 1000.0;

	// Target = top middle of map
	const double mapW = Scene::Size().x;
	const double mapH = Scene::Size().y;
	Vec2 topMiddle(mapW * 0.5, mapH * 0.2);
	Vec2 middleLeft(mapW * 0.2, mapH * 0.5);
	Vec2 middleRight(mapW * 0.8, mapH * 0.5);

	static Array<Vec2> waypoints; // Move targets
	if (waypoints.isEmpty())
	{
		waypoints = { topMiddle, middleLeft, middleRight };
	}

	if (!m_isAttacking)
	{
		m_isAttacking = true;
		m_pattern1Phase = 0;
		m_pattern1Timer = 0.0;
		m_startPos = m_boss_pos;  // remember where boss started
		m_projectileActive = false;
		m_boss_bpm += 10;
		// Shuffle everything except the first waypoint
		if (waypoints.size() > 1)
		{
			Array<Vec2> rest = waypoints.slice(1);  // Copy all except first
			Shuffle(rest);                          // Shuffle that subset
			waypoints = { waypoints.front() };      // Keep the first fixed
			waypoints.append(rest);                 // Append shuffled remainder
		}
		m_currentWaypoint = 0; // NEW: current waypoint index
		//Print << U"[Pattern_1] Started: moving to top middle.";
	}

	m_pattern1Timer +=  dt_enemy;

	switch (m_pattern1Phase)
	{
	case 0: // Move to top middle
	{
		Vec2 target = waypoints[m_currentWaypoint];
		Vec2 dir = target - m_boss_pos;
		double dist = dir.length();

		if (dist > 1.0)
		{
			setState(AnimState_Boss::Fly);
			dir /= dist;
			m_boss_pos += dir * moveSpeed * tScale;
		}

		if (dist < 15.0)
		{
			m_boss_pos = target;
			m_pattern1Phase = 1;
			m_pattern1Timer = 0.0;
			// Face toward player in reverse cause of the sprite
			m_FaceRight = (player.GetPlayerPosition().x <= m_boss_pos.x);
			//Print << U"[Pattern_1] Reached waypoint " << m_currentWaypoint << U" → prepare to shoot.";
		}
		break;
	}
	case 1: // Fire projectile toward player
	{
		if (!m_projectileActive)
		{
			setState(AnimState_Boss::Throw_star);
			// Create simple projectile data
			m_projectilePos = m_boss_pos;
			Vec2 toPlayer = player.GetPlayerPosition() - m_boss_pos;
			m_projectileDir = toPlayer.normalized();
			m_projectileActive = true;
			//Print << U"[Pattern_1] Fired projectile toward player!";
		}

		// Move projectile
		if (m_projectileActive)
		{
			m_projectilePos += m_projectileDir * projectileSpeed * dt_enemy;

			const Texture& starTex = TextureAsset(U"shuriken");

			double projW = 60.0;
			double projH = 60.0;
			starTex
				.scaled(projW / starTex.width(), projH / starTex.height())
				.draw(m_projectilePos - Vec2(projW / 2, projH / 2) - cam_pos, ColorF(1.0));

			// Collision check
			// Collision with player attk box
			RectF attackRect = player.getAttackRect(cam_pos);
			// Collision with player
			RectF playerRect = player.getHitRect(cam_pos);
			Circle projectileCircle(m_projectilePos - cam_pos, 10);

			if (projectileCircle.intersects(attackRect))
			{
				if (player.IsPlayerAttacking())
				{
					// Reverse direction
					m_projectileDir = (m_boss_pos - m_projectilePos).normalized(); // send back to boss
					m_projectileReflected = true;
					//Print << U"[Pattern_1] Projectile reflected!";
				}
			}


			if (projectileCircle.intersects(playerRect))
			{
				m_projectileActive = false;
				//Print << U"[Pattern_1] Projectile hit player!";
				player.takeDamage(1);
			}
			if (m_projectileReflected)
			{
				m_projectileReflected = false;
				// Collision with boss
				Circle bossCircle(m_boss_pos - cam_pos, 30); // adjust radius

				if (projectileCircle.intersects(bossCircle))
				{
					m_projectileActive = false;
					Print << U"[Pattern_1] Reflected projectile hit boss!";
					m_boss_hp -= 2;
					// TODO: Apply damage to boss
				}
			}

			// Lifetime or off-screen cleanup
			if (m_pattern1Timer > 1.0)
			{
				m_projectileActive = false;
				m_pattern1Phase = 2;
				m_pattern1Timer = 0.0;
				//Print << U"[Pattern_1] Projectile finished → returning.";
			}
		}
		break;
	}
	case 2: // Return to start position
	{
		m_currentWaypoint++;

		if (m_currentWaypoint < (int)waypoints.size())
		{
			m_pattern1Phase = 0; // move to next target
			//Print << U"[Pattern_1] Moving to next waypoint " << m_currentWaypoint;
		}
		else
		{
			m_pattern1Phase = 3; // return to start
			//Print << U"[Pattern_1] All waypoints done → returning to start.";
		}
		break;
	}
	case 3:
	{
		Vec2 dir = m_startPos - m_boss_pos;
		double dist = dir.length();
		if (dist > 1.0)
		{
			setState(AnimState_Boss::Fly);
			dir /= dist;
			m_boss_pos += dir * moveSpeed * tScale;
		}

		if (dist < 15.0)
		{
			m_boss_pos = m_startPos;
			m_isAttacking = false; // done
			m_FaceRight = (player.GetPlayerPosition().x >= m_boss_pos.x);
			m_pattern1Phase = 0;
			//Print << U"[Pattern_1] Returned to start → Pattern complete.";
		}
		break;
	}
	}	
}

void Enemy_Boss::Pattern_2(Player& player, Vec2 cam_pos , double dt_enemy)
{
	const double dt = Scene::DeltaTime();
	const double tScale = GetTimeScale();
	// --- Timing and phase constants ---
	const double moveTime = 0.2;      // move duration
	const double attackTime = 0.20 * tScale;    // attack active window
	const double pauseTime = 0.20 * tScale;     // pause after attack
	const double moveSpeed = 400.0;    // short dash speed
	const int maxRepeats = 4;          // repeat count

	// Initialize pattern if just starting
	if (!m_isAttacking)
	{
		m_isAttacking = true;
		m_hasHitPlayer = false;
		m_pattern2Phase = 0;   // 0 = move, 1 = attack, 2 = pause
		m_pattern2Timer = 0.0;
		m_pattern2Count = 0;
		m_boss_bpm += 10;
		const double forwardOffset = m_hitBox.x * -0.2;
		m_pattern2Dir = (m_FaceRight ? Vec2{ +1.0, 0.0 } : Vec2{ -1.0, 0.0 });

		//Print << U"[Pattern_2] locked dir = " << m_pattern2Dir;
		//Print << U"[Pattern_2] Started multi short attacks!";
	}
	m_pattern2Timer += dt_enemy;

	switch (m_pattern2Phase)
	{
	case 0: // --- Move forward ---
		m_boss_pos.x += m_pattern2Dir.x * moveSpeed * dt_enemy;
		setState(AnimState_Boss::Dash);
		if (m_pattern2Timer >= moveTime)
		{
			m_pattern2Timer = 0.0;
			m_pattern2Phase = 1; // next: attack
			//Print << U"[Pattern_2] Move → Attack";
		}
		break;

	case 1: // --- Attack (active hit) ---
	{
		if (m_pattern2Count == 0)
			setState(AnimState_Boss::P2_1_Atk);
		else if (m_pattern2Count == 1)
			setState(AnimState_Boss::P2_2_Atk);
		else if (m_pattern2Count == 2)
			setState(AnimState_Boss::P2_3_Atk);
		else if (m_pattern2Count == 3)
			setState(AnimState_Boss::P2_4_Atk);

		const float hitW = m_hitBox.x * 1.1f;
		const float hitH = m_hitBox.y;
		const float forwardOffset = hitW;

		Vec2 hitboxCenter = m_boss_pos;
		hitboxCenter.x += m_pattern2Dir.x * forwardOffset;
		hitboxCenter.y += m_hitOffsetY;

		RectF attackHitbox(Arg::center = hitboxCenter - cam_pos, SizeF{ hitW, hitH });
		//attackHitbox.drawFrame(3, Palette::Orange);

		const RectF playerRect = player.getHitRect(cam_pos);
		if (attackHitbox.intersects(playerRect))
		{
			if(!m_hasHitPlayer)
			player.takeDamage(1);
			m_hasHitPlayer = true;  // Set the flag to prevent further damage
		}

		if (m_pattern2Timer >= attackTime)
		{
			m_pattern2Timer = 0.0;
			m_pattern2Phase = 2; // next: pause
			m_hasHitPlayer = false; // Reset the hit flag for the next attack phase
			//Print << U"[Pattern_2] Attack → Pause";
		}
		break;
	}

	case 2: // --- Pause before next cycle ---
		if (m_pattern2Timer >= pauseTime)
		{
			m_pattern2Timer = 0.0;
			m_pattern2Count++;

			if (m_pattern2Count >= maxRepeats)
			{
				m_isAttacking = false;
				setState(AnimState_Boss::Idle);
				//Print << U"[Pattern_2] Finished sequence.";
			}
			else
			{
				m_pattern2Phase = 0; // back to move
				//Print << U"[Pattern_2] Repeat " << m_pattern2Count;
			}
		}
		break;
	}
}

void Enemy_Boss::Pattern_3(Player& player, Vec2 cam_pos)
{
	// Only spawn smoke the first time Pattern 3 starts
	if (!m_pattern3Done)
	{
		setState(AnimState_Boss::Throw_Gas);
	
		// Determine throw direction
		Vec2 dir = (m_FaceRight ? Vec2{ 1.0, 0.0 } : Vec2{ -1.0, 0.0 });
	
		// Where smoke spawns (in front of boss)
		const float throwDist = m_hitBox.x * 4.0f; // how far boss throws
		m_smoke.position = m_boss_pos + dir * throwDist;
		m_smoke.position.y += m_hitOffsetY;
		m_smoke.lifetime = 5.0;   // lasts 5 seconds
		m_smoke.active = true;
	
		//Print << U"[Pattern_3] Boss throws smoke!";
		m_boss_bpm += 10;
		m_pattern3Done = true;  // prevent re-triggering
		m_isAttacking = false;  // signal pattern complete
	}
}

void Enemy_Boss::Pattern_4(Player& player, Vec2 cam_pos)
{
	if (!m_isAttacking)
	{
		m_boss_bpm += 10;
		m_isAttacking = true;
		m_counterReady = true;   // flag to indicate counter stance
		setState(AnimState_Boss::Parry); // new animation for counter
		Print << U"[Pattern_4] Boss is in counter stance!";
	}
	// --- Check if player is attacking ---
	const RectF playerAttackBox = player.getAttackRect(cam_pos);
	const RectF bossRect = BossRect(cam_pos);

	if (m_counterReady && bossRect.intersects(playerAttackBox))
	{
		if (player.IsPlayerAttacking())
		{
			setState(AnimState_Boss::P2_2_Atk); // new animation for counter
			// Player hit detection while in counter stance
			Print << U"[Pattern_4] Player attacked! Boss countering!";
			executeCounterAttack(player, cam_pos);
			m_counterReady = false;  // prevent multiple counters in same stance
		}
	}

	m_pattern4Timer += Scene::DeltaTime();
	if (m_pattern4Timer >= 2.0) // 2 seconds counter stance duration
	{
		m_isAttacking = false;
		m_hasHitPlayer = false;
		m_pattern4Timer = 0.0;
		Print << U"[Pattern_4] Boss exits counter stance.";
	}
}

void Enemy_Boss::Pattern_5(Player& player, Vec2 cam_pos , double dt_enemy)
{
	const double tScale = GetTimeScale();

	const double windupTime = 0.5 * tScale ;  // Pre-attack delay before dash
	const double dashTime = 0.6 * tScale;  // Max dash duration
	const double stopDistance = 50.0; // Stop this far in front of player
	const double postDashPause = 0.5 * tScale; // Pause before actual attack
	const double attackTime = 0.25 * tScale; // Duration of hit window
	const double cooldownTime = 0.6 * tScale;  // Recovery delay after attack

	if (!m_isAttacking)
	{
		m_boss_bpm += 10;
		m_hasHitPlayer = false;
		m_isAttacking = true;
		m_pattern5Phase = 0;  // Start at the wind-up phase
		m_pattern5Timer = 0.0; // Reset timer
		//Print << U"[Pattern_5] Started Attack Sequence.";
	}

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

	// Update the timer based on the delta time
	m_pattern5Timer += dt_enemy;

	// Debugging: Print out the timer value after update
	//Print << U"[Pattern_5] After Timer Update: " << m_pattern5Timer;

	switch (m_pattern5Phase)
	{
	case 0:  // Wind-up phase
		//Print << U"[Pattern_5] Wind-up phase.";
		setState(AnimState_Boss::Battle_Idle);
		if (m_pattern5Timer >= windupTime)
		{
			m_pattern5Timer = 0.0;  // Reset the timer after wind-up
			m_pattern5Phase = 1;  // Transition to dash phase
			//Print << U"[Boss] Wind-up complete → Dash!";
		}
		break;

	case 1:  // chargup phase
	{
		//Print << U"[Pattern_5] chargup phase.";
		setState(AnimState_Boss::Charge_Up);
		const float dashSpeed = 2000.0f; // Adjust speed here
		m_boss_pos.x += dir.x * dashSpeed *dt_enemy;

		// Compute current distance to player
		float currentDist = (player.GetPlayerPosition().x - m_boss_pos.x) * dir.x;

		// If we are close enough or dash time expired, stop the dash
		if (currentDist <= stopDistance || m_pattern5Timer >= dashTime)
		{
			m_pattern5Phase = 2;   // Move to attack phase
			m_pattern5Timer = 0.0;
			//Print << U"[Boss] Dash complete → Attack!";
		}
		break;
	}
	case 2:  // Dash phase
	{
		//Print << U"[Pattern_5] Pause before attack, timer=" << m_pattern5Timer;
		if (m_pattern5Timer >= postDashPause)
		{
			m_pattern5Timer = 0.0;
			m_pattern5Phase = 3;
			//Print << U"[Boss] Pause done → Begin attack";
		}
		break;

	}

	case 3:  // Attack phase
	{
		//Print << U"[Pattern_5] Attack phase.";
		setState(AnimState_Boss::Charge_Atk);
		if (m_pattern5Timer >= attackTime)
		{
			m_pattern5Phase = 4; // Transition to cooldown phase
			m_pattern5Timer = 0.0;
			//Print << U"[Boss] Attack complete → Cooldown.";
		}

		// Boss performs an actual attack hit check
		const float hitW = m_hitBox.x * 0.75f;
		const float hitH = m_hitBox.y;
		const float forwardOffset = hitW;

		Vec2 hitboxCenter = m_boss_pos;
		hitboxCenter.x += dir.x * forwardOffset;
		hitboxCenter.y += m_hitOffsetY;

		RectF attackHitbox(Arg::center = hitboxCenter - cam_pos, SizeF{ hitW, hitH });
		attackHitbox.drawFrame(3, Palette::Red); // Red = active attack phase

		const RectF playerRect = player.getHitRect(cam_pos);
		if (attackHitbox.intersects(playerRect))
		{
			Print << U"[Boss] Pattern 5 Attack HIT!";
			player.takeDamage(1);
			m_hasHitPlayer = true;  // Set the flag to prevent further damage
		}
		break;
	}
	case 4:
	{
		if (m_pattern5Timer >= cooldownTime)
		{
			m_hasHitPlayer = false;
			m_isAttacking = false;
			m_pattern5Timer = 0.0;
			m_pattern5Phase = 0;
			setState(AnimState_Boss::Battle_Idle);
			//Print << U"[Boss] Cooldown complete → Idle";
		}

		break;
	}
	}

	// Debugging: Print the timer and phase after every cycle
	//Print << U"[Pattern_5] After Phase " << m_pattern5Phase << U", Timer: " << m_pattern5Timer;
}

void Enemy_Boss::Pattern_6(Player& player, Vec2 cam_pos)
{
	// Begin pattern if not already attacking
	if (!m_isAttacking)
	{
		m_isAttacking = true;
		setState(AnimState_Boss::Meditate); // Boss is meditating (vulnerable)
		m_pattern6Timer = 0.0f;
		m_pattern6Count = 0;
		//Print << U"[Pattern_6 Start]";
	}

	m_pattern6Timer += Scene::DeltaTime();

	const RectF bossRect = BossRect(cam_pos);
	const RectF playerRect = player.getHitRect(cam_pos);
	const RectF attackRect = player.getAttackRect(cam_pos);

	// Check if player attack hits the boss
	if (player.IsPlayerAttacking() && attackRect.intersects(bossRect))
	{
		// Optional: small hit cooldown to prevent multi-hit in one frame
		if (!m_recentlyHit)
		{
			m_pattern6Count++;
			m_recentlyHit = true;
			m_hitTimer = 0.0f;
			Print << U"[Pattern_6] Boss hit " << m_pattern6Count << U" times";
		}
	}

	// Reset hit cooldown timer
	if (m_recentlyHit)
	{
		m_hitTimer += Scene::DeltaTime();
		if (m_hitTimer > 0.3f) // 0.3s hit delay
			m_recentlyHit = false;
	}

	// Boss wakes up after 3 hits or 5 seconds of meditation
	if (m_pattern6Count >= 3 || m_pattern6Timer >= 3.0f)
	{
		m_boss_bpm -= 20;               // Optional stat change
		m_pattern6Count = 0;
		m_pattern6Timer = 0.0f;
		m_isAttacking = false;
		Print << U"[Pattern_6 End] Boss woke up!";
	}
}

void Enemy_Boss::UpdateSmoke(Vec2 cam_pos, Player& player)
{
	if (!m_smoke.active)
		return;

	m_smoke.lifetime -= Scene::DeltaTime();
	if (m_smoke.lifetime <= 0.0)
	{
		m_smoke.active = false;
		// Print << U"[Smoke] Disappeared.";
		return;
	}

	// Smoke visual area
	const float smokeW = m_hitBox.x * 10.0f;
	const float smokeH = m_hitBox.y * 8.0f;

	RectF smokeArea(Arg::center = m_smoke.position - cam_pos, SizeF{ smokeW, smokeH });

	// --- Draw smoke texture instead of solid color ---
	const Texture& smokeTex = TextureAsset(U"Smoke");

	// Fade out as time passes
	double alpha = 0.4 * (m_smoke.lifetime / 3.0);

	smokeTex.scaled(smokeW / smokeTex.width(), smokeH / smokeTex.height())
		.draw(smokeArea.center() - Vec2(smokeW / 2.0, smokeH / 2.0), ColorF(1.0, alpha));

	//smokeArea.draw(ColorF(0.5, 0.5, 0.5, alpha)); // gray smoke area
	//smokeArea.drawFrame(2, Palette::Gray);

	// Collision check
	const RectF playerRect = player.getHitRect(cam_pos);
	if (smokeArea.intersects(playerRect))
	{
		Print << U"[Smoke] Player inside smoke!";

		// Apply damage over time if the player is inside the smoke
		if (m_smoke.timeInSmoke <= 0.0)
		{
			// Start the timer when the player first enters the smoke
			m_smoke.timeInSmoke = Scene::DeltaTime();
		}
		else
		{
			// Increment the time inside the smoke
			m_smoke.timeInSmoke += Scene::DeltaTime();

			// Damage the player every 1 second inside the smoke
			if (m_smoke.timeInSmoke >= 0.5f) // Apply damage every 1 second
			{
				const int damageAmount = 5; // Set the amount of damage to apply
				player.SetPlayerBPM(player.GetPlayerBPM() - damageAmount);  // This can still be here if you want to affect BPM.
				m_smoke.timeInSmoke = 0.0f; // Reset the timer after applying damage
			}
		}
	}
	else
	{
		// If player leaves the smoke, reset the timer
		m_smoke.timeInSmoke = 0.0f;
	}
}

void Enemy_Boss::executeCounterAttack(Player& player, Vec2 cam_pos)
{
	//setState(AnimState_Boss::CounterAttack); // new counter attack animation

	// Create a hitbox in front of the boss
	Vec2 attackDir = (m_FaceRight ? Vec2{ 1.0, 0.0 } : Vec2{ -1.0, 0.0 });
	const float hitW = m_hitBox.x * 1.5f;
	const float hitH = m_hitBox.y;
	Vec2 attackCenter = m_boss_pos + attackDir * (hitW * 0.5);
	attackCenter.y += m_hitOffsetY;

	RectF counterHitbox(Arg::center = attackCenter - cam_pos, SizeF{ hitW, hitH });

	const RectF playerRect = player.getHitRect(cam_pos);
	if (counterHitbox.intersects(playerRect))
	{
		player.takeDamage(1);
		m_hasHitPlayer = true;  // Set the flag to prevent further damage
	}
}

void Enemy_Boss::updateSpeedByBPM()
{
	float bpmRatio = m_boss_bpm / 100;
	m_boss_speed = m_base_speed + (m_base_speed * (bpmRatio - 1.0f) * 2.0f); // 2.0 = multiplier
	// Optional: clamp to prevent crazy speeds
	m_boss_speed = Clamp(m_boss_speed, 300.0f, 1200.0f);

	if (m_boss_bpm >= 180)
	{
		m_OverBPM = true;
		m_OverBPMTimer = 0.0;
		Print << U"[Boss] BPM too high! Boss overheats!";
	}
	//Print << U"[Boss_speed] : " << m_boss_speed;
	//Print << U"[BPM] : " << m_boss_bpm;
}
