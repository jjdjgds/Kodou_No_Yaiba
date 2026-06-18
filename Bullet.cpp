#include <Siv3D.hpp>
#include "Bullet.hpp"
#include "Game_Map.hpp" 
#include "Collision.hpp"

RectF Bullet::rectScreen(const Vec2& cam) const {
	const Vec2 center = (m_pos + faceApply(m_hitBiasLocal)) - cam;
	return RectF(Arg::center = center, m_hitSize);
}

RectF Bullet::rectWorld() const {
	const Vec2 center = (m_pos + faceApply(m_hitBiasLocal));
	return RectF(Arg::center = center, m_hitSize);
}


void Bullet::draw(const Game_Map& map) const
{
	if (!m_alive) return;

	const Vec2 center = (m_pos + faceApply(m_drawBiasLocal)) - map.getCameraPos();

	const Texture& tex = TextureAsset(U"Enemy2Bullet");
	(m_faceRight ? tex : tex.mirrored())
		.resized(m_hitSize.x * 2.0, m_hitSize.y * 2.0)
		.drawAt(center);
	RectF(Arg::center = center, m_hitSize).drawFrame(1, Palette::Red);
	

}


bool Bullet::updateAndHit(double dt, const Game_Map& map,
						  const RectF& pHitBoxScreen,
						   const RectF& pAttackBoxScreen,
							const Vec2& cam, bool flg,bool doge)
{
	if (!m_alive) return false;

	m_life -= dt;
	if (m_life <= 0.0) { m_alive = false; return false; }

	const double speed = m_vel.length();
	if (speed <= 0.0) { m_alive = false; return false; }

	const Vec2 dir = (m_vel / speed);

	double remaining = speed * dt;
	const double step = 2.0;
	int guard = 0;


	while (remaining > 0.0 && ++guard < 256) {
		const double d = Min(remaining, step);
		const Vec2 probe = m_pos + dir * d;


		const RectF box(Arg::center = probe + faceApply(m_hitBiasLocal), m_hitSize);


		

		if (map.CheckCollision_RecF(box)) {
			m_alive = false;
			return false;// 衝突したら終了
		}


		m_pos = probe;
		remaining -= d;

		if (Collision::RectToRect(rectScreen(cam), pAttackBoxScreen) && flg)
		{
			m_vel.x = -m_vel.x; // X反射
			m_pos.x += (m_vel.x > 0 ? 1 : -1); // 少しずらす
			m_faceRight = !m_faceRight; // 反転
			m_RemoveFlag = true; // 　 ここを追加
			//U"BCunter"
			const Audio& Cunter1 = AudioAsset(U"BCunter");
			Cunter1.stop();
			Cunter1.play();

		}
		if (rectScreen(cam).intersects(pHitBoxScreen)) {// プレイヤーに命中
			if (doge)
			{
				// Dodge中は当たり判定無視・弾は生存
				return false;
			}
			else
			{
				m_alive = false;
				return true; // 当たり
			}
		}
	}

	return false;
}
