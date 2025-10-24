#include "Game.hpp"
#include "Enemy.hpp"
#include "Collision.hpp"
#include "Player.hpp"

using namespace Collision;

RectF Enemy::hurtRect() const {// ダメージ判定矩形を取得
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = m_Position.movedBy(0, m_hitOffsetY * m_Scale.y), sz };
}

RectF Enemy::hurtRectAt(const Vec2& pos) const {// 指定位置での当たり判定矩形取得
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = pos.movedBy(0, m_hitOffsetY * m_Scale.y), sz };
}

void Enemy::update(const Player& player, Game_Map& map)
{
	const double dt = Scene::DeltaTime();


	m_velY += m_gravity * dt;
	Vec2 tryPos = m_Position;// 仮の位置
	tryPos.y += m_velY * dt;

	RectF testY = hurtRectAt(tryPos);// 仮の位置での当たり判定矩形
	if (map.CheckCollision(testY)) {
		const double step = 2.0;// 微小移動量
		int guard = 0;
		while (map.CheckCollision(testY) && guard++ < 200) {
			tryPos.y -= Math::Sign(m_velY) * step;
			testY = hurtRectAt(tryPos);
		}

		m_Position.y = tryPos.y;
		m_velY = 0.0;

		m_onGround = true;
	}
	else {
		m_Position.y = tryPos.y;
		m_onGround = false;
	}

	// 巡回範囲チェック
	if (m_Position.x > m_patrolR) { m_Position.x = m_patrolR; m_FaceRight = false; }
	if (m_Position.x < m_patrolL) { m_Position.x = m_patrolL; m_FaceRight = true; }





	const RectF eBox = hurtRect();
	// プレイヤーの当たり判定用長方形
	const RectF pBox(Arg::center = player.GetPlayerPosition(), player.GetPlayerAttackRengeBox());


	if (eBox.leftClicked()) takeDamage(1);// テスト用　敵の当たり判定BOXをクリックでダメージを受ける


	const bool gotHit = (RectToRect(pBox, eBox) && (player.GetPlayerState() == StateMode::Attack)) || m_takeDamage;
	
	if (gotHit) {
		if (m_state != AnimState::Hurt)
		{
			setState(AnimState::Hurt); // 状態変化時のみリセット
			m_Speed = 0.0f;      // ダメージ中は停止
		}
	}
	else {
		m_Speed = 0.0; // 通常移動速度に戻す


		m_Speed = (KeyS.pressed()) ? m_speedBase : 0.0;// テスト用　Sキーで停止



		float vx = (m_FaceRight ? 1.0f : -1.0f) * m_Speed;// 移動速度計算
		m_Position.x += vx * Scene::DeltaTime();// 位置更新

		
		if (std::abs(vx) > 1.0f) { // 在移动 → Run
			if (m_state != AnimState::Run) setState(AnimState::Run);
		}
		else {                    // 静止 → Idle
			if (m_state != AnimState::Idle) setState(AnimState::Idle);
		}
	}


	m_time += Scene::DeltaTime();
	const auto& A = m_anims[m_state];// 現在のアニメーション情報取得
	
	while (m_time >= A.frameTime) {
		m_time -= A.frameTime;

		if (A.loop) {
			m_frameIndex = (m_frameIndex + 1) % A.frames;// フレーム更新
		}
		else {
			if (m_frameIndex < (A.frames - 1)) 
				++m_frameIndex;
			else {
				m_takeDamage = false;
				break;
			}
		}
	}


	if (m_debugDraw)// デバッグ用
	{
		eBox.drawFrame(2.0, Palette::Red);
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
}

void Enemy::takeDamage(int damage)
{
	if (!m_takeDamage) {
		m_takeDamage = true;
		m_frameIndex = 0;
		m_time = 0.0;
	}
}
