#include "stdafx.h"
#include "Enemy_Boss.hpp"
#include "Player.hpp"
#include "Game_Map.hpp"

void Enemy_Boss::update(Player& player, Game_Map& map)
{

	const double dt = Scene::DeltaTime();

	Vec2 playerPos = player.GetPlayerPosition();
	float dx = playerPos.x - m_boss_pos.x;
	float dy = playerPos.y - m_boss_pos.y;
	dist = std::sqrt(dx * dx + dy * dy);

	switch (m_behavior)
	{
	//case Boss_Behavior::Attack:
	//	m_boss_speed = 0.0f;
	//	break;

	case Boss_Behavior::Chase:
	{
		bool inRange = (dist >= 0.0f && dist < 400.0f);
		m_FaceRight = (playerPos.x >= m_boss_pos.x);

		if (inRange)
		{
			float vx = (dx / dist) * m_boss_speed;
			m_vel.x = vx;
		}
		break;
	}

	case Boss_Behavior::idle:
		m_vel.x = 0.0f;
		break;
	}

	
	m_vel.y += m_gravity * dt;
	Vec2 tryPosX = m_boss_pos;
	tryPosX.x += m_vel.x * dt;

	RectF bossRect(
		tryPosX.x - m_hitBox.x / 2,
		tryPosX.y - m_hitBox.y / 2 + tex_offsetY,
		m_hitBox.x,
		m_hitBox.y
	);
	// --- X
	if (!map.CheckCollision_RecF(bossRect))
	{
		m_boss_pos.x = tryPosX.x;
	}
	else
	{
		m_vel.x = 0.0f; // stop when hitting wall
	}

	// --- Y
	Vec2 tryPosY = m_boss_pos;
	tryPosY.y += m_vel.y * dt;

	if (!map.CheckCollision_RecF(bossRect))
	{
		m_boss_pos.y = tryPosY.y;
	}
	else
	{
		m_vel.y = 0.0f; // stop falling when hitting the ground
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

	RectF(
	pos.x - m_hitBox.x /2,
	pos.y - m_hitBox.y /2 + tex_offsetY,
	m_hitBox.x,
	m_hitBox.y
	).drawFrame(2, Palette::Red); // 🔲 show hitbox
}
