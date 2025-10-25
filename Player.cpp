#include "Player.hpp"
#include "Game.hpp"
#include "Collision.hpp"

using namespace Collision;

RectF enemyRect{ 1000, 750, 64, 64 };

Player::Player() {}
Player::~Player() {}

RectF Player::getAttackRect() const
{
	return RectF{
		Arg::center = GetPlayerPosition().movedBy(0, -GetPlayerHitBox().y * 0.2),
		SizeF{ 200, 220 }
	};
}

void Player::takeDamage(int dmg)
{
	if (GetPlayerState() == StateMode::Hurt || GetPlayerState() == StateMode::Doge)
		return;

	SetPlayerState(StateMode::Hurt);
	SetPlayerHP(GetPlayerHP() - dmg);
}

void Player::PlayerAttack()
{
	const double attackFrameDuration = 0.08;

	if (!m_AttackFlag) return;

	if (animTime >= attackFrameDuration)
	{
		animTime -= attackFrameDuration;
		m_frameIndex++;

		// 攻撃判定のフレームを限定（例：3〜5フレーム目でヒット）
		if (m_frameIndex >= 3 && m_frameIndex <= 5)
		{
			const RectF pBox = getAttackRect();
			if (RectToRect(pBox, enemyRect))
			{
				Print << U"攻撃ヒット！";
				// 敵のダメージ処理をここに追加予定
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
	// === 初回突入時の処理 ===
	static bool isDodging = false;
	static double dogeTimer = 0.0;

	if (!isDodging)
	{
		isDodging = true;
		dogeTimer = 0.0;

		//// 入力方向へ高速移動（回避の瞬間ダッシュ）
		//double dir = IsPlayerFacingRight() ? 1.0 : -1.0;
		//SetPlayerVelocity(Vec2(700 * dir, GetPlayerVelocity().y));

		SetPlayerSpeed(DogePlayerSpeed);

		// アニメ初期化
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// === アニメーション更新 ===
	animTime += Scene::DeltaTime();
	const double dogeFrameDuration = 0.08;

	if (animTime >= dogeFrameDuration)
	{
		animTime -= dogeFrameDuration;
		m_frameIndex++;

		if (m_frameIndex >= m_dogePatterns.size())
		{
			m_frameIndex = 0;
		}
	}

	// === 持続時間管理 ===
	dogeTimer += Scene::DeltaTime();
	const double dogeDuration = 0.5; // 回避時間

	if (dogeTimer >= dogeDuration)
	{
		// 終了時のリセット
		SetPlayerVelocity(Vec2(0, GetPlayerVelocity().y));
		SetPlayerSpeed(NormalPlayerSpeed);
		
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
		dogeTimer = 0.0;
		isDodging = false;
	}
}


void Player::PlayerHurt()
{
	if (GetPlayerState() != StateMode::Pareise)
	{
		const double hurtFrameDuration = 0.3;
		if (animTime >= hurtFrameDuration)
		{
			animTime -= hurtFrameDuration;
			m_frameIndex++;
			if (m_frameIndex >= m_hurtPatterns.size())
			{
				m_frameIndex = 0;
				SetPlayerState(StateMode::Idle);
			}
		}
	}
}

void Player::PlayerIdleToRun()
{
	if (GetPlayerState() == StateMode::IdleToRun)
	{
		const double runFrameDuration = 0.15;
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

void Player::PlayerIdleToAttack()
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
			const RectF pBox = getAttackRect();
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

		// === ★ループさせる（止まらない）===
		if (m_frameIndex >= m_runPatterns.size())
		{
			m_frameIndex = 0;
		}
	}
}



void Player::update(Game_Map& map)
{
	
	animTime += Scene::DeltaTime();

	//-----------------------------------
	// 入力処理
	//-----------------------------------
	//-----------------------------------
// 入力処理 & 状態遷移
//-----------------------------------
	//-----------------------------------
// 入力処理
//-----------------------------------
	Vec2 input{
		
		(KeyD.pressed() ? 1.0 : 0.0) - (KeyA.pressed() ? 1.0 : 0.0),
		0.0
	};

	// === 状態遷移 ===

	if (KeyEnter.down()) {
		SetPlayerState(StateMode::Doge);
	}

	// Idle → IdleToRun（最初の走り出し）
	if ((KeyD.down() || KeyA.down()) && GetPlayerState() == StateMode::Idle)
	{
		SetPlayerState(StateMode::IdleToRun);
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// IdleToRun → Run は PlayerIdleToRun() 内で自動遷移

	// Run 維持処理（キー押しっぱなし）
	if ((KeyD.pressed() || KeyA.pressed()) && GetPlayerState() == StateMode::Run)
	{
		// 何もしない（Runを維持）
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

		bool isTouchingWallLeft = false;
		bool isTouchingWallRight = false;

		//-----------------------------------
		// 横移動処理
		//-----------------------------------
		
		velocity.x = input.x * GetPlayerSpeed();
		Vec2 nextPosX = pos + Vec2(velocity.x * Scene::DeltaTime() * 10, 0);
		RectF rectX(Arg::center = nextPosX, size);
		RectF erectX(Arg::center = Vec2(1000, 750), Vec2(64, 64));
		bool mapColli = map.CheckCollision(rectX);
		bool enemyColli = false;
		if (GetPlayerState() != StateMode::Doge)
		{
			 enemyColli = RectToRect(rectX, enemyRect);
		}
		
		if ((!mapColli) && (!enemyColli))
		{
			pos.x = nextPosX.x;
		}
		else
		{
			// 壁衝突
			if (input.x > 0) isTouchingWallRight = true;
			else if (input.x < 0) isTouchingWallLeft = true;

			velocity.x = 0;
		}

		if (input.x != 0)
		{
			SetPlayerFaceRight(input.x > 0);
		}

		//-----------------------------------
		// 重力処理
		//-----------------------------------
		velocity.y += m_gravity * Scene::DeltaTime() * 90;

		//-----------------------------------
		// ジャンプ処理（地上 or 壁キック）
		//-----------------------------------
		static bool canWallJump = true; // ★ 一度だけ壁ジャンプできるフラグ

		bool tryJump = (KeyW.down() || KeyUp.down());

		if (m_onGround)
		{
			canWallJump = true; // 地面に着いたらリセット
		}

		if (tryJump)
		{
			constexpr double JumpPowerScale = 100.0;

			if (m_onGround)
			{
				// 通常ジャンプ
				velocity.y = -GetPlayerJumpSpeed() * JumpPowerScale;
				m_onGround = false;
			}
			else if (canWallJump && (isTouchingWallLeft || isTouchingWallRight))
			{
				// 壁ジャンプ
				canWallJump = false;
				velocity.y = -GetPlayerJumpSpeed() * (JumpPowerScale * 0.9); // 少し弱め
				velocity.x = (isTouchingWallLeft ? 500 : -500); // 反対方向に跳ねる
				m_onGround = false;
			}
		}

		//-----------------------------------
		// 縦方向移動処理（壁上端安定）
		//-----------------------------------
		Vec2 nextPosY = pos + Vec2(0, velocity.y * Scene::DeltaTime());
		RectF rectY(Arg::center = nextPosY, SizeF{ size.x * 0.95, size.y * 1.05 });

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
				hitGround = true;
				velocity.y = 0;

				// 微妙にめり込んだ時の戻し
				while (map.CheckCollision(rectY))
				{
					nextPosY.y -= 0.5;
					rectY.setCenter(nextPosY);
				}
				pos.y = nextPosY.y;
				m_onGround = true;
			}
			else if (velocity.y < 0)
			{
				// 天井衝突
				hitCeiling = true;
				velocity.y = 0;
				while (map.CheckCollision(rectY))
				{
					nextPosY.y += 0.5;
					rectY.setCenter(nextPosY);
				}
				pos.y = nextPosY.y;
			}
		}

		//-----------------------------------
		// 接地判定
		//-----------------------------------
		RectF groundCheckRect(
			Arg::center = pos.movedBy(0, size.y / 2 + 2), // 判定を小さく
			SizeF{ size.x * 0.9, 6 }
		);

		m_onGround = map.CheckCollision(groundCheckRect) || hitGround;

		//-----------------------------------
		// 攻撃処理
		//-----------------------------------
		// 攻撃処理
		if (KeySpace.down() && !IsPlayerAttacking())
		{
			SetPlayerLastState(GetPlayerState());
			SetPlayerAttackFlag(true);
			m_frameIndex = 0;
			animTime = 0.0;

			// ★IdleならIdleToAttack、それ以外はAttack
			if (GetPlayerState() == StateMode::Idle)
			{
				SetPlayerState(StateMode::IdleToAttack);
			}
			else
			{
				SetPlayerState(StateMode::Attack);
			}
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
		
		case StateMode::Attack:
			PlayerAttack();
			break;
		case StateMode::Hurt:
			PlayerHurt();
			break;
		case StateMode::IdleToAttack:
			PlayerIdleToAttack();
			break;

		case StateMode::Doge:
			PlayerDoge();
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

	int32 n = 0;
	int32 y = idleY;

	switch (GetPlayerState())
	{
	case StateMode::Idle:
		n = m_idlePatterns[m_frameIndex];
		break;
	case StateMode::Attack:
		n = m_attackPatterns[m_frameIndex];
		y = attackY+30;
		break;
	case StateMode::IdleToRun:
		n = m_idleToRunPatterns[m_frameIndex];
		y = runY;
		break;

	case StateMode::IdleToAttack:
		n = m_IdleAttackPatterns[m_frameIndex];
		if (m_frameIndex >3 )
		{
			
			y = IdleAttack+50;
		}
		else
		{
			y = IdleAttack+50;
		}
		break;
	case StateMode::Run:
		n = m_runPatterns[m_frameIndex];
		if (m_frameIndex==5)
		{
			n = 0;
			y = attackY+30;
		}
		else
		{
			y = runY;
		}
		break;
	case StateMode::Hurt:
		n = m_hurtPatterns[m_frameIndex];
		
	    y = hurtY - 300;
		
		
		break;


	case StateMode::Doge:
		n = m_dogePatterns[m_frameIndex];
		y = Doge+65;

		break;
	default:
		n = m_idlePatterns[m_frameIndex];
		break;
	}

	Vec2 drawPos = GetPlayerPosition() - CameraPos.getCameraPos();

	// ★ 当たり判定にぴったり足元が接地
	double scaleY = GetPlayerHitBox().y / frameHeight;
	double scaleX = scaleY * (IsPlayerFacingRight() ? 1.0 : -1.0);

	// 足元をピッタリ合わせる
	drawPos.y -= (frameHeight * scaleY) / 2 - (GetPlayerHitBox().y / 2);
	if (GetPlayerState() == StateMode::Doge)
	{
		if(IsPlayerFacingRight())
		{
			PlayerTex(n * frameWidth, y, frameWidth, frameHeight)
				.scaled(scaleX, scaleY)
				.drawAt(drawPos - Vec2{ 15,0 });
		}
		else {
			PlayerTex(n * frameWidth, y, frameWidth, frameHeight)
				.scaled(scaleX, scaleY)
				.drawAt(drawPos + Vec2{ 15,0 });
		}
	}

	PlayerTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(scaleX, scaleY)
		.drawAt(drawPos);

	// デバッグ枠
	RectF hitBox(Arg::center = GetPlayerPosition(), GetPlayerHitBox());
	hitBox.movedBy(-CameraPos.getCameraPos()).drawFrame(1, ColorF{ 1, 1, 0, 0.8 });
	// デバッグ枠
	RectF attackBox = getAttackRect();
	attackBox.movedBy(-CameraPos.getCameraPos()).drawFrame(2, ColorF{ 0, 1, 0, 0.5 });
	enemyRect.movedBy(-CameraPos.getCameraPos()).drawFrame(2, ColorF{ 0, 1, 0, 0.5 });
	/*Print << U"state:" << (int)GetPlayerState();
	Print << U"Frame:" << m_frameIndex;*/
	Print << U"velo" << m_Velocity;
	
}
