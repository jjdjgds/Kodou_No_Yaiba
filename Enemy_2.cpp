// Enemy.cpp
#include "Game.hpp"
#include "Enemy_2.hpp"
#include "Collision.hpp"
#include "Player.hpp"

using namespace Collision;





void Enemy_2::takeDamage(int damage)// ダメージを受けたときの処理
{
	if (!m_takeDamage) {
		m_takeDamage = true;
		m_frameIndex = 0;
		m_time = 0.0;
	}
}

RectF Enemy_2::hurtRect(const Vec2& cam) const
{
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = m_Position.movedBy(0-cam.x, m_hitOffsetY * m_Scale.y- cam.y), sz};
}

RectF Enemy_2::hurtRectAt(const Vec2& pos) const
{
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = pos.movedBy(0 , m_hitOffsetY * m_Scale.y ), sz };
}

RectF Enemy_2::attackRect(const Vec2& cam) const
{
	// 被弾矩形をベースに前方へオフセット（攻撃判定）
	const double forwardOffset = m_hitBox.x * 1 * m_Scale.x; // 調整可
	const double xOffset = (m_FaceRight ? +forwardOffset : -forwardOffset);
	const SizeF sz{ m_hitBox.x * m_Scale.x, m_hitBox.y * m_Scale.y };
	return RectF{ Arg::center = m_Position.movedBy(xOffset - cam.x, m_hitOffsetY * m_Scale.y - cam.y), sz };
}

RectF Enemy_2::chaseRect(const Vec2& cam) const
{
	const double baseW = m_hitBox.x * m_Scale.x;
	const double baseH = m_hitBox.y * m_Scale.y;

	const double extraForward = 350.0;                       // 只向前多出的长度（始终为正）
	const double lead = m_hitBox.x * - 0.5 * m_Scale.x; // 前移基准（原先的 forwardOffset）

	const int dir = (m_FaceRight ? +1 : -1);

	// 新的中心：在原中心基础上，先前移 lead，再额外前移 extraForward/2
	const Vec2 worldCenter = m_Position.movedBy(
		dir * (lead + extraForward * 0.5),
		m_hitOffsetY * m_Scale.y
	);

	// 新的尺寸：宽度 = 基础宽度 + extraForward（永远为正）
	const SizeF sz{ baseW + extraForward, baseH };

	// 仍按你当前做法：函数内减相机
	return RectF{ Arg::center = (worldCenter - cam), sz };
}

Line Enemy_2::makeGroundProbeLine(const Vec2& cam) const
{
	// 地面探査用の線分を作成
	const double fwd = 17 * m_Scale.x;
	const double down = 40.0 * m_Scale.y;
	const Vec2 dir = (m_FaceRight ? Vec2{ +fwd, +down } : Vec2{ -fwd, +down });
	return Line{ m_Position-cam, m_Position-cam + dir };
}


void Enemy_2::update(Player& player, Game_Map& map)
{
	if (!TextureAsset::IsRegistered(U"Enemy2")) {
		TextureAsset::Register(U"Enemy2", U"example/spritesheet/Soldier.png");
	}

	const double dt = Scene::DeltaTime();
	const Vec2 camPos = map.getCameraPos();

	if (m_attackCooldown > 0.0) m_attackCooldown = Max(0.0, m_attackCooldown - dt);// 攻撃クールダウン減少

	const bool inBlockedState = (m_state == AnimState_Enemy2::Hurt) || (m_state == AnimState_Enemy2::Attack);// ダメージ中or攻撃中は行動制限

	const RectF eHurtBox= hurtRect(camPos);                               // 敵が被弾される矩形（現在位置）
	const RectF eAttackBox = attackRect(camPos);                           // 敵の攻撃矩形（前方オフセット）
	const RectF eChaseBox = chaseRect(camPos);             // 敵のプレイヤー攻撃矩形（広域前方オフセット）
	const RectF pHitBox(Arg::center = player.GetPlayerPosition()-camPos, player.GetPlayerHitBox()); // プレイヤー本体
	const RectF pAttackBox = player.getAttackRect(camPos); // プレイヤーの攻撃矩形（Player の関数利用）

	if (m_state == AnimState_Enemy2::Hurt) {// ダメージ中
		m_mode = Behavior_Enemy2::Patrol;
	}
	else if (m_state == AnimState_Enemy2::Attack) {// 攻撃中
		m_mode = Behavior_Enemy2::Attack;
	}
	else {// 通常状態
		const bool canAttack = RectToRect(eAttackBox, pHitBox) && (m_attackCooldown <= 0.0) && m_onGround;// 攻撃可能か
		if (canAttack) {// 攻撃範囲内なら攻撃モードへ
			m_mode = Behavior_Enemy2::Attack;
			setState(AnimState_Enemy2::Attack);
			AttackFlag = true;
			m_hasHitPlayer = false;
			m_Speed = 0.0;
			m_FaceRight = (player.GetPlayerPosition().x >= m_Position.x);
		}
		else if (RectToRect(eChaseBox, pHitBox)) {// 追跡範囲内なら追跡モードへ
			m_mode = Behavior_Enemy2::Chase;
		}
		else {// それ以外は巡回モードへ
			m_mode = Behavior_Enemy2::Patrol;
		}
	}

	if (m_mode == Behavior_Enemy2::Attack) {// 攻撃モード
		m_Speed = 0.0;
		isRuning = false;
		if (m_state == AnimState_Enemy2::Attack) {
			// 攻撃アニメーション中
		}
	}
	else if (m_mode == Behavior_Enemy2::Chase) {// 追跡モード
		m_Speed = (m_speedBase > 0 ? m_speedBase : 150.0);// 恢复正常速度
		double remaining = m_Speed * dt;// 移動可能距離
		const double unit = 2.0;// 移動単位
		int safety = 0;// 無限ループ防止用カウンタ
		isRuning = false;// 走っているかどうか

		while (remaining > 0.0 && safety++ < 400) {// 移動可能距離が残っている限り移動処理
			m_FaceRight = (player.GetPlayerPosition().x >= m_Position.x);// プレイヤーの位置に向く
			const double step = Min(remaining, unit);// 今回の移動ステップ

			Vec2 probe = m_Position;
			probe.x += (m_FaceRight ? +step : -step);

			RectF box = hurtRectAt(probe);
			if (!map.CheckCollision(box)) {
				m_Position.x = probe.x;
				remaining -= step;
				isRuning = true;
			}
			else {
				int fix = 0; while (map.CheckCollision(box) && fix++ < 32) {
					probe.x -= (m_FaceRight ? +0.5 : -0.5);
					box = hurtRectAt(probe);
				}
				m_Position.x = probe.x;
				m_FaceRight = !m_FaceRight;
			}
		}
	}
	else { // 巡回モード
		if (m_phaseTimer <= 0.0) enterPhase(RandomBool() ? PatrolPhase_Enemy2::Move : PatrolPhase_Enemy2::Wait);
		else {// フェーズタイマー減少
			m_phaseTimer -= dt;
			if (m_phaseTimer <= 0.0) enterPhase(m_phase == PatrolPhase_Enemy2::Move ? PatrolPhase_Enemy2::Wait : PatrolPhase_Enemy2::Move);
		}

		isRuning = false;

		if (m_phase == PatrolPhase_Enemy2::Move) {// 移動フェーズ
			m_Speed = (m_speedBase > 0 ? m_speedBase : 150.0);
			double remaining = m_Speed * dt;
			const double unit = 2.0;
			int safety = 0;

			while (remaining > 0.0 && safety++ < 400) {
				const double step = Min({ remaining, m_budget, unit });

				Vec2 probe = m_Position;
				probe.x += (m_FaceRight ? +step : -step);

				RectF box = hurtRectAt(probe);
				if (!map.CheckCollision(box)) {
					m_Position.x = probe.x;
					remaining -= step;
					m_budget -= step;
					isRuning = true;

					if (m_budget <= 0.0) {
						m_FaceRight = !m_FaceRight;
						m_budget = m_strideRandom ? Random(m_strideMin, m_strideMax) : m_stride;
					}
				}
				else {
					int fix = 0; while (map.CheckCollision(box) && fix++ < 32) {
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
			isRuning = false;
		}
	}

	// ----------------------------
	// --- 重力＆縦移動（Y）処理
	// ----------------------------
	m_velY += m_gravity * dt;
	Vec2 tryPosY = m_Position;
	tryPosY.y += m_velY * dt;

	RectF testY = hurtRectAt(tryPosY);

	if (!map.CheckCollision(testY)) {
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
	{
		const double eps = 1.5;
		RectF footProbe = hurtRectAt(m_Position).movedBy(0, eps);
		m_onGround = m_onGround || map.CheckCollision(footProbe);
	}


	

	// デバッグ：クリックで敵にダメージ
	if (eHurtBox.leftClicked()) {
		takeDamage(1);
		Print << U"pos" << m_Position;
	}

	// --- プレイヤーの攻撃が敵に当たったか ---
	const bool playerAttackingThisFrame = (player.GetPlayerState() == StateMode::Attack) && player.IsPlayerAttacking();
	if (playerAttackingThisFrame && RectToRect(pAttackBox, eHurtBox)) {
		if (!m_takeDamage) {
			takeDamage(1);
		}
	}

	// --- 行動決定（被弾 / 攻撃 / 通常） ---
	const bool gotHit = (RectToRect(pAttackBox, eHurtBox) && playerAttackingThisFrame) || m_takeDamage;

	if (gotHit) {//
		if (m_state != AnimState_Enemy2::Hurt) {
			setState(AnimState_Enemy2::Hurt);
			m_Speed = 0.0f;
		}
	}
	else if (m_state == AnimState_Enemy2::Attack) {
		// 攻撃中は動かない（そのまま）
	}
	else if (KeySpace.down() && !AttackFlag) {
		// デバッグ用：スペースで攻撃開始
		setState(AnimState_Enemy2::Attack);
		AttackFlag = true;
		m_Speed = 0.0f;
	}
	else {
		// 通常巡回：vx がゼロでなければ Run
		if (isRuning) {
			if (m_state != AnimState_Enemy2::Run) setState(AnimState_Enemy2::Run);
		}
		else {
			if (m_state != AnimState_Enemy2::Idle) setState(AnimState_Enemy2::Idle);
		}
	}

	// ----------------------------
	// --- アニメーション更新
	// ----------------------------
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
				if (m_state == AnimState_Enemy2::Hurt) {
					m_takeDamage = false;
				}
				else if (m_state == AnimState_Enemy2::Attack) {// 攻撃アニメーション終了
					if (!m_hasHitPlayer && RectToRect(eAttackBox, pHitBox)) {
						player.takeDamage(1);
						m_hasHitPlayer = true;
					}

					AttackFlag = false;
					m_hasHitPlayer = false;
					m_attackCooldown = m_attackCooldownMax;
				}
				setState(AnimState_Enemy2::Idle);
				break;
			}
		}
	}

	if (m_state != AnimState_Enemy2::Attack && m_state != AnimState_Enemy2::Hurt) {// 攻撃中・被弾中以外は状態を速度に応じて更新
		setState(isRuning ? AnimState_Enemy2::Run : AnimState_Enemy2::Idle);
	}
}

void Enemy_2::draw(const Game_Map& map) const
{
	constexpr int ATLAS_COLS = 9;
	constexpr int ATLAS_ROWS = 7;
	const Texture& atlas = TextureAsset(U"Enemy2");
	const Size c= { atlas.width() / ATLAS_COLS, atlas.height() / ATLAS_ROWS };

	const auto& R = m_anims.at(m_state);

	int col = R.start + m_frameIndex;
	int row = R.row;

	const int32 sx = col * c.x;
	const int32 sy = row * c.y;
	const auto  reg = atlas(sx, sy, c);

	(m_FaceRight ? reg : reg.mirrored())
		.scaled(m_Scale.x, m_Scale.y)
		.drawAt(m_Position - map.getCameraPos());

	// ----------------------------
	// --- デバッグ描画
	// ----------------------------
	if (m_debugDraw) {
		hurtRect(map.getCameraPos()).drawFrame(2.0, Palette::Red);
		attackRect(map.getCameraPos()).drawFrame(2.0, Palette::Blue);
		chaseRect(map.getCameraPos()).drawFrame(2.0, Palette::White);
		makeGroundProbeLine(map.getCameraPos()).draw(2, Palette::Yellow);
		//footBox.drawFrame(1.5, Palette::Aqua);
	}
}

