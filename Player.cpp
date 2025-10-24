#include "Player.hpp"
#include "Game.hpp"
#include "Collision.hpp"

using namespace Collision;

RectF enemyRect{ 1600, 100, 64, 64 };

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
	if (GetPlayerState() == StateMode::Hurt || GetPlayerState() == StateMode::Avoidance)
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

		if (m_frameIndex >= m_attackPatterns.size())
		{
			m_frameIndex = 0;
			SetPlayerAttackFlag(false);
			SetPlayerState(StateMode::Idle);
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

void Player::PlayerHurt()
{
	if (GetPlayerState() != StateMode::Pareise)
	{
		const double hurtFrameDuration = 0.15;
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

void Player::update(Game_Map& map)
{
	animTime += Scene::DeltaTime();

	Vec2 input{
		(KeyD.pressed() ? 1.0 : 0.0) - (KeyA.pressed() ? 1.0 : 0.0),
		0.0
	};

	if (KeyO.down())
	{
		SetPlayerState(StateMode::Hurt);
		m_frameIndex = 0;
	}

	Vec2 pos = GetPlayerPosition();
	Vec2 size = GetPlayerHitBox();
	Vec2 velocity = GetPlayerVelocity();

	// 横移動
	velocity.x = input.x * GetPlayerSpeed();
	Vec2 nextPosX = pos + Vec2(velocity.x * Scene::DeltaTime() * 10, 0);
	RectF rectX(Arg::center = nextPosX, size);

	if (!map.CheckCollision(rectX))
		pos.x = nextPosX.x;
	else
		velocity.x = 0; // 壁に当たったら横速度をリセット

	if (input.x != 0)
		SetPlayerFaceRight(input.x > 0);

	// 重力
	velocity.y += m_gravity * Scene::DeltaTime() * 90;

	// ジャンプ（接地判定の前に入力をチェック）
	if (m_onGround && (KeyW.down() || KeyUp.down()))
	{
		constexpr double JumpPowerScale = 100.0;
		velocity.y = -GetPlayerJumpSpeed() * JumpPowerScale;
		m_onGround = false;
	}

	// 縦移動（高さを1.1倍に拡大）
	Vec2 nextPosY = pos + Vec2(0, velocity.y * Scene::DeltaTime());
	RectF rectY(Arg::center = nextPosY, SizeF{ size.x, size.y * 1.1 });

	bool hitCeiling = false;
	bool hitGround = false;

	if (!map.CheckCollision(rectY))
	{
		pos.y = nextPosY.y;
	}
	else
	{
		if (velocity.y > 0)
		{
			// 地面衝突
			hitGround = true;
			velocity.y = 0;

			// 1ピクセルずつ戻す（細かく補正）
			while (map.CheckCollision(rectY))
			{
				nextPosY.y -= 0.5;
				rectY.setCenter(nextPosY);
			}
			pos.y = nextPosY.y;
		}
		else if (velocity.y < 0)
		{
			// 天井衝突 → 速度を0にして落下開始
			hitCeiling = true;
			velocity.y = 0;

			// 1ピクセルずつ戻す（細かく補正）
			while (map.CheckCollision(rectY))
			{
				nextPosY.y += 0.1;
				rectY.setCenter(nextPosY);
			}
			pos.y = nextPosY.y;
		}
	}

	// 接地判定（位置更新後にチェック）
	RectF groundCheckRect(Arg::center = pos.movedBy(0, size.y / 2 + 2),
		SizeF{ size.x * 0.8, 5 });
	m_onGround = map.CheckCollision(groundCheckRect) || hitGround;

	// 攻撃入力
	if (KeySpace.down() && !IsPlayerAttacking())
	{
		SetPlayerState(StateMode::Attack);
		SetPlayerAttackFlag(true);
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// アニメーション処理
	switch (GetPlayerState())
	{
	case StateMode::Idle: PlayerIdle(); break;
	case StateMode::Attack: PlayerAttack(); break;
	case StateMode::Hurt: PlayerHurt(); break;
	default:
		break;
	}

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
	const int32 frameHeight = 300;

	const int32 idleY = 0;
	const int32 attackY = frameHeight * 1;
	const int32 hurtY = frameHeight * 4;

	int32 n = 0;
	int32 y = idleY;

	// アニメーションフレーム選択
	switch (GetPlayerState())
	{
	case StateMode::Idle:
		n = m_idlePatterns[m_frameIndex];
		break;
	case StateMode::Attack:
		n = m_attackPatterns[m_frameIndex];
		y = attackY - 70;
		break;
	case StateMode::Hurt:
		n = m_hurtPatterns[m_frameIndex];
		y = hurtY - 300;
		break;
	default:
		n = m_idlePatterns[m_frameIndex];
		break;
	}
	// ------------------------------
	// デバッグ用当たり判定表示
	// ------------------------------
	// ------------------------------
// デバッグ用当たり判定表示
// ------------------------------
	RectF attackBox{
		Arg::center = GetPlayerPosition().movedBy(0, -GetPlayerHitBox().y * 0.2),
		SizeF{ 200, 160 }
	};

	// カメラ補正して描画
	attackBox.movedBy(-CameraPos.getCameraPos()).drawFrame(3, 0, ColorF{ 0.0, 1.0, 0.0, 0.5 });

	//RectF playerBox{
	//	Arg::center = GetPlayerPosition().movedBy(0, -GetPlayerHitBox().y * 0.25),
	//	SizeF{ GetPlayerHitBox().x, GetPlayerHitBox().y * 1.5 }
	//};

	//NOT COLLISION BOX
	Vec2 hitSize = GetPlayerHitBox();
	Vec2 pos = GetPlayerPosition();
	Vec2 offset = Vec2(0, 10); // same as collision box shift

	RectF playerBox{
		Arg::center = pos,   // shift down
		SizeF{ hitSize.x, hitSize.y}
	};
	// カメラ補正して描画
	playerBox.movedBy(-CameraPos.getCameraPos()).drawFrame(3, 0, ColorF{ 1.0, 1.0, 0.0, 1.0 });


	// カメラ補正
	Vec2 drawPos = GetPlayerPosition() - CameraPos.getCameraPos();

	// --- ヒットボックスに合わせたスケール計算（少し大きめに） ---
	double scaleY = GetPlayerHitBox().y / frameHeight * 1.3;  // 1.1倍で少し大きく
	double scaleX = scaleY * (IsPlayerFacingRight() ? 1.0 : -1.0);

	// 足元基準で補正（浮きにくくするため少し下げる）
	drawPos.y -= (frameHeight * scaleY) - GetPlayerHitBox().y * 1.15;

	// 描画
	PlayerTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(scaleX, scaleY)
		.drawAt(drawPos);

	// デバッグ表示
	 attackBox = getAttackRect();
	attackBox.movedBy(-CameraPos.getCameraPos()).drawFrame(2, ColorF{ 0, 1, 0, 0.5 });

	RectF hitBox(Arg::center = GetPlayerPosition(), GetPlayerHitBox());
	hitBox.movedBy(-CameraPos.getCameraPos()).drawFrame(2, ColorF{ 1, 1, 0, 0.8 });
}
