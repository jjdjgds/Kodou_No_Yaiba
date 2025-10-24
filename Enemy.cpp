#include "Game.hpp"
#include "Enemy.hpp"
#include "Collision.hpp"
#include "Player.hpp"

using namespace Collision;

RectF Enemy::hurtRect() const
{
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = m_Position.movedBy(0, m_hitOffsetY * m_Scale.y), sz };
}

RectF Enemy::hurtRectAt(const Vec2& pos) const
{
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = pos.movedBy(0, m_hitOffsetY * m_Scale.y), sz };
}

RectF Enemy::attackRect() const
{
	// 攻撃矩形は被弾矩形と同サイズで、顔の向きに応じてオフセットする (調整可)
	const double xOffset = (m_FaceRight ? +17.0 : -17.0);
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = m_Position.movedBy(xOffset * m_Scale.x, m_hitOffsetY * m_Scale.y), sz };
}

Line Enemy::makeGroundProbeLine() const
{
	const double fwd = 17 * m_Scale.x;
	const double down = 40.0 * m_Scale.y;
	const Vec2 dir = (m_FaceRight ? Vec2{ +fwd, +down } : Vec2{ -fwd, +down });
	return Line{ m_Position, m_Position + dir };
}

void Enemy::update(Player& player, Game_Map& map)
{
	const double dt = Scene::DeltaTime();

	// --- 重力＆Y移動（地面補正） ---
	m_velY += m_gravity * dt;
	Vec2 tryPos = m_Position;
	tryPos.y += m_velY * dt;

	RectF testY = hurtRectAt(tryPos);
	if (map.CheckCollision(testY)) {
		const double step = 2.0;
		int guard = 0;
		// 地面にめり込んでいたら少しずつ戻す
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

	// --- 巡回範囲チェック ---
	if (m_Position.x > m_patrolR) { m_Position.x = m_patrolR; m_FaceRight = false; }
	if (m_Position.x < m_patrolL) { m_Position.x = m_patrolL; m_FaceRight = true; }

	// --- 足元プローブで落下判定（必要なら有効化） ---
	Line probe = makeGroundProbeLine();
	// if (!map.CheckCollision_Line(probe)) { m_FaceRight = !m_FaceRight; }

	// --- 各矩形を作成（ひと通り先に作る） ---
	const RectF eBoxHurt = hurtRect();          // 敵が被弾される矩形（中心基準）
	const RectF eBoxAttack = attackRect();       // 敵の攻撃矩形
	const RectF pHitBox(Arg::center = player.GetPlayerPosition(), player.GetPlayerHitBox()); // プレイヤー本体矩形（中心基準）
	const RectF pAttackBox(Arg::center = player.GetPlayerPosition(), player.GetPlayerAttackRengeBox()); // プレイヤーの攻撃矩形

	// --- デバッグ: クリックで敵がダメージ ---
	if (eBoxHurt.leftClicked()) {
		takeDamage(1);
	}

	// --- プレイヤーの攻撃が敵に当たったか？（プレイヤー側の状態を確認）---
	const bool playerAttackingThisFrame = (player.GetPlayerState() == StateMode::Attack) && player.IsPlayerAttacking();
	if (playerAttackingThisFrame && RectToRect(pAttackBox, eBoxHurt)) {
		// 敵は被弾
		if (!m_takeDamage) {
			takeDamage(1); // もしくは m_takeDamage = true; のみ
		}
	}

	// --- 敵の攻撃がプレイヤーに当たったか（攻撃中のみ／1回ヒット制御） ---
	if (m_state == AnimState::Attack) {
		// 攻撃モーション中だけ当たり判定を取る
		if (RectToRect(eBoxAttack, pHitBox)) {
			if (!m_hasHitPlayer) {
				// 初回ヒットのみダメージ処理
				player.takeDamage(1);
				m_hasHitPlayer = true;
			}
		}
	}
	// 攻撃状態でなければヒットフラグはリセット（アニメーション終了タイミングでもリセット可）
	else {
		// do nothing here — we reset m_hasHitPlayer when attack animation ends (下で)
	}

	// --- 行動決定（被弾／攻撃開始／通常移動） ---
	const bool gotHit = (RectToRect(pAttackBox, eBoxHurt) && playerAttackingThisFrame) || m_takeDamage;

	if (gotHit) {
		if (m_state != AnimState::Hurt) {
			setState(AnimState::Hurt);
			m_Speed = 0.0f;
		}
	}
	else if (m_state == AnimState::Attack) {
		// 攻撃中は行動固定（移動しない）
	}
	else if (KeySpace.down() && !AttackFlag) {
		// テスト用: Spaceで敵が攻撃（デバッグ）
		setState(AnimState::Attack);
		AttackFlag = true;
		m_Speed = 0.0f;
	}
	else {
		// 通常巡回移動
		m_Speed = (KeyS.pressed()) ? m_speedBase : m_speedBase; // デフォルトは m_speedBase
		// (上はテスト用。必要なら KeyS テスト削る)

		float vx = (m_FaceRight ? +1.0f : -1.0f) * m_Speed;
		m_Position.x += vx * dt;

		if (std::abs(vx) > 1.0f) {
			if (m_state != AnimState::Run) setState(AnimState::Run);
		}
		else {
			if (m_state != AnimState::Idle) setState(AnimState::Idle);
		}
	}

	// --- アニメーション更新 ---
	m_time += Scene::DeltaTime();
	const auto& A = m_anims[m_state];

	while (m_time >= A.frameTime) {
		m_time -= A.frameTime;

		if (A.loop) {
			m_frameIndex = (m_frameIndex + 1) % A.frames;
		}
		else {
			if (m_frameIndex < (A.frames - 1)) {
				++m_frameIndex;
			}
			else {
				// ノンループアニメ終了時の後処理
				if (m_state == AnimState::Hurt) {
					m_takeDamage = false;
					setState(AnimState::Idle);
				}
				else if (m_state == AnimState::Attack) {
					// 攻撃終了で「再ヒット可能」に戻す
					AttackFlag = false;
					m_hasHitPlayer = false;
					setState(AnimState::Idle);
				}
				break;
			}
		}
	}

	// --- デバッグ描画（最後に） ---
	if (m_debugDraw) {
		eBoxHurt.drawFrame(2.0, Palette::Red);
		eBoxAttack.drawFrame(2.0, Palette::Blue);
		probe.draw(2, Palette::Yellow); // 必要なら有効化
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
