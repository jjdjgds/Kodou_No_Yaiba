// Enemy.cpp
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
	// 被弾矩形をベースに前方へオフセット（攻撃判定）
	const double forwardOffset = m_hitBox.x * 0.6 * m_Scale.x; // 調整可
	const double xOffset = (m_FaceRight ? +forwardOffset : -forwardOffset);
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = m_Position.movedBy(xOffset, m_hitOffsetY * m_Scale.y), sz };
}

Line Enemy::makeGroundProbeLine() const
{
	const double fwd = 17 * m_Scale.x;
	const double down = 40.0 * m_Scale.y;
	const Vec2 dir = (m_FaceRight ? Vec2{ +fwd, +down } : Vec2{ -fwd, +down });
	return Line{ m_Position, m_Position + dir };
}

RectF Enemy::footRect() const
{
	const double footHeight = 8.0; // 足元判定の高さ（調整可）
	const SizeF sz{ m_hitBox.x * m_Scale.x, footHeight };
	const double bottomY = m_Position.y + (m_hitBox.y * 0.5 * m_Scale.y);
	return RectF(Arg::center = Vec2(m_Position.x, bottomY + (footHeight * 0.5)), sz);
}

void Enemy::update(Player& player, Game_Map& map)
{
	const double dt = Scene::DeltaTime();

	// --- m_speedBase が未設定なら最初に設定しておく（ヘッダでの初期化ミス対策） ---
	if (m_speedBase == 0.0f) {
		m_speedBase = m_Speed;
		if (m_speedBase == 0.0f) m_speedBase = 120.0f; // 保険：適当なデフォルト
	}

	// ----------------------------
	// --- 横移動（X）処理（衝突チェック）
	// ----------------------------
	m_Speed = m_speedBase;
	float vx = (m_FaceRight ? +1.0f : -1.0f) * m_Speed;

	Vec2 tryPosX = m_Position;
	tryPosX.x += vx * dt;

	// 横移動用の矩形（y は現在位置ベース）
	RectF testX = hurtRectAt(tryPosX);

	if (!map.CheckCollision(testX)) {
		// 横移動可能
		m_Position.x = tryPosX.x;
	}
	else {
		// 衝突：巡回なら向きを変えるのが自然
		m_FaceRight = !m_FaceRight;
		vx = 0;
	}

	// ----------------------------
	// --- 重力＆縦移動（Y）処理
	// ----------------------------
	m_velY += m_gravity * dt;
	Vec2 tryPosY = m_Position;
	tryPosY.y += m_velY * dt;

	RectF testY = hurtRectAt(tryPosY);

	const double guardStep = 0.5; // 補正の刻み（px単位）
	const int guardMax = 400;     // 無限ループ防止

	if (!map.CheckCollision(testY)) {
		// 移動できる
		m_Position.y = tryPosY.y;
		m_onGround = false;
	}
	else {
		// 衝突している -> 上昇 or 下降で補正
		if (m_velY < 0.0) {
			// 上昇中 -> 天井に当たった。下へ補正して止める
			int guard = 0;
			while (map.CheckCollision(testY) && guard++ < guardMax) {
				tryPosY.y += guardStep;
				testY = hurtRectAt(tryPosY);
			}
			m_velY = 0.0;
			m_Position.y = tryPosY.y;
			m_onGround = false;
		}
		else {
			// 下降中 -> 地面に当たった。上へ補正して接地
			int guard = 0;
			while (map.CheckCollision(testY) && guard++ < guardMax) {
				tryPosY.y -= guardStep;
				testY = hurtRectAt(tryPosY);
			}
			m_velY = 0.0;
			m_Position.y = tryPosY.y;
			m_onGround = true;
		}
	}

	// footRect による最終的な接地安定化
	RectF footBox = footRect();
	if (map.CheckCollision(footBox)) {
		m_onGround = true;
	}

	// ----------------------------
	// --- 巡回範囲クリップ
	// ----------------------------
	if (m_Position.x > m_patrolR) { m_Position.x = m_patrolR; m_FaceRight = false; }
	if (m_Position.x < m_patrolL) { m_Position.x = m_patrolL; m_FaceRight = true; }

	// ----------------------------
	// --- 矩形作成（判定用）
	// ----------------------------
	const RectF eBoxHurt = hurtRect();                               // 敵が被弾される矩形（現在位置）
	const RectF eBoxAttack = attackRect();                           // 敵の攻撃矩形（前方オフセット）
	const RectF pHitBox(Arg::center = player.GetPlayerPosition(), player.GetPlayerHitBox()); // プレイヤー本体
	const RectF pAttackBox = player.getAttackRect(); // プレイヤーの攻撃矩形（Player の関数利用）

	// デバッグ：クリックで敵にダメージ
	if (eBoxHurt.leftClicked()) {
		takeDamage(1);
	}

	// --- プレイヤーの攻撃が敵に当たったか ---
	const bool playerAttackingThisFrame = (player.GetPlayerState() == StateMode::Attack) && player.IsPlayerAttacking();
	if (playerAttackingThisFrame && RectToRect(pAttackBox, eBoxHurt)) {
		if (!m_takeDamage) {
			takeDamage(1);
		}
	}

	// --- 敵の攻撃がプレイヤーに当たったか（攻撃中のみ／一度だけヒット） ---
	if (m_state == AnimState::Attack) {
		if (RectToRect(eBoxAttack, pHitBox)) {
			if (!m_hasHitPlayer) {
				player.takeDamage(1);
				m_hasHitPlayer = true;
			}
		}
	}

	// --- 行動決定（被弾 / 攻撃 / 通常） ---
	const bool gotHit = (RectToRect(pAttackBox, eBoxHurt) && playerAttackingThisFrame) || m_takeDamage;

	if (gotHit) {
		if (m_state != AnimState::Hurt) {
			setState(AnimState::Hurt);
			m_Speed = 0.0f;
		}
	}
	else if (m_state == AnimState::Attack) {
		// 攻撃中は動かない（そのまま）
	}
	else if (KeySpace.down() && !AttackFlag) {
		// デバッグ用：スペースで攻撃開始
		setState(AnimState::Attack);
		AttackFlag = true;
		m_Speed = 0.0f;
	}
	else {
		// 通常巡回：vx がゼロでなければ Run
		if (std::abs(vx) > 0.0f) {
			if (m_state != AnimState::Run) setState(AnimState::Run);
		}
		else {
			if (m_state != AnimState::Idle) setState(AnimState::Idle);
		}
	}

	// ----------------------------
	// --- アニメーション更新
	// ----------------------------
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
				// ノンループ終了後の後処理
				if (m_state == AnimState::Hurt) {
					m_takeDamage = false;
					setState(AnimState::Idle);
				}
				else if (m_state == AnimState::Attack) {
					AttackFlag = false;
					m_hasHitPlayer = false;
					setState(AnimState::Idle);
				}
				break;
			}
		}
	}

	// デバッグ用ライン
	Line probe = makeGroundProbeLine();

	// ----------------------------
	// --- デバッグ描画
	// ----------------------------
	if (m_debugDraw) {
		eBoxHurt.drawFrame(2.0, Palette::Red);
		eBoxAttack.drawFrame(2.0, Palette::Blue);
		probe.draw(2, Palette::Yellow);
		footBox.drawFrame(1.5, Palette::Aqua);
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
