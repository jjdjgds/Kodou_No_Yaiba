#include "stdafx.h"
#include "Enemy_Boss.hpp"
#include "Player.hpp"

void Enemy_Boss::update(Player& player)
{

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
		.resized(size * 3.5)
		.drawAt(pos);
}
