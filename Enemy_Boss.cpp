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


void Enemy_Boss::update(Player& player, Game_Map& map, AllEffect& ae)
{
	const double dt = Scene::DeltaTime() * TimeStopManager::GetEnemyScale();
	const Vec2 camPos = map.getCameraPos();
	Vec2 playerPos = player.GetPlayerPosition();

	// Update collision detection to use world coordinates
	const RectF eHurtBox = BossRect(Vec2{ 0, 0 }); // World coordinates
	const RectF eAttackBox = attackRect(Vec2{ 0, 0 }); // World coordinates
	const RectF eChaseBox = chaseRect(Vec2{ 0, 0 }); // World coordinates
	const RectF pHitBox(Arg::center = player.GetPlayerPosition(), player.GetPlayerHitBox()); // World coordinates
	const RectF pAttackBox = player.getAttackRect(Vec2{ 0, 0 }); // World coordinates

	const bool playerInChase = RectToRect(eChaseBox, pHitBox); // World coordinates
	const bool playerInAttack = RectToRect(eAttackBox, pHitBox); // World coordinates

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
		
		//Print << U"[Boss] dead";

	}

	// --- X Collision ---

	Vec2 probeX = m_boss_pos;
	probeX.x += m_vel.x * dt;

	RectF boxX = BossRectAt(probeX); // world → screen adjustment for collision
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

	RectF boxY = BossRectAt(probeY);

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
	const RectF bossRect = BossRect(Vec2{ 0, 0 });
	const RectF playerRect = player.getHitRect(Vec2{ 0, 0 });


	if (player.IsPlayerAttacking())
	{
		//Print << U"Player is attacking";
		//Print << U"m_hasTakenHit: " << m_hasTakenHit;
		//Print << U"bossRect: " << bossRect;
		//Print << U"pAttackBox: " << pAttackBox;
		//Print << U"Intersects: " << bossRect.intersects(pAttackBox);

		if (!m_hasTakenHit && bossRect.intersects(pAttackBox))
		{
			//Print << U"=== BOSS HIT! ===";
			// Boss takes damage once per attack
			m_hasTakenHit = true;
			m_boss_hp -= 1;
			ae.SetEffect(m_boss_pos, Vec2{ 1.0,0.3 }, 0.4, player.IsPlayerFacingRight());

			// Play sound
			AudioAsset(U"Sowrd4").play();
			//Print << U"[hp] : " << m_boss_hp;
		}
		else
		{
			if (m_hasTakenHit) {
				//rint << U"Already took hit this attack";
			}
			if (!bossRect.intersects(pAttackBox)) {
				//Print << U"Boss not in attack box";
				//Print << U"Distance between centers: " << bossRect.center().distanceFrom(pAttackBox.center());
			}
		}
	}
	else
	{
		// Attack animation ended, ready to take another hit next time
		m_hasTakenHit = false;
	}
	



	if (bossRect.intersects(playerRect))
	{
		//// Boss rectangle edges
		//float bossLeft = bossRect.left().begin.x;
		//float bossRight = bossRect.right().begin.x;
		//float bossTop = bossRect.top().begin.y;
		//float bossBottom = bossRect.bottom().begin.y;
		//
		//// Player rectangle edges
		//float playerLeft = playerRect.left().begin.x;
		//float playerRight = playerRect.right().begin.x;
		//float playerTop = playerRect.top().begin.y;
		//float playerBottom = playerRect.bottom().begin.y;
		//
		//// Centers for push direction calculation
		//float bossCenterX = (bossLeft + bossRight) / 2.0f;
		//float bossCenterY = (bossTop + bossBottom) / 2.0f;
		//float playerCenterX = (playerLeft + playerRight) / 2.0f;
		//float playerCenterY = (playerTop + playerBottom) / 2.0f;
		//
		//// Calculate overlap
		//float overlapX = std::min(bossRight, playerRight) - std::max(bossLeft, playerLeft);
		//float overlapY = std::min(bossBottom, playerBottom) - std::max(bossTop, playerTop);
		//
		//// Resolve collision along the smaller overlap
		//	// Push player away
		//if (overlapX < overlapY)
		//{
		//	// Horizontal push
		//	const Vec2 pushDir = (m_FaceRight ? Vec2{ +1, 0 } : Vec2{ -1, 0 });
		//	player.SetPlayerPosition(player.GetPlayerPosition() + pushDir * overlapX);
		//}
		//else
		//{
		//	// Vertical push
		//	player.SetPlayerPosition(player.GetPlayerPosition() - Vec2{ 0, overlapY });
		//}

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

				// Calculate overlap
				float overlapX = std::min(bossRight, playerRight) - std::max(bossLeft, playerLeft);
				float overlapY = std::min(bossBottom, playerBottom) - std::max(bossTop, playerTop);

				Vec2 playerPos = player.GetPlayerPosition();
				Vec2 newPos = playerPos;


				if (overlapX < overlapY)
				{
					// Horizontal push
					Vec2 pushDir = (m_FaceRight ? Vec2{ +1, 0 } : Vec2{ -1, 0 });
					newPos = playerPos + pushDir * overlapX;

					RectF newPlayerRect(newPos, player.GetPlayerScale());
					if (!map.CheckCollision(newPlayerRect))
					{
						player.SetPlayerPosition(newPos);
					}
					else
					{
						// Optional: Slide slightly along vertical if possible
						Vec2 slidePos = playerPos + Vec2{ 0, Sign(overlapY) } *2.0; // small nudge
						RectF slideRect(slidePos, player.GetPlayerScale());
						if (!map.CheckCollision(slideRect))
							player.SetPlayerPosition(slidePos);
					}
				}
				else
				{
					// Vertical push
					Vec2 pushDir = Vec2{ 0, -1 };
					newPos = playerPos + pushDir * overlapY;

					RectF newPlayerRect(newPos, player.GetPlayerScale());
					if (!map.CheckCollision(newPlayerRect))
					{
						player.SetPlayerPosition(newPos);
					}
				}
			
		    }
	
	}

	if (m_OverBPM)
	{
		// Keep the boss completely still
		m_vel = Vec2{ 0, 0 };
		setState(AnimState_Boss::Idle);
		m_OverBPMTimer += Scene::DeltaTime();
		// Optional: effects or debug message
		//Print << U"[Boss] Overheated... Cooling down!";

		// Wait for 2 seconds before resuming
		if (m_OverBPMTimer >= 2.0f)
		{
			// Boss cools down and resumes action
			m_OverBPMTimer = 0.0f;
			m_boss_bpm = 100;
			m_OverBPM = false;
			m_isAttacking = false;
			setState(AnimState_Boss::Battle_Idle);
			//Print << U"[Boss] Cooldown finished, resuming combat!";
		}

		return; // stop rest of update while overheated
	}
	updateSpeedByBPM();
	// --- Animation Timer ---
	const auto& anim = m_anims[m_state];
	m_time += dt * (m_boss_speed / m_base_speed);
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
			m_bossDead = true;
			//Print << U"isdead";
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

	// Draw pattern-specific elements
	drawPatternElements(map);

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

void Enemy_Boss::drawPatternElements(const Game_Map& map) const
{
	const Vec2 cam_pos = map.getCameraPos();

	// Draw Pattern 1 projectile (camera-relative)
	if (m_projectileActive)
	{
		const Texture& starTex = TextureAsset(U"shuriken");
		double projW = 60.0;
		double projH = 60.0;
		Vec2 screenPos = m_projectilePos - cam_pos; // Convert to screen space
		starTex
			.scaled(projW / starTex.width(), projH / starTex.height())
			.draw(screenPos - Vec2(projW / 2, projH / 2), ColorF(1.0));
	}

	// Draw Pattern 3 smoke (camera-relative)
	if (m_smoke.active && m_smoke.lifetime > 0.0)
	{
		const Texture& smokeTex = TextureAsset(U"Smoke");
		const float smokeW = m_hitBox.x * 10.0f;
		const float smokeH = m_hitBox.y * 8.0f;

		double alpha = 0.4 * (m_smoke.lifetime / 5.0); // Use 5.0 since lifetime starts at 5.0

		Vec2 screenPos = m_smoke.position - cam_pos; // Convert to screen space
		RectF smokeArea(Arg::center = screenPos, SizeF{ smokeW, smokeH });
		smokeTex.scaled(smokeW / smokeTex.width(), smokeH / smokeTex.height())
			.draw(smokeArea.pos, ColorF(1.0, alpha));
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

void Enemy_Boss::Pattern_1(Player& player, Vec2 cam_pos, double dt_enemy)
{
	const double tScale = GetTimeScale();
	const double moveSpeed = 2000.0 * dt_enemy;
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
		m_projectileReflected = false; // Reset reflection flag
		// Shuffle everything except the first waypoint
		if (waypoints.size() > 1)
		{
			Array<Vec2> rest = waypoints.slice(1);  // Copy all except first
			Shuffle(rest);                          // Shuffle that subset
			waypoints = { waypoints.front() };      // Keep the first fixed
			waypoints.append(rest);                 // Append shuffled remainder
		}
		m_currentWaypoint = 0;
	}

	m_pattern1Timer += dt_enemy;

	switch (m_pattern1Phase)
	{
	case 0: // Move to waypoint
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
			// Face toward player
			m_FaceRight = (player.GetPlayerPosition().x <= m_boss_pos.x);
		}
		break;
	}
	case 1: // Fire projectile toward player
	{
		if (!m_projectileActive)
		{
			setState(AnimState_Boss::Throw_star);

			// Set projectile start position at boss position
			m_projectilePos = m_boss_pos;

			// Calculate direction from boss to player
			Vec2 toPlayer = player.GetPlayerPosition() - m_boss_pos;

			// Ensure the direction is valid and normalized
			if (toPlayer.length() > 0.0001f) {
				m_projectileDir = toPlayer.normalized();
			}
			else {
				// Fallback: throw in facing direction
				m_projectileDir = (m_FaceRight ? Vec2{ 1.0, 0.0 } : Vec2{ -1.0, 0.0 });
			}

			m_projectileActive = true;
			m_projectileReflected = false;

			//Print << U"[Pattern_1] Throwing shuriken at player!";
			//Print << U"Boss Pos: " << m_boss_pos << U" Player Pos: " << player.GetPlayerPosition();
			//Print << U"Direction: " << m_projectileDir;

			throw_star.play();
		}

		// Move projectile toward player
		if (m_projectileActive)
		{
			
			// Update projectile position - moving toward player
			m_projectilePos += m_projectileDir * projectileSpeed * dt_enemy;

			// Collision check with player's attack (for parry/reflection)
			RectF attackRect = player.getAttackRect(Vec2{ 0, 0 }); // World coordinates
			RectF playerRect = player.getHitRect(Vec2{ 0, 0 });    // World coordinates
			Circle projectileCircle(m_projectilePos, 10); // World coordinates

			// Check if player parries the projectile
			if (projectileCircle.intersects(attackRect))
			{
				if (player.IsPlayerAttacking())
				{
					// Player parried - reverse direction back to boss
					parry.play();
					m_projectileDir = (m_boss_pos - m_projectilePos).normalized();
					m_projectileReflected = true;
					//Print << U"[Pattern_1] Player parried the shuriken!";
				}
			}

			// Check if projectile hits player
			if (projectileCircle.intersects(playerRect))
			{
				m_projectileActive = false;
				player.takeDamage(1);
				m_boss_bpm += 15;
				//Print << U"[Pattern_1] Shuriken hit player!";
			}

			// Check if reflected projectile hits boss
			if (m_projectileReflected)
			{
				Circle bossCircle(m_boss_pos, 30); // World coordinates

				if (projectileCircle.intersects(bossCircle))
				{
					m_projectileActive = false;
					m_boss_hp -= 2;
					//Print << U"[Pattern_1] Reflected shuriken hit boss!";
				}
			}

			// Lifetime cleanup - destroy projectile after 3 seconds
			if (m_pattern1Timer > 3.0)
			{
				m_projectileActive = false;
				m_pattern1Phase = 2;
				m_pattern1Timer = 0.0;
				//Print << U"[Pattern_1] Projectile timeout";
			}
		}
		break;
	}
	case 2: // Move to next waypoint or return
	{
		m_currentWaypoint++;

		if (m_currentWaypoint < (int)waypoints.size())
		{
			m_pattern1Phase = 0; // move to next target
			//Print << U"[Pattern_1] Moving to next waypoint: " << m_currentWaypoint;
		}
		else
		{
			m_pattern1Phase = 3; // return to start
			//Print << U"[Pattern_1] Returning to start position";
		}
		break;
	}
	case 3: // Return to start position
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
			m_isAttacking = false; // pattern complete
			m_FaceRight = (player.GetPlayerPosition().x >= m_boss_pos.x);
			m_pattern1Phase = 0;
			m_projectileActive = false;
			//Print << U"[Pattern_1] Pattern complete";
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
	const double attackTime = 0.25 * tScale;    // attack active window
	const double pauseTime = 0.35 * tScale;     // pause after attack
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
		{
			s1.play();
			setState(AnimState_Boss::P2_1_Atk);
		}
		else if (m_pattern2Count == 1)
		{
			s2.play();
			setState(AnimState_Boss::P2_2_Atk);
		}
		else if (m_pattern2Count == 2)
		{
			s3.play();
			setState(AnimState_Boss::P2_3_Atk);
		}
		else if (m_pattern2Count == 3)
		{
			s4.play();
			setState(AnimState_Boss::P2_4_Atk);
		}
			

		const float hitW = m_hitBox.x * 1.1f;
		const float hitH = m_hitBox.y;
		const float forwardOffset = hitW;

		Vec2 hitboxCenter = m_boss_pos; // World coordinates
		hitboxCenter.x += m_pattern2Dir.x * forwardOffset;
		hitboxCenter.y += m_hitOffsetY;

		RectF attackHitbox(Arg::center = hitboxCenter, SizeF{ hitW, hitH }); // World coordinates

		const RectF playerRect = player.getHitRect(Vec2{ 0, 0 }); // World coordinates
		if (attackHitbox.intersects(playerRect))
		{
			if (!m_hasHitPlayer)
				player.takeDamage(1);
			m_boss_bpm += 15;
			m_hasHitPlayer = true;
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
		smoke.play();
		m_smoke.active = true;
	
		//Print << U"[Pattern_3] Boss throws smoke!";
		m_pattern3Done = true;  // prevent re-triggering
		m_isAttacking = false;  // signal pattern complete
	}
}

void Enemy_Boss::Pattern_4(Player& player, Vec2 cam_pos)
{
	if (!m_isAttacking)
	{
		m_isAttacking = true;
		m_counterReady = true;   // flag to indicate counter stance
		setState(AnimState_Boss::Parry); // new animation for counter
		//Print << U"[Pattern_4] Boss is in counter stance!";
	}
	// --- Check if player is attacking ---
	const RectF playerAttackBox = player.getAttackRect(Vec2{ 0, 0 }); // World coordinates
	const RectF bossRect = BossRect(Vec2{ 0, 0 }); // World coordinates

	if (m_counterReady && bossRect.intersects(playerAttackBox))
	{
		if (player.IsPlayerAttacking())
		{
			setState(AnimState_Boss::P2_2_Atk);
			executeCounterAttack(player, Vec2{ 0, 0 }); // Use world coordinates
			m_counterReady = false;
		}
	}

	m_pattern4Timer += Scene::DeltaTime();
	if (m_pattern4Timer >= 2.0) // 2 seconds counter stance duration
	{
		m_isAttacking = false;
		m_hasHitPlayer = false;
		m_pattern4Timer = 0.0;
		//Print << U"[Pattern_4] Boss exits counter stance.";
	}
}

void Enemy_Boss::Pattern_5(Player& player, Vec2 cam_pos, double dt_enemy)
{
	const double dt_5 = Scene::DeltaTime();

	// --- Smooth timing for better flow ---
	const double windupTime = 0.8;                 // Shorter windup for responsiveness
	const double dashTime = 1.2;                   // Smooth dash duration
	const double stopDistance = 80.0;              // Comfortable stop distance
	const double postDashPause = 0.3;              // Quick pause for impact
	const double attackTime = 0.15;                // Brief attack window
	const double cooldownTime = 0.8;               // Quick recovery

	const float dashSpeed = 1200.0f;               // Smooth, fast speed
	const float attackRange = 180.0f;              // Generous attack range

	if (!m_isAttacking)
	{
		m_hasHitPlayer = false;
		m_isAttacking = true;
		m_pattern5Phase = 0;
		m_pattern5Timer = 0.0;

		// Store player position at start for consistent targeting
		m_pattern5TargetPos = player.GetPlayerPosition();

		//Print << U"[Pattern_5] Started Smooth Charge Attack!";
	}

	Vec2 playerPos = player.GetPlayerPosition();
	Vec2 dir = m_pattern5TargetPos - m_boss_pos;
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

	// Update the timer
	m_pattern5Timer += dt_5;

	// Get player rectangle once at the start
	const RectF playerRect = player.getHitRect(Vec2{ 0, 0 });

	switch (m_pattern5Phase)
	{
	case 0:  // Wind-up phase - build anticipation
	{
		setState(AnimState_Boss::Charge_Up); // Start charge animation early

		// Optional: Add subtle visual effects during windup
		// (particle effects, screen shake, etc.)

		if (m_pattern5Timer >= windupTime)
		{
			m_pattern5Timer = 0.0;
			m_pattern5Phase = 1;
		}
		break;
	}

	case 1:  // Smooth dash phase
	{
		setState(AnimState_Boss::Dash); // Use dash animation for movement

		// Only move if we haven't hit the player yet
		if (!m_hasHitPlayer)
		{
			// Smooth movement with consistent speed
			m_boss_pos.x += dir.x * dashSpeed * dt_5;
		}

		// Create attack hitbox during dash for early collision
		const float hitW = attackRange;
		const float hitH = m_hitBox.y * 1.2f;
		const float forwardOffset = hitW * 0.4f; // Slightly closer to boss

		Vec2 hitboxCenter = m_boss_pos;
		hitboxCenter.x += dir.x * forwardOffset;
		hitboxCenter.y += m_hitOffsetY;

		RectF attackHitbox(Arg::center = hitboxCenter, SizeF{ hitW, hitH });

		// Check collision with player during dash
		if (attackHitbox.intersects(playerRect) && !m_hasHitPlayer)
		{
			// Smooth stop on hit
			m_hasHitPlayer = true;
			m_vel.x = 0;
			player.takeDamage(1);
			m_boss_bpm += 15;
			m_pattern5Phase = 3; // Go directly to attack phase
			m_pattern5Timer = 0.0;
		}

		// Calculate remaining distance
		float remainingDist = (m_pattern5TargetPos.x - m_boss_pos.x) * dir.x - stopDistance;

		// Stop if reached target or time expired
		if (remainingDist <= 0 || m_pattern5Timer >= dashTime)
		{
			m_pattern5Phase = 2;
			m_pattern5Timer = 0.0;
		}
		break;
	}

	case 2:  // Brief pause for impact anticipation
	{
		setState(AnimState_Boss::Charge_Atk); // Transition to attack pose

		if (m_pattern5Timer >= postDashPause)
		{
			m_pattern5Timer = 0.0;
			m_pattern5Phase = 3;
		}
		break;
	}

	case 3:  // Attack release phase
	{
		setState(AnimState_Boss::Charge_Atk);

		// Create final attack hitbox
		const float hitW = attackRange;
		const float hitH = m_hitBox.y * 1.2f;
		const float forwardOffset = hitW * 0.5f;

		Vec2 hitboxCenter = m_boss_pos;
		hitboxCenter.x += dir.x * forwardOffset;
		hitboxCenter.y += m_hitOffsetY;

		RectF attackHitbox(Arg::center = hitboxCenter, SizeF{ hitW, hitH });

		// Debug draw
		if (m_debugDraw) {
			Vec2 screenCenter = hitboxCenter - cam_pos;
			RectF screenHitbox(Arg::center = screenCenter, SizeF{ hitW, hitH });
			screenHitbox.drawFrame(3, Palette::Red);
		}

		// Final collision check (if not already hit during dash)
		if (attackHitbox.intersects(playerRect) && !m_hasHitPlayer)
		{
			m_vel.x = 0;
			player.takeDamage(1);
			m_boss_bpm += 15;
			m_hasHitPlayer = true;
		}

		if (m_pattern5Timer >= attackTime)
		{
			m_pattern5Phase = 4;
			m_pattern5Timer = 0.0;
		}
		break;
	}

	case 4:  // Recovery phase
	{
		setState(AnimState_Boss::Battle_Idle); // Return to idle

		// Ensure boss is stopped
		m_vel.x = 0;

		if (m_pattern5Timer >= cooldownTime)
		{
			m_hasHitPlayer = false;
			m_isAttacking = false;
			m_pattern5Timer = 0.0;
			m_pattern5Phase = 0;
		}
		break;
	}
	}

	// Global movement stop when hit
	if (m_hasHitPlayer) {
		m_vel.x = 0;
	}
}

void Enemy_Boss::Pattern_6(Player& player, Vec2 cam_pos)
{
	// Begin pattern if not already attacking
	if (!m_isAttacking)
	{
		m_isAttacking = true;
		setState(AnimState_Boss::Meditate); // Boss is meditating (vulnerable)
		rest.play();
		m_pattern6Timer = 0.0f;
		m_pattern6Count = 0;
		//Print << U"[Pattern_6 Start]";
	}

	m_pattern6Timer += Scene::DeltaTime();

	const RectF bossRect = BossRect(Vec2{ 0, 0 }); // World coordinates
	const RectF playerRect = player.getHitRect(Vec2{ 0, 0 }); // World coordinates
	const RectF attackRect = player.getAttackRect(Vec2{ 0, 0 }); // World coordinates

	// Check if player attack hits the boss
	if (player.IsPlayerAttacking() && attackRect.intersects(bossRect))
	{
		// Optional: small hit cooldown to prevent multi-hit in one frame
		if (!m_recentlyHit)
		{
			m_pattern6Count++;
			m_recentlyHit = true;
			m_hitTimer = 0.0f;
			//Print << U"[Pattern_6] Boss hit " << m_pattern6Count << U" times";
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
		setState(AnimState_Boss::Battle_Idle);
		rest.stop();
		//Print << U"[Pattern_6 End] Boss woke up!";
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
		m_smoke.timeInSmoke = 0.0;
		return;
	}

	// Smoke visual area for collision - use world coordinates
	const float smokeW = m_hitBox.x * 10.0f;
	const float smokeH = m_hitBox.y * 8.0f;
	RectF smokeArea(Arg::center = m_smoke.position, SizeF{ smokeW, smokeH }); // World coordinates

	// Collision check - get player rect in world space
	const RectF playerRect = player.getHitRect(Vec2{ 0, 0 }); // World coordinates
	if (smokeArea.intersects(playerRect))
	{
		//Print << U"*** PLAYER IN SMOKE! ***";
		if (m_smoke.timeInSmoke <= 0.0)
		{
			m_smoke.timeInSmoke = Scene::DeltaTime();
		}
		else
		{
			m_smoke.timeInSmoke += Scene::DeltaTime();
			if (m_smoke.timeInSmoke >= 0.5f)
			{
				const int damageAmount = 5;
				player.SetPlayerBPM(player.GetPlayerBPM() - damageAmount);
				m_smoke.timeInSmoke = 0.0f;
			}
		}
	}
	else
	{
		m_smoke.timeInSmoke = 0.0f;
	}
}

void Enemy_Boss::executeCounterAttack(Player& player, Vec2 cam_pos)
{
	// Create a hitbox in front of the boss - world coordinates
	Vec2 attackDir = (m_FaceRight ? Vec2{ 1.0, 0.0 } : Vec2{ -1.0, 0.0 });
	const float hitW = m_hitBox.x * 1.5f;
	const float hitH = m_hitBox.y;
	Vec2 attackCenter = m_boss_pos + attackDir * (hitW * 0.5); // World coordinates
	attackCenter.y += m_hitOffsetY;

	RectF counterHitbox(Arg::center = attackCenter, SizeF{ hitW, hitH }); // World coordinates

	const RectF playerRect = player.getHitRect(Vec2{ 0, 0 }); // World coordinates
	if (counterHitbox.intersects(playerRect))
	{
		parry.play();
		player.takeDamage(1);
		m_boss_bpm += 15;
		m_hasHitPlayer = true;
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
		//Print << U"[Boss] BPM too high! Boss overheats!";
	}
	//Print << U"[Boss_speed] : " << m_boss_speed;
	//Print << U"[BPM] : " << m_boss_bpm;
}
