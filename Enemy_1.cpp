// Enemy.cpp
#include "Game.hpp"
#include "Enemy_1.hpp"
#include "Collision.hpp"
#include "Player.hpp"
#include "AllEffect.h"
using namespace Collision;


void Enemy_1::die() {// 死亡処理
	if (m_dead) return;
	m_dead = true;
	m_pendingRemoval = false;
	m_attackFlag = false;
	m_hasHitPlayer = false;

	m_Speed = 0.0;
	m_isRunning = false;
	m_velY = 0.0;

	m_frameIndex = 0;
	m_time = 0.0;
}

RectF Enemy_1::hurtRect(const Vec2& cam) const
{
	// 矩形をキャラの中心に合わせる
	const SizeF sz{ m_hitBox.x, m_hitBox.y };
	Vec2 center = m_Position - cam;

	// 横方向の微調整（まずは0で確認）
	// center.x += (m_FaceRight ? 0 : 0);

	// 縦方向も中心に合わせる
	center.y -= 0;

	return RectF{ Arg::center = center, sz };
}


RectF Enemy_1::hurtRectAt(const Vec2& pos) const
{
	const double forwardOffset = m_hitBox.x * 1; // 調整可
	const double xOffset = (m_FaceRight ? +forwardOffset : -forwardOffset);
	const SizeF sz{ m_hitBox.x , m_hitBox.y };
	return RectF{ Arg::center = pos.movedBy(xOffset , m_hitOffsetY), sz };
}


RectF Enemy_1::attackRect(const Game_Map& map) const
{
	const Vec2 cam = map.getCameraPos();

	// 被弾矩形をベースに前方へオフセット（攻撃判定）
	const double forwardOffset = m_hitBox.x * 0.6; // 調整可
	const double xOffset = (m_FaceRight ? +forwardOffset : -forwardOffset);
	const SizeF sz{ m_hitBox.x , m_hitBox.y };
	return RectF{ Arg::center = m_Position.movedBy(xOffset , m_hitOffsetY) - cam, sz };
}

RectF Enemy_1::chaseRect(const Game_Map& map) const// プレイヤー追跡用矩形を作成
{
	const Vec2 cam = map.getCameraPos();
	const double baseW = m_hitBox.x;
	const double baseH = m_hitBox.y;

	const double extraForward = 250.0;// 前方拡張量
	const double lead = m_hitBox.x * -0.5;

	const int dir = (m_FaceRight ? +1 : -1);

	//const double maxF = forwardClearance(map, baseW, baseH, lead, extraForward, dir);
	//const double usedForward = Min(extraForward, maxF);
	const Vec2 worldCenter = m_Position.movedBy(
		dir * (lead + extraForward * 0.5),
		m_hitOffsetY
	);
	const SizeF sz{ baseW + extraForward, baseH };
	return RectF{ Arg::center = (worldCenter - cam), sz };
}

Line Enemy_1::makeGroundProbeLine(const Vec2& cam, bool debug) const
{
	// 地面探査用の線分を作成
	const double fwd = 1.3 * m_hitBox.x;
	const double down = 1.0 * m_hitBox.y;
	const Vec2 dir = (m_FaceRight ? Vec2{ +fwd, +down } : Vec2{ -fwd, +down });
	if (debug)
	{
		return Line{ m_Position - cam, m_Position - cam + dir };
	}
	else
	{
		return Line{ m_Position, m_Position + dir };
	}
}

// プレイヤーへの視線が通っているかを判定
static bool hasLineOfSight(const Enemy_1& self, Game_Map& map, const RectF& playerBox)
{
	const Vec2 from = self.getPosition();
	const Vec2 to = playerBox.center();
	Vec2 dir = to - from;
	const double dist = dir.length();
	if (dist <= 0.0001) {
		return true;
	}
	dir /= dist;


	const double step = 6.0;// 探査ステップ
	const SizeF  probeSize{ 8, 8 };// 视线探测用矩形サイズ
	for (double t = 0.0; t <= dist; t += step) {
		const Vec2 p = from + dir * t;// 探査点

		if (playerBox.intersects(Circle{ p, 3.0 })) {// プレイヤーに到達
			return true;
		}

		if (map.CheckCollision_RecF(RectF{ Arg::center = p, probeSize })) {// 障害物に当たった
			return false;
		}
	}

	return true;
}



void Enemy_1::update(Player& player, Game_Map& map,AllEffect& alleffe)
{
	const double dt = Scene::DeltaTime() * TimeStopManager::GetEnemyScale();
	const Vec2 cam = map.getCameraPos() * TimeStopManager::GetEnemyScale();

	m_faceFlipCooldown = Max(0.0, m_faceFlipCooldown - dt);
	m_attackCooldown = Max(0.0, m_attackCooldown - dt);

	const RectF eHurtBox = hurtRect(cam);                               // 敵が被弾される矩形（現在位置）
	const RectF eAttackBox = attackRect(map);                           // 敵の攻撃矩形（前方オフセット）
	const RectF eChaseBox = chaseRect(map);// プレイヤー追跡矩形（広域前方オフセット）
	const Line  eGroundProbeLine = makeGroundProbeLine(cam, false); // 敵の地面探査用線分
	const RectF pHitBox(Arg::center = player.GetPlayerPosition() - cam, player.GetPlayerHitBox()); // プレイヤー本体
	const RectF pAttackBox = player.getAttackRect(cam); // プレイヤーの攻撃矩形（Player の関数利用）


	const bool playerInAttack = RectToRect(eAttackBox, pHitBox);// プレイヤーが攻撃矩形内にいるか
	const bool groundAhead = map.CheckCollision_Line(eGroundProbeLine);// 敵の地面が前方にあるか


	const bool playerInChase = RectToRect(eChaseBox, pHitBox);
	const RectF pHitBoxWorld(Arg::center = player.GetPlayerPosition(), player.GetPlayerHitBox());
	const bool playerInChaseWall = playerInChase
		&& hasLineOfSight(*this, map, pHitBoxWorld);// プレイヤーが追跡矩形内にいるか（粗判定+视线判定）

	
	if (playerInChase) {// プレイヤーが追跡矩形内にいるなら交戦状態に移行
		if (!m_engaged) {
			constexpr double spotCooldown = 1;
			m_attackCooldown = Max(m_attackCooldown, spotCooldown);
		}
		m_engaged = true;
		m_yLoseTimer = 0.0;
	}
	else {// プレイヤーが追跡矩形外にいるなら縦軸判定
		const double dy = Abs(player.GetPlayerPosition().y - m_Position.y);
		if (dy >= m_ySepThreshold)  m_yLoseTimer += dt;
		else m_yLoseTimer = 0.0;
	}


	//const bool playerDead = player.IsDead ? player.IsDead() : false;
	//if (playerDead) {// プレイヤーが死亡しているなら脱戦
	//	m_engaged = false;
	//	m_yLoseTimer = 0.0;
	//}

	if (m_engaged && (m_yLoseTimer >= m_yLoseThresholdSec)) {
		m_engaged = false;
		m_yLoseTimer = 0.0;
	}

	auto updateFacingStable = [&]() {// 安定向き更新
		const double dx = player.GetPlayerPosition().x - m_Position.x;
		const double dy = player.GetPlayerPosition().y - m_Position.y;

		if (m_faceFlipCooldown > 0.0) return;
		if (Abs(dy) > m_yFacingGate)   return;

		if (dx > m_flipThreshold && !m_FaceRight) { m_FaceRight = true;  m_faceFlipCooldown = m_faceFlipCooldownMax; }
		else if (dx < -m_flipThreshold && m_FaceRight) { m_FaceRight = false; m_faceFlipCooldown = m_faceFlipCooldownMax; }
		};



	if (m_dead || (m_state == AnimState_Enemy1::Dead)) {// 死亡状態
		setState(AnimState_Enemy1::Dead);
		m_Speed = 0.0;
		m_isRunning = false;
		m_velY = 0.0;
		m_onGround = true;


		{// 死亡時飛び出し
			m_FaceRight = (player.GetPlayerPosition().x >= m_Position.x);
			const double base = (m_speedBase > 0 ? m_speedBase : 150.0);
			m_Speed = 100;

			double remaining = m_Speed * dt;
			const double unit = 2.0;
			int safety = 0;
			m_isRunning = false;

			while (remaining > 0.0 && safety++ < 400)
			{
				const double step = Min(remaining, unit);
				Vec2 probe = m_Position;
				probe.x += (m_FaceRight ? -step : +step);
				m_Position.x = probe.x;
				remaining -= step;
			}
		}


		const auto& A = m_anims[m_state];
		m_time += dt;
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
					m_pendingRemoval = true;
				}
			}
		}
		return;
	}
	else if (m_state == AnimState_Enemy1::Attack) {
		m_Speed = 0.0;
		m_isRunning = false;
		updateFacingStable();

	}
	else {// 通常行動状態

		if (m_engaged) {// 交戦モード
			if (playerInAttack && (m_attackCooldown <= 0.0) && m_onGround && player.GetPlayerState() != StateMode::Dead) {
				m_mode = Behavior_Enemy1::Attack;
				setState(AnimState_Enemy1::Attack);
				m_attackFlag = true;
				m_hasHitPlayer = false;
				m_Speed = 0.0;
				m_hasHitPlayer = false; // ←ここでリセット！
				updateFacingStable();
			}
			else {// 追跡モード
				m_mode = Behavior_Enemy1::Chase;
				updateFacingStable();
				if (!groundAhead || playerInAttack) {
					m_Speed = 0.0;
					m_isRunning = false;
				}
				else {// プレイヤーに向かって移動
					m_Speed = (m_speedBase > 0 ? m_speedBase : 150.0);
					double remaining = m_Speed * dt;
					const double unit = 2.0;
					int safety = 0;
					m_isRunning = false;

					while (remaining > 0.0 && safety++ < 400) {
						const double step = Min(remaining, unit);
						Vec2 probe = m_Position;
						probe.x += (m_FaceRight ? +step : -step);
						RectF box = hurtRectAt(probe);
						if (!map.CheckCollision_RecF(box)) {
							m_Position.x = probe.x;
							remaining -= step;
							m_isRunning = true;
						}
						else {
							int fix = 0; while (map.CheckCollision_RecF(box) && fix++ < 32) {
								probe.x -= (m_FaceRight ? +0.5 : -0.5);
								box = hurtRectAt(probe);
							}
							m_Position.x = probe.x;

							remaining = 0.0;
						}
					}
				}
			}
		}
		else { // 巡回モード

			m_mode = Behavior_Enemy1::Patrol;
			if (!groundAhead) {
				if (m_onGround) {
					m_FaceRight = !m_FaceRight;
				}
				m_isRunning = false;
			}


			if (m_phaseTimer <= 0.0) enterPhase(RandomBool() ? PatrolPhase_Enemy1::Move : PatrolPhase_Enemy1::Wait);
			else {// フェーズタイマー減少
				m_phaseTimer -= dt;
				if (m_phaseTimer <= 0.0) enterPhase(m_phase == PatrolPhase_Enemy1::Move ? PatrolPhase_Enemy1::Wait : PatrolPhase_Enemy1::Move);
			}

			m_isRunning = false;

			if (m_phase == PatrolPhase_Enemy1::Move) {// 移動フェーズ
				m_Speed = (m_speedBase > 0 ? m_speedBase : 150.0);
				double remaining = m_Speed * dt;
				const double unit = 2.0;
				int safety = 0;

				while (remaining > 0.0 && safety++ < 400) {
					const double step = Min({ remaining, m_budget, unit });
					Vec2 probe = m_Position;
					probe.x += (m_FaceRight ? +step : -step);
					RectF box = hurtRectAt(probe);
					if (!map.CheckCollision_RecF(box)) {
						m_Position.x = probe.x;
						remaining -= step;
						m_budget -= step;
						m_isRunning = true;
						if (m_budget <= 0.0) {
							m_FaceRight = !m_FaceRight;
							m_budget = m_strideRandom ? Random(m_strideMin, m_strideMax) : m_stride;
						}
					}
					else {
						int fix = 0; while (map.CheckCollision_RecF(box) && fix++ < 32) {
							probe.x -= (m_FaceRight ? +0.5 : -0.5);
							box = hurtRectAt(probe);
						}
						m_Position.x = probe.x;

						m_FaceRight = !m_FaceRight;
						m_budget = m_strideRandom ? Random(m_strideMin, m_strideMax) : m_stride;
						remaining -= step;
					}
				}
			}
			else {// 待機フェーズ
				m_Speed = 0.0;
				m_isRunning = false;
			}
		}
	}
	// ----------------------------
	// --- 重力＆縦移動（Y）処理
	// ----------------------------
	m_velY += m_gravity * dt;
	Vec2 tryPosY = m_Position;
	tryPosY.y += m_velY * dt;

	RectF testY = hurtRectAt(tryPosY);

	if (!map.CheckCollision_RecF(testY)) {
		// 移動できる
		m_Position.y = tryPosY.y;
		m_onGround = false;
	}
	else {
		const double guardStep = 0.5;
		const int guardMax = 400;

		// 衝突している -> 上昇 or 下降で補正
		if (m_velY < 0.0) {
			// 上昇中 -> 天井に当たった。下へ補正して止める
			int guard = 0;
			while (map.CheckCollision_RecF(testY) && guard++ < guardMax) {
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
			while (map.CheckCollision_RecF(testY) && guard++ < guardMax) {
				tryPosY.y -= guardStep;
				testY = hurtRectAt(tryPosY);
			}
			m_velY = 0.0;
			m_Position.y = tryPosY.y;
			m_onGround = true;
		}
	}
	// footRect による最終的な接地安定化
	{
		const double eps = 1.5;
		RectF footProbe = hurtRectAt(m_Position).movedBy(0, eps);
		m_onGround = m_onGround || map.CheckCollision_RecF(footProbe);
	}


	// デバッグ：クリックで敵にダメージ
	if (eHurtBox.leftClicked()) {
		die();
	}

	// --- プレイヤーの攻撃が敵に当たったか ---
	const bool playerAttackingThisFrame =
		((player.GetPlayerState() == StateMode::Attack) ||
		(player.GetPlayerState() == StateMode::IdleToAttack) ||
		(player.GetPlayerState() == StateMode::JumpAttack) ||
		player.IsPlayerAttacking());
	// ---  ---
	bool parryOccurred =
		(m_state == AnimState_Enemy1::Attack) &&                 // 敵が攻撃中
		(player.GetPlayerState() == StateMode::Attack) &&        // プレイヤーも攻撃中
		RectToRect(eAttackBox, pAttackBox);                      // 攻撃範囲が重なった

	if (parryOccurred)
	{
		// パリィが成立したら攻撃をキャンセル
		m_attackFlag = false;
		m_hasHitPlayer = false;
		m_attackCooldown = m_attackCooldownMax;
		setState(AnimState_Enemy1::Idle);

		// プレイヤー側にもパリィ処理を伝える（反動・エフェクトなど）
		player.OnParrySuccess();  // Playerクラスに関数を作っておくと良い

		
		//AudioAsset(U"ParrySound").play();

		return; // このフレームは以降の処理をスキップ
	}

	// --- 行動決定（被弾 / 攻撃 / 通常） ---
	const bool gotHit = (RectToRect(pAttackBox, eHurtBox) && playerAttackingThisFrame ) || m_takeDamage;
	if (gotHit) {//
		alleffe.SetEffect(getPosition(), Vec2{1.0,0.3},0.4,player.IsPlayerFacingRight());
		const Audio& AS1 = AudioAsset(U"Sowrd1");
		const Audio& AS2 = AudioAsset(U"Sowrd2");
		const Audio& AS3 = AudioAsset(U"Sowrd3");
		const Audio& AS4 = AudioAsset(U"Sowrd4");
		AS1.stop();
		AS2.stop();
		AS3.stop();
		AS4.stop();
		int a = Random(0, 3);
		a = 1;
		switch (a)
		{

		case 0:
			AS1.play();
			break;

		case 1:
			AS2.play();
			break;
		case 2:
			AS3.play();
			break;
		case 3:
			AS4.play();
			break;
		default:
			AS4.play();
			break;
		}
		die();
	}
	else if (m_state == AnimState_Enemy1::Attack) {
		// 攻撃中は動かない（そのまま）
	}
	else {
		// 通常巡回：vx がゼロでなければ Run
		if (m_isRunning) {
			if (m_state != AnimState_Enemy1::Run) setState(AnimState_Enemy1::Run);
		}
		else {
			if (m_state != AnimState_Enemy1::Idle) setState(AnimState_Enemy1::Idle);
		}
	}

	// ----------------------------
	// --- アニメーション更新
	// ----------------------------d


	const auto& A = m_anims[m_state];
	m_time += dt;
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
				if (m_state == AnimState_Enemy1::Dead) {
					m_pendingRemoval = true;
				}
				else if (m_state == AnimState_Enemy1::Attack) {// 攻撃アニメーション終了
					m_attackFlag = false;
					m_hasHitPlayer = false;
					m_attackCooldown = m_attackCooldownMax;
					setState(AnimState_Enemy1::Idle);
				}
				break;
			}
		}
	}

	if (m_state == AnimState_Enemy1::Attack)
	{
		m_hitWindowActive = (m_frameIndex >= 1 && m_frameIndex <= 2);
		if (m_hitWindowActive && !m_hasHitPlayer)
		{
			if (RectToRect(eAttackBox, pHitBox)) {
				player.takeDamage(1, m_FaceRight);
				m_hasHitPlayer = true;
			}
		}
	}

	if (m_state != AnimState_Enemy1::Attack && m_state != AnimState_Enemy1::Dead) {// 攻撃中・被弾中以外は状態を速度に応じて更新
		setState(m_isRunning ? AnimState_Enemy1::Run : AnimState_Enemy1::Idle);
	}

	const bool inChase = playerInChase;
	const bool engagedNow = m_engaged;

	if (!textallowLose && inChase && !textChase) {
		text.trigger(U'!');
		textallowLose = true;
	}
	if (textallowLose && m_mode == Behavior_Enemy1::Patrol && !textLoseCounting) {
		text.trigger(U'?');
		textallowLose = false;
	}



	text.update(false, dt);
	textChase = inChase;
	textLoseCounting = engagedNow;
}

void Enemy_1::draw(const Game_Map& map) const
{
	constexpr int ATLAS_COLS = 5;
	constexpr int ATLAS_ROWS = 5;
	const Texture& atlas = TextureAsset(U"Enemy1");
	const Size c = { atlas.width() / ATLAS_COLS, atlas.height() / ATLAS_ROWS };


	const auto& R = m_anims.at(m_state);
	int linear = R.start + m_frameIndex;
	int row = R.row + linear / ATLAS_COLS;
	int col = linear % ATLAS_COLS;

	row = Clamp(row, 0, ATLAS_ROWS - 1);
	col = Clamp(col, 0, ATLAS_COLS - 1);

	const int32 sx = col * c.x;
	const int32 sy = row * c.y;
	const auto  reg = atlas(sx, sy, c);

	double sxScale = m_Scale.x / c.x;
	double syScale = m_Scale.y / c.y;

	Vec2 center = m_Position - map.getCameraPos();
	const double visualH = c.y * syScale;
	center.y -= (visualH * 0.5 - m_hitBox.y * 0.59) - m_hitOffsetY;

	(m_FaceRight ? reg : reg.mirrored())
		.scaled(sxScale, syScale)
		.drawAt(center);

	text.draw(m_Position, m_FaceRight, map.getCameraPos(), ColorF{ 1.0 }, 32, Vec2(17, 90));
	// ----------------------------
	// --- デバッグ描画
	// ----------------------------
	if (m_debugDraw)
	{
		hurtRect(map.getCameraPos()).drawFrame(2.0, Palette::Red);
		attackRect(map).drawFrame(2.0, Palette::Blue);
		chaseRect(map).drawFrame(2.0, Palette::White);
		makeGroundProbeLine(map.getCameraPos(), true).draw(2, Palette::Yellow);
	}
}

