#include "Game.hpp"
#include "Enemy.hpp"
#include "Collision.hpp"
using namespace Collision;


void Enemy::update()
{
	m_Speed = KeyS.pressed() ? 0.0 : m_speedBase;// テスト用　Sキーで停止

	setHitbox(Vec2(100, 150));//テスト用 当たり判定サイズ設定
	RectF pBox(getPosition(), getScale());// 敵の当たり判定用長方形
	pBox.setPos(getPosition()).setSize(m_hitBox);

	const Circle c{ Cursor::Pos(), 30 };//テスト用 マウスの当たり判定

	float vx = (m_FaceRight ? 1.0f : -1.0f) * m_Speed;// 移動速度計算
	m_Position.x += vx * Scene::DeltaTime();// 位置更新

	// 巡回範囲チェック
	if (m_Position.x > m_patrolR) { m_Position.x = m_patrolR; m_FaceRight = false; }
	if (m_Position.x < m_patrolL) { m_Position.x = m_patrolL; m_FaceRight = true; }

	if (RectToCircle(pBox, c))setState(AnimState::Hurt);// ダメージを受けたらHurt
	else if (std::abs(vx) > 1.0) setState(AnimState::Run);// 移動中はRun
	else setState(AnimState::Idle);// 停止中はIdle

	const auto& A = m_anims[m_state];// 現在のアニメーション情報取得
	m_time += Scene::DeltaTime();
	while (m_time >= A.frameTime) {
		m_time -= A.frameTime;
		m_frameIndex = (m_frameIndex + 1) % A.frames;// フレーム更新
	}
}

void Enemy::draw() const
{
	const auto& A = m_anims.at(m_state);
	const Texture& tex = TextureAsset(A.asset);

	const int32 frameW = tex.width() / A.frames;
	const int32 frameH = tex.height();
	const int32 sx = m_frameIndex * frameW;

	auto reg = tex(sx, 0, frameW, frameH);
	(m_FaceRight ? reg : reg.mirrored())
		.scaled(m_Scale.x, m_Scale.y)
		.drawAt(m_Position);

	if (!m_debugDraw) return;// デバッグ用

	RectF(getPosition(), getScale())
		.setPos(getPosition()).setSize(m_hitBox)
		.drawFrame(2.0, Palette::Red);
}
