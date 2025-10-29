#include <Siv3D.hpp>
#include "Player.hpp"
#include "Game.hpp"
#include "Collision.hpp"
#include "Enemy.hpp"
using namespace Collision;

RectF enemyRect{ 0, 0, 0, 0 };
/******************************************
//
//プレイヤーの移動量によって変化？
//プレイヤーの行動によって変化？
//
//
*****************************************/

Player::~Player() {}
HeartRateState Player::GetHeartRateState(int bpm)
{
	if (bpm <= 60 || bpm >= 140)
		return HeartRateState::Stun;

	if ((bpm >= 61 && bpm <= 70) || (bpm >= 130 && bpm <= 139))
		return HeartRateState::Warning;

	if (bpm >= 120 && bpm <= 129)
		return HeartRateState::Berserk;

	if (bpm >= 71 && bpm <= 80)
		return HeartRateState::TimeControl;

	if(bpm==0)
	{
		return HeartRateState::Dead;
	}


	return HeartRateState::Normal;
}

RectF Player::getAttackRect(const Vec2& camera) const
{
	// === プレイヤーの当たり判定サイズを基準にする ===
	const SizeF hitSize = GetPlayerHitBox() ;
	const double attackWidth = hitSize.x*12 ; // 攻撃範囲を少し広げる
	const double attackHeight = hitSize.y*10 ; // 高さはプレイヤーと同じ
	const SizeF attackSize{ attackWidth, attackHeight  };

	// === 基準点（プレイヤーの中心） ===
	Vec2 center = GetPlayerPosition().movedBy(-camera);

	// === 向きによって左右に矩形をオフセット ===
	const double offsetX = (IsPlayerFacingRight() ? +hitSize.x * 0.6+50 : -hitSize.x * 0.6-50);
	center.x += offsetX;

	// === 少し上にオフセットして、胸〜腰あたりの高さに ===
	center.y -= hitSize.y+30 ;

	return RectF{
		Arg::center = center,
		attackSize
	};
}


RectF Player::getHitRect(const Vec2& camera) const
{
	// === 実際の当たり判定サイズ（スケール反映） ===
	const SizeF sz = {
		m_HitBox.x * m_Scale.x/10,
		m_HitBox.y * m_Scale.y/10
	};

	// === 中心をスプライトと一致させる（体中心基準） ===
	// m_Position がキャラ中心座標なのでそのまま使用
	const Vec2 center = m_Position
		.movedBy(-camera + Vec2{0,-40}); // カメラ補正

	return RectF{
		Arg::center = center,
		sz
	};
}

RectF Player::getHitRectWorld() const
{
	const SizeF sz = {
		m_HitBox.x * m_Scale.x / 10,
		m_HitBox.y * m_Scale.y / 10
	};

	const Vec2 center = m_Position + Vec2{ 0, -40 };

	return RectF{
		Arg::center = center,
		sz
	};
}




void Player::UpdateHeartState()
{
	auto bpm = GetPlayerBPM();
	if (bpm == 0)
		m_HeartRateState = HeartRateState::Dead;
	else if (bpm <= 60 || bpm >= 140)
		m_HeartRateState = HeartRateState::Stun;
	else if ((bpm >= 61 && bpm <= 70) || (bpm >= 130 && bpm <= 139))
		m_HeartRateState = HeartRateState::Warning;
	else if (bpm >= 120 && bpm <= 129)
		m_HeartRateState = HeartRateState::Berserk;
	else if (bpm >= 71 && bpm <= 80)
		m_HeartRateState = HeartRateState::TimeControl;
	
	else
		m_HeartRateState = HeartRateState::Normal;
}

void Player::takeDamage(int dmg)
{
	// ★無敵時は無視
	if (m_IsInvincible)
		return;

	if (GetPlayerState() == StateMode::Hurt || GetPlayerState() == StateMode::Doge)
		return;

	SetPlayerState(StateMode::Hurt);
	SetPlayerHP(GetPlayerHP() - dmg);

	SetPlayerBPM(GetPlayerBPM() - 8);
	m_HeartTimer = 0.0;
	m_HeartCoolTimer = m_HeartCooldown;
	m_HeartCoolFlg = true;
}

// ワールド座標での当たり判定取得（カメラ補正なし）
RectF Player::getAttackRectWorld() const
{
	const SizeF hitSize = GetPlayerHitBox();
	const double attackWidth = hitSize.x * 12;
	const double attackHeight = hitSize.y * 10;
	const SizeF attackSize{ attackWidth, attackHeight };

	// ★★★ カメラ補正なし ★★★
	Vec2 center = GetPlayerPosition();

	const double offsetX = (IsPlayerFacingRight() ? +hitSize.x * 0.6 + 50 : -hitSize.x * 0.6 - 50);
	center.x += offsetX;
	center.y -= hitSize.y + 30;

	return RectF{
		Arg::center = center,
		attackSize
	};
}

// ============================================
// Player.cpp の修正部分
// ============================================

void Player::PlayerAttack(const Vec2& camera, Array<Enemy>& m_enemies)
{
	double attackFrameDuration = ATTACKSPEED;

	// バーサーク中は攻撃速度上昇
	if (m_BersarkFlg)
	{
		attackFrameDuration /= m_AttackSpeedBoost;
	}

	if (!m_AttackFlag) return;

	if (animTime >= attackFrameDuration)
	{
		animTime -= attackFrameDuration;
		m_frameIndex++;

		// 攻撃判定フレーム（3〜5）
		if (m_frameIndex >= 3 && m_frameIndex <= 5)
		{
			//  カメラ座標を渡さない（ワールド座標で判定） 
			const RectF pBox = getAttackRect(Vec2{ 0, 0 });

			for (auto& e : m_enemies)
			{
				//  敵もワールド座標で取得 
				RectF eBox = e.hurtRect(Vec2{ 0, 0 });

				if (RectToRect(pBox, eBox))
				{
					Print << U"攻撃ヒット！";
					e.takeDamage(10);
				}
			}
		}

		// 攻撃アニメ終了
		if (m_frameIndex >= m_attackPatterns.size())
		{
			m_frameIndex = 0;
			SetPlayerAttackFlag(false);

			// ★ ここが重要！ 攻撃後の状態を決める

			if (KeyA.pressed() || KeyD.pressed())
			{
				SetPlayerState(StateMode::Run);
			}
			else
			{
				SetPlayerState(StateMode::Idle);
			}
			m_HeartTimer = 0.0;
		}
	}
}
void Player::PlayerIdle()
{
	const double idleFrameDuration = 0.15;
	if (animTime >= idleFrameDuration)
	{
		animTime -= idleFrameDuration;
		m_frameIndex = (m_frameIndex + 1) % m_idlePatterns.size();
	}
}

void Player::PlayerDoge()
{
	// Dodge中の速度変更
	if (m_DogeTimer == 0.0)
	{
		SetPlayerSpeed(DogePlayerSpeed);
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// アニメ更新
	animTime += Scene::DeltaTime();
	const double dogeFrameDuration = 0.08;
	if (animTime >= dogeFrameDuration)
	{
		animTime -= dogeFrameDuration;
		m_frameIndex = (m_frameIndex + 1) % m_dogePatterns.size();
	}

	// 経過時間
	m_DogeTimer += Scene::DeltaTime();
	const double dogeDuration = 0.2; // Dodge継続時間

	if (m_DogeTimer >= dogeDuration)
	{
		// Dodge終了
		SetPlayerVelocity(Vec2(0, GetPlayerVelocity().y));
		SetPlayerSpeed(NormalPlayerSpeed);

		if (KeyA.pressed() || KeyD.pressed())
		{
			SetPlayerState(StateMode::Run);
		}
		else
		{
			SetPlayerState(StateMode::Idle);
		}

		m_isDodging = false;
		m_DogeTimer = 0.0;
	}
}



void Player::PlayerHurt()
{
	const double hurtFrameDuration = 0.3;
	if (animTime >= hurtFrameDuration)
	{
		animTime -= hurtFrameDuration;
		m_frameIndex++;

		if (m_frameIndex >= m_hurtPatterns.size())
		{
			m_frameIndex = 0;

			// 一旦 Idle に固定（次のフレームで入力による遷移）
			SetPlayerState(StateMode::Idle);
			animTime = 0.0;

			// HPが0ならDeadへ
			if (GetPlayerHP() <= 0)
			{
				SetPlayerState(StateMode::Dead);
			}
			return;
		}
	}
}

void Player::PlayerJumpAttack()
{


	const double JumpattackFrameDuration = 0.08;

	if (!m_AttackFlag) return;

	if (animTime >= JumpattackFrameDuration)
	{
		animTime -= JumpattackFrameDuration;
		m_frameIndex++;

		// 攻撃判定のフレームを限定（例：3〜5フレーム目でヒット）
		if (m_frameIndex >= 3 && m_frameIndex <= 5)
		{
			const RectF pBox = getAttackRect(Vec2{});
			if (RectToRect(pBox, enemyRect))
			{
				Print << U"攻撃ヒット！";
				// 敵のダメージ処理をここに追加予定
			}
		}

		// 攻撃アニメ終了
		if (m_frameIndex >= m_jumpAttackPatterns.size())
		{
			m_frameIndex = 0;
			SetPlayerAttackFlag(false);




			// ★ ここが重要！ 攻撃後の状態を決める
			if (KeyA.pressed() || KeyD.pressed())
			{
				// まだ移動キーが押されている → Runへ
				SetPlayerState(StateMode::Run);
			}
			
			else
			{
				// 押されていない → Idleへ
				SetPlayerState(StateMode::Idle);
			}
		}
	}

}

void Player::ApplyHeartEffects()
{
	//移動速度、攻撃速度、
	// ステータスによっては時止めフラグ、バーサーカーモード
	//Warningはたぶん画面赤フラグ？
	auto state = GetPlayerHeartState();
	switch (state)
	{
	case HeartRateState::Stun:
		break;
	case HeartRateState::Warning:
		break;
	case HeartRateState::Berserk:
		break;
	case HeartRateState::TimeControl:
		break;
	case HeartRateState::Normal:
		break;

	case HeartRateState::Dead:

		break;
	default:
		break;
	}

}

void Player::PlayerMedecine()
{


	const double medicineFrameDuration = 0.15;
	if (animTime >= medicineFrameDuration)
	{
		animTime -= medicineFrameDuration;
		m_frameIndex++;
		if (m_frameIndex >= m_medecinePatterns.size())
		{

			m_frameIndex = 0;
			//  ここが重要！ 攻撃後の状態を決める
			if (KeyA.pressed() || KeyD.pressed())
			{
				// まだ移動キーが押されている → Runへ
				SetPlayerState(StateMode::Run);
			}
			else
			{
				// 押されていない → Idleへ
				SetPlayerState(StateMode::Idle);
			}
			if (KeySpace.down())
			{
				SetPlayerState(StateMode::Attack);
			}
		}
	}


}

void Player::PlayerBerserk()
{





}

void Player::PlayerDead()
{
	// Dead 状態のときのみ処理
	if (GetPlayerState() != StateMode::Dead)
		return;

	const double DeadFrameDuration = 0.45;
	animTime += Scene::DeltaTime();  // 時間を加算（忘れていないか確認）

	if (animTime >= DeadFrameDuration)
	{
		animTime -= DeadFrameDuration;

		// まだ最後のフレームに到達していない場合のみ進める
		if (m_frameIndex < static_cast<int>(m_deadPatterns.size()) - 1)
		{
			m_frameIndex++;
		}
		// 最後に達したら何もしない（そのまま静止）
	}
}


void Player::PlayerIdleToRun()
{
	if (GetPlayerState() == StateMode::IdleToRun)
	{
		const double runFrameDuration = 0.1;
		if (animTime >= runFrameDuration)
		{				
			animTime -= runFrameDuration;
			m_frameIndex++;
			if (m_frameIndex >= m_idleToRunPatterns.size())
			{
				m_frameIndex = 0;
				SetPlayerState(StateMode::Run);
			}
		}
	}

}

void Player::PlayerIdleToAttack(const Vec2& camera)
{
	const double attackToIdleFrameDuration = 0.08;
	if (!m_AttackFlag) return;

	if (animTime >= attackToIdleFrameDuration)
	{
		animTime -= attackToIdleFrameDuration;
		m_frameIndex++;

		// 攻撃判定のフレームを限定（例：3〜5フレーム目でヒット）
		if (m_frameIndex >= 3 && m_frameIndex <= 5)
		{
			const RectF pBox = getAttackRect(camera);
			if (RectToRect(pBox, enemyRect))
			{
				Print << U"攻撃ヒット！";
				// 敵のダメージ処理をここに追加予定
			}
		}

		// 攻撃アニメ終了
		if (m_frameIndex >= m_IdleAttackPatterns.size())
		{
			m_frameIndex = 0;
			SetPlayerAttackFlag(false);

			//  ここが重要！ 攻撃後の状態を決める
			if (KeyA.pressed() || KeyD.pressed())
			{
				// まだ移動キーが押されている → Runへ
				SetPlayerState(StateMode::Run);
			}
			else
			{
				// 押されていない → Idleへ
				SetPlayerState(StateMode::Idle);
			}
		}
	}
}

void Player::PlayerRun()
{
	// Run状態でのみ処理
	if (GetPlayerState() != StateMode::Run)
		return;

	const double runFrameDuration = 0.1; // アニメ速度（少し速めに）

	if (animTime >= runFrameDuration)
	{
		animTime -= runFrameDuration;
		m_frameIndex++;

		// === ループさせる（止まらない）===
		if (m_frameIndex >= m_runPatterns.size())
		{
			m_frameIndex = 0;
		}
	}
}

void Player::PlayerJump()
{

	// === アニメーション更新 ===
	animTime += Scene::DeltaTime();
	const double JumpFrameDuration = 0.08;

	if (animTime >= JumpFrameDuration)
	{
		animTime -= JumpFrameDuration;
		m_frameIndex++;

		if (m_frameIndex >= m_jumpPatterns.size())
		{
			m_frameIndex = 0;
			// ★ ここが重要！ 攻撃後の状態を決める
			if (KeyA.pressed() || KeyD.pressed())
			{
				// まだ移動キーが押されている → Runへ
				SetPlayerState(StateMode::Run);
			}
			else
			{
				SetPlayerState(StateMode::Idle);
			}
		}
	}


}

void Player::PlayerOnTheWall()
{

	const double onTheWallFrameDuration = 0.15;
	if (animTime >= onTheWallFrameDuration)
	{
		animTime -= onTheWallFrameDuration;
		m_frameIndex++;
		if (m_frameIndex >= m_onTheWallPatterns.size())
		{
			m_frameIndex = 0;
			
			// ★ ここが重要！ 攻撃後の状態を決める
			if (KeyA.pressed() || KeyD.pressed())
			{
				// まだ移動キーが押されている → Runへ
				SetPlayerState(StateMode::Run);
			}
			else
			{
				// 押されていない → Idleへ
				SetPlayerState(StateMode::Idle);
			}

		}
	}

	

}

void Player::PlayerFall()
{
	const double onTheWallFrameDuration = 0.15;
	if (animTime >= onTheWallFrameDuration)
	{
		animTime -= onTheWallFrameDuration;
		m_frameIndex++;
		if (m_frameIndex >= m_onTheWallPatterns.size())
		{
			m_frameIndex = 0;

			if (m_onGround)
			{
				// ★ ここが重要！ 攻撃後の状態を決める
				if (KeyA.pressed() || KeyD.pressed())
				{
					// まだ移動キーが押されている → Runへ
					SetPlayerState(StateMode::Run);
				}
				else
				{
					// 押されていない → Idleへ
					SetPlayerState(StateMode::Idle);
				}
			}

		}
	}

}



void Player::update(Game_Map& map, Array<Enemy>& m_enemies)
{

	//行動するたびにFlgをTrueにし減少処理を遮断
	//行動終了後にタイマー開始
	//タイマーが指定時間に達したらFlgをFalseに変更し減少処理を開始



	animTime += Scene::DeltaTime();
	m_DogelstTimer += Scene::DeltaTime();
	
	m_HeartTimer += Scene::DeltaTime(); // 
	// クールタイム中は m_DogeCoolTimer を減らす
	if (m_DogeCoolTimer > 0.0)
	{
		m_DogeCoolTimer -= Scene::DeltaTime();
		m_DogeCoolTimer = Max(0.0, m_DogeCoolTimer);
	}
	if (m_HeartCoolTimer > 0.0)
	{
		m_HeartCoolTimer -= Scene::DeltaTime();
		m_HeartCoolTimer = Max(0.0, m_HeartCoolTimer);
	}
	else {
		m_HeartCoolFlg = false;
	}
	
	if (!m_HeartCoolFlg && GetPlayerBPM() >= 90)
	{
		if (m_HeartTimer >= 1.0) // 1秒経過ごと
		{
			m_BPM -= 1;
			m_HeartTimer = 0.0;
		}
	}
	// ===== Player::update の中から抜粋 =====

// バーサークモード突入条件
	if (GetPlayerBPM() >= 120 && !m_BersarkFlg)
	{
		m_BersarkFlg = true;
		m_BersarkTimer = 8.0;             // バーサーク継続秒数
		m_IsInvincible = true;            // ★無敵ON
		m_AttackSpeedBoost = 1.5;         // ★攻撃速度倍率（1.5倍）
		Print << U"🔥バーサークモード突入！🔥";
	}

	// バーサーク継続処理
	if (m_BersarkFlg)
	{
		if (m_BersarkTimer > 0.0)
		{
			m_BersarkTimer -= Scene::DeltaTime();
			m_BersarkTimer = Max(0.0, m_BersarkTimer);
		}
		else
		{
			// 時間切れ → 通常状態に戻す
			m_BersarkFlg = false;
			m_IsInvincible = false;        // ★無敵解除
			m_AttackSpeedBoost = 1.0;      // ★攻撃速度戻す
			Print << U"バーサーク解除";
		}
	}

	UpdateHeartState();
	ApplyHeartEffects();
	//-----------------------------------
	// 入力処理 & 状態遷移
	//-----------------------------------
	Vec2 input{
		(KeyD.pressed() ? 1.0 : 0.0) - (KeyA.pressed() ? 1.0 : 0.0),
		0.0
	};

	// Dodge入力受付
	if (KeyEnter.down() && m_DogeCoolTimer <= 0.0)
	{
		SetPlayerAttackFlag(false);
		SetPlayerState(StateMode::Doge);
		SetPlayerBPM(GetPlayerBPM() + 5);
		m_isDodging = true;
		m_DogeTimer = 0.0;
		m_DogeCoolTimer = m_DogeCooldown; // ← クールタイム発動
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// Idle → IdleToRun（最初の走り出し）
	if ((KeyD.down() || KeyA.down()) && GetPlayerState() == StateMode::Idle)
	{
		SetPlayerState(StateMode::IdleToRun);
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// Run 維持処理（キー押しっぱなし）
	if ((KeyD.pressed() || KeyA.pressed()) && GetPlayerState() == StateMode::Run)
	{
		// 何もしない（Runを維持）
		m_HeartTimer = 0.0;
		m_HeartCoolTimer = m_HeartCooldown;
		m_HeartCoolFlg = true; // ★ クールタイム中は減少を止める
	}
	else if (!KeyA.pressed() && !KeyD.pressed())
	{
		// 入力が無い場合のみ Idle に戻す
		if (GetPlayerState() == StateMode::Run)
		{
			SetPlayerState(StateMode::Idle);
			m_frameIndex = 0;
			animTime = 0.0;
		}
	}

	// 向きの反転
	if (input.x != 0)
	{
		SetPlayerFaceRight(input.x > 0);
	}

	Vec2 pos = GetPlayerPosition();
	Vec2 size = GetPlayerHitBox();
	Vec2 velocity = GetPlayerVelocity();

	// === 統一した当たり判定サイズを計算 ===
	const SizeF collisionSize = {
		m_HitBox.x * m_Scale.x / 10,
		m_HitBox.y * m_Scale.y / 10
	};

	// === 重要：描画との整合性を取るためのオフセット ===
	const Vec2 collisionOffset = Vec2{ 0, -40 };

	bool isTouchingWallLeft = false;
	bool isTouchingWallRight = false;

	//-----------------------------------
	// 横移動処理
	//-----------------------------------
	{
		//  壁キック中は強制移動（最優先） 
		if (m_WallKickTimer > 0.0)
		{
			m_WallKickTimer -= Scene::DeltaTime();

			// 壁キック方向の速度を維持（入力を無視）
			// velocity.x はすでに壁ジャンプ時に設定済み

			// 位置更新
			Vec2 nextPosX = pos + Vec2(velocity.x * Scene::DeltaTime(), 0);
			RectF rectX(Arg::center = nextPosX + collisionOffset, collisionSize);

			// マップ衝突チェックのみ（壁に当たったら停止）
			if (!map.CheckCollision(rectX))
			{
				pos.x = nextPosX.x;
			}
			else
			{
				// 壁に当たったらタイマー終了
				m_WallKickTimer = 0.0;
				velocity.x = 0;
			}

			// 壁判定フラグを無効化（壁に張り付かないように）
			isTouchingWallLeft = false;
			isTouchingWallRight = false;
		}
		else
		{
			//  通常の横移動処理 
			velocity.x = input.x * GetPlayerSpeed();
			Vec2 nextPosX = pos + Vec2(velocity.x * Scene::DeltaTime(), 0);

			RectF rectX(Arg::center = nextPosX + collisionOffset, collisionSize);

			bool mapColli = map.CheckCollision(rectX);
			bool enemyColli = false;
			if (GetPlayerState() != StateMode::Doge )
			{
				enemyColli = RectToRect(rectX, enemyRect);

			}

			if ((!mapColli) && (!enemyColli))
			{
				pos.x = nextPosX.x;
			}
			else
			{
				// 壁衝突 - 位置を補正
				velocity.x = 0;

				// めり込みを戻す
				int maxIterations = 100;
				int iterations = 0;
				while ((map.CheckCollision(rectX) ||
					(GetPlayerState() != StateMode::Doge &&RectToRect(rectX, enemyRect)))&& iterations < maxIterations)
				{
					if (input.x > 0)
					{
						nextPosX.x -= 0.5;
						isTouchingWallRight = true;
					}
					else if (input.x < 0)
					{
						nextPosX.x += 0.5;
						isTouchingWallLeft = true;
					}
					else
					{
						break;
					}
					rectX.setCenter(nextPosX + collisionOffset);
					iterations++;
				}

				if (iterations < maxIterations)
				{
					pos.x = nextPosX.x;
				}
			}
		}
	}

	//-----------------------------------
	// 重力処理
	//-----------------------------------
	if (!m_onGround)
	{
		velocity.y += m_gravity * Scene::DeltaTime() * 400;
	}

	//-----------------------------------
	// ジャンプ処理（地上 or 壁キック）
	//-----------------------------------
	{
		static bool canWallJump = true;
		bool tryJump = (KeyW.down() || KeyUp.down());

		if (m_onGround)
		{
			canWallJump = true;
		}

		// 通常ジャンプ
		if (tryJump && m_onGround)
		{
			constexpr double JumpPowerScale = 200.0;
			velocity.y = -GetPlayerJumpSpeed() * JumpPowerScale;
			m_onGround = false;
			SetPlayerAttackFlag(false);
			SetPlayerState(StateMode::Jump);
			m_frameIndex = 0;
			animTime = 0.0;
		}
		//  壁ジャンプ（修正版） 
		else if (tryJump && canWallJump && (isTouchingWallLeft || isTouchingWallRight))
		{
			constexpr double JumpPowerScale = 200.0;
			constexpr double WallKickForce = 500.0;  // 壁キックの横方向の力

			canWallJump = true;

			// 縦方向の速度
			velocity.y = -GetPlayerJumpSpeed() * (JumpPowerScale * 1.1);

			//  横方向の速度：壁の反対方向に固定 
			if (isTouchingWallLeft)
			{
				velocity.x = WallKickForce;  // 右方向へ
			}
			else // isTouchingWallRight
			{
				velocity.x = -WallKickForce; // 左方向へ
			}

			m_onGround = false;
			SetPlayerAttackFlag(false);
			SetPlayerState(StateMode::Jump);
			m_frameIndex = 0;
			animTime = 0.0;

			//  壁キックタイマー開始（0.2-0.3秒程度） 
			m_WallKickTimer = 0.25;
		}
		// 壁に張り付き
		else if (!m_onGround && (isTouchingWallLeft || isTouchingWallRight))
		{
			velocity.y = Min(velocity.y, 100.0);
			SetPlayerAttackFlag(false);
			SetPlayerState(StateMode::OnTheWall);
		}
	}
	//-----------------------------------
	// 縦方向移動処理
	//-----------------------------------
	Vec2 nextPosY = pos + Vec2(0, velocity.y * Scene::DeltaTime());
	RectF rectY(Arg::center = nextPosY + collisionOffset, collisionSize);

	bool hitGround = false;
	bool hitCeiling = false;

	if (!map.CheckCollision(rectY))
	{
		pos.y = nextPosY.y;
	}
	else
	{
		if (velocity.y > 0)
		{
			// 地面に衝突
			velocity.y = 0;

			int maxIterations = 100;
			int iterations = 0;
			while (map.CheckCollision(rectY) && iterations < maxIterations)
			{
				nextPosY.y -= 0.5;
				rectY.setCenter(nextPosY + collisionOffset);
				iterations++;
			}

			if (iterations < maxIterations)
			{
				pos.y = nextPosY.y;
				hitGround = true;
			}
		}
		else if (velocity.y < 0)
		{
			// 天井衝突
			velocity.y = 0;

			int maxIterations = 100;
			int iterations = 0;
			while (map.CheckCollision(rectY) && iterations < maxIterations)
			{
				nextPosY.y += 0.5;
				rectY.setCenter(nextPosY + collisionOffset);
				iterations++;
			}

			if (iterations < maxIterations)
			{
				pos.y = nextPosY.y;
				hitCeiling = true;
			}
		}
	}

	//-----------------------------------
	// 接地判定
	//-----------------------------------
	RectF groundCheckRect(
		Arg::center = (pos + collisionOffset).movedBy(0, collisionSize.y / 2 + 1),
		SizeF{ collisionSize.x * 0.9, 4 }
	);

	m_onGround = map.CheckCollision(groundCheckRect) || hitGround;

	//-----------------------------------
	//  Fall状態への自動遷移（ジャンプ以外で空中にいる場合）
	//-----------------------------------
	if (!m_onGround &&
		GetPlayerState() != StateMode::Jump &&
		GetPlayerState() != StateMode::Fall &&
		GetPlayerState() != StateMode::OnTheWall &&
		GetPlayerState() != StateMode::Doge &&
		GetPlayerState() != StateMode::Attack &&
		GetPlayerState() != StateMode::IdleToAttack &&
		GetPlayerState() != StateMode::IdleToRun &&
		GetPlayerState() != StateMode::JumpAttack &&
		GetPlayerState() != StateMode::Dead
		)  // ← 追加
	{
		//  攻撃フラグをリセット
		SetPlayerAttackFlag(false);
		SetPlayerState(StateMode::Fall);
		m_frameIndex = 0;
		animTime = 0.0;
	}

	//  接地時の状態遷移（Fall状態から復帰）
	if (m_onGround && GetPlayerState() == StateMode::Fall)
	{
		if (KeyA.pressed() || KeyD.pressed())
		{
			SetPlayerState(StateMode::Run);
		}
		else
		{
			SetPlayerState(StateMode::Idle);
		}
		m_frameIndex = 0;
		animTime = 0.0;
	}

	//-----------------------------------
	// 攻撃処理（優先度を高く）
	//-----------------------------------
	// --- update() 内の末尾付近 ---
	if (KeySpace.down()
		&& !IsPlayerAttacking()
		&& GetPlayerState() != StateMode::Hurt
		&& GetPlayerState() != StateMode::Dead
		&& GetPlayerState() != StateMode::Doge)
	{
		SetPlayerLastState(GetPlayerState());
		SetPlayerAttackFlag(true);
		m_frameIndex = 0;
		animTime = 0.0;

		// 攻撃ステートへ
		if (m_onGround)
		{
			if (GetPlayerState() == StateMode::Idle)
				SetPlayerState(StateMode::IdleToAttack);
			else
				SetPlayerState(StateMode::Attack);
			SetPlayerBPM(GetPlayerBPM() + 5);
		}
		else
		{
			SetPlayerState(StateMode::JumpAttack);
			SetPlayerBPM(GetPlayerBPM() + 8);
		}	
		m_HeartTimer = 0.0;
		m_HeartCoolTimer = m_HeartCooldown;
		m_HeartCoolFlg = true; // ★ クールタイム中は減少を止める
	}


	// ======== 接地時の状態復帰 ========
	if (m_onGround)
	{
		if (GetPlayerState() == StateMode::Fall ||
			GetPlayerState() == StateMode::Jump ||
			GetPlayerState() == StateMode::OnTheWall)
		{
			// 速度リセット
			velocity.y = 0;

			// 状態遷移
			if (KeyA.pressed() || KeyD.pressed())
			{
				SetPlayerState(StateMode::Run);
			}
			else
			{
				SetPlayerState(StateMode::Idle);
			}

			m_frameIndex = 0;
			animTime = 0.0;
		}
	}

	//-----------------------------
	//デバック用
	//-----------------------------
	{
		if (KeyP.down())
		{

			SetPlayerState(StateMode::Dead);

		}
		if (KeyL.down())
		{
			SetPlayerState(StateMode::Medecine);
			SetPlayerBPM(GetPlayerBPM() - 30);//仮の数値、薬をブッキメの値を変えたかったらここ
		}

		if (KeyT.pressed())
		{
			TimeStopManager::Start(); // ザ・ワールド発動
		}
		if (KeyT.up())
		{
			TimeStopManager::Stop(); // ザ・ワールド発動
		}

	}
	//-----------------------------------
    // 走行中の心拍数上昇（時間経過で強くなる）
    //-----------------------------------
	static double runHeartTimer = 0.0;
	static double runDuration = 0.0;

	if (GetPlayerState() == StateMode::Run)
	{
		runHeartTimer += Scene::DeltaTime();
		runDuration += Scene::DeltaTime();

		// BPM上昇間隔を徐々に短くする（走り続けるほど疲れる）
		double interval = Max(0.2, 2.0 - runDuration * 0.2); // 最短0.5秒まで

		if (runHeartTimer >= interval)
		{
			SetPlayerBPM(GetPlayerBPM() + 1.5);
			runHeartTimer = 0.0;

			m_HeartTimer = 0.0;
			m_HeartCoolTimer = m_HeartCooldown;
			m_HeartCoolFlg = true;
		}
	}
	else
	{
		runHeartTimer = 0.0;
		runDuration = 0.0;
	}


	//-----------------------------------
	// アニメーション処理
	//-----------------------------------
	switch (GetPlayerState())
	{
	case StateMode::Idle:
		PlayerIdle();
		break;
	case StateMode::IdleToRun:
		PlayerIdleToRun();
		break;
	case StateMode::Run:
		PlayerRun();
		break;
	case StateMode::Jump:
		PlayerJump();
		break;
	case StateMode::JumpAttack:
		PlayerJumpAttack();
		break;

	case StateMode::Fall:
		PlayerFall();
		break;
	case StateMode::OnTheWall:
		PlayerOnTheWall();
		break;
	case StateMode::Attack:
		PlayerAttack(map.getCameraPos(), m_enemies);
		break;
	case StateMode::Hurt:
		PlayerHurt();
		break;
	case StateMode::IdleToAttack:
		PlayerIdleToAttack(map.getCameraPos());
		break;
	case StateMode::Doge:
		PlayerDoge();
		break;

	case StateMode::Medecine:

		PlayerMedecine();
		break;

	case StateMode::Dead:
		PlayerDead();
		break;
	default:
		break;
	}

	//-----------------------------------
	// 値を反映
	//-----------------------------------
	SetPlayerVelocity(velocity);
	SetPlayerPosition(pos);
}

// ============================
// 描画処理
// ============================
void Player::draw(const Game_Map& CameraPos) const
{
	const Texture& PlayerTex = TextureAsset(U"Player");

	const int32 frameWidth = 516;
	const int32 frameHeight = 285;

	const int32 idleY = 0;
	const int32 runY = frameHeight * 1;
	const int32 attackY = frameHeight * 2;
	const int32 hurtY = frameHeight * 4;
	const int32 IdleAttack = frameHeight * 4;
	const int32 Doge = frameHeight * 4;
	const int32 Jump = frameHeight * 4;
	const int32 Fall = frameHeight * 4;
	const int32 OnTheWall = frameHeight * 6;
	const int32 Dead = frameHeight * 5;
	const int32 Dead2 = frameHeight * 6;
	const int32 Medicine = frameHeight * 6;

	int32 n = 0;
	int32 y = idleY;

	// === アニメーション選択 ===
	switch (GetPlayerState())
	{
	case StateMode::Idle:
		n = m_idlePatterns[m_frameIndex];
		break;

	case StateMode::IdleToRun:
		n = m_idleToRunPatterns[m_frameIndex];
		y = runY;
		break;

	case StateMode::Run:
		n = m_runPatterns[m_frameIndex];
		if (m_frameIndex == 4)
		{
			n = 0; y = attackY + 30;
		}
		else
		{
			y = runY;
		}
		break;

	case StateMode::Attack:
		n = m_attackPatterns[m_frameIndex];
		y = attackY + 30;
		break;
	
	case StateMode::Jump:
		n = m_jumpPatterns[m_frameIndex];
		y = Jump + 65;
		break;

	case StateMode::JumpAttack:
		n = m_jumpAttackPatterns[m_frameIndex];
		if (m_frameIndex >= 1)
		{
			//n = 0;
			y = (frameHeight * 5) + 60;
		}
		else
		{
			y = (frameHeight * 4) + 60;
		}
		break;

	case StateMode::Fall:
		n = m_FallPatterns[m_frameIndex];
		y = Fall + 65;
		break;
	case StateMode::OnTheWall:
		n = m_onTheWallPatterns[m_frameIndex];
		y = OnTheWall + 65;
		break;

	case StateMode::IdleToAttack:
		n = m_IdleAttackPatterns[m_frameIndex];
		if (m_frameIndex >= 2)
		{
			//n = 0;
			y = (frameHeight * 4) + 50;
		}
		else
		{
			y = (frameHeight * 3) + 50;
		}
		break;
		
	

	case StateMode::Hurt:
		n = m_hurtPatterns[m_frameIndex];
		y = hurtY + 45;
		break;

	case StateMode::Doge:
		n = m_dogePatterns[m_frameIndex];
		y = Doge + 65;
		break;

	case StateMode::Medecine:
		n = m_medecinePatterns[m_frameIndex];
		y = Medicine + 75;
		break;

	case StateMode::Dead:
		n = m_deadPatterns[m_frameIndex];
		if (m_frameIndex >= 4)
		{
			//n = 0;
			y = Dead2+75;
		}
		else
		{
			y = Dead + 75;
		}
		break;

	
	default:
		n = m_idlePatterns[m_frameIndex];
		break;
	}

	// === 描画位置（体中心） ===
	Vec2 drawPos = GetPlayerPosition() - CameraPos.getCameraPos();

	// === スケールと左右反転 ===
	const double scaleY = GetPlayerScale().y / frameHeight;
	const double scaleX = scaleY * (IsPlayerFacingRight() ? 1.0 : -1.0);

	// === オフセット ===
	const Vec2 offset = IsPlayerFacingRight() ? Vec2{ 10, 0 } : Vec2{ -10, 0 };

	// === 当たり判定中心と一致 ===
	// （スプライトの中心がキャラクターの中心に一致）
	const Vec2 spriteCenterOffset = Vec2{ 0, (GetPlayerHitBox().y * 0.5) - (frameHeight * scaleY * 0.5) };
	drawPos += spriteCenterOffset;

	// === ドッジ専用オフセット ===
	Vec2 dogeOffset = Vec2::Zero();
	if (GetPlayerState() == StateMode::Doge)
	{
		dogeOffset = IsPlayerFacingRight() ? Vec2{ -15, 0 } : Vec2{ 15, 0 };
	}

	// === スプライト描画 ===
	PlayerTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(scaleX, scaleY)
		.drawAt(drawPos + offset + dogeOffset);

	// === デバッグ表示 ===
	RectF hitBox = getHitRect(CameraPos.getCameraPos());
	hitBox.drawFrame(3, ColorF{ 1, 0, 0, 1.0 }); // 赤

	RectF attackBox = getAttackRect(CameraPos.getCameraPos());
	attackBox.drawFrame(3, ColorF{ 0, 1, 1, 0.5 }); // シアン

	enemyRect.movedBy(-CameraPos.getCameraPos()).drawFrame(2, ColorF{ 0, 1, 1, 0.5 });

	
	Print << U"" << m_BPM;
	
}
