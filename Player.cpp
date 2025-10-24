#include "Player.hpp"
#include "Game.hpp"
#include "Collision.hpp"

using namespace Collision;

// 仮の敵の当たり判定
RectF enemyRect{ 1600, 100, 64, 64 };

Player::Player() {}
Player::~Player() {}

void Player::update(Game_Map& map)
{
	// ------------------------------
	// 時間更新（アニメーション制御用）
	// ------------------------------
	animTime += Scene::DeltaTime();

	// ------------------------------
	// 入力処理
	// ------------------------------
	Vec2 input{
		(KeyD.pressed() ? 1.0 : 0.0) - (KeyA.pressed() ? 1.0 : 0.0),
		0.0
	};

	// ------------------------------
	// 現在の情報取得
	// ------------------------------
	Vec2 pos = GetPlayerPosition();
	Vec2 size = GetPlayerHitBox();
	Vec2 velocity = GetPlayerVelocity();

	// ------------------------------
	// 横移動処理
	// ------------------------------
	velocity.x = input.x * GetPlayerSpeed() * 10;
	Vec2 nextPosX = pos + Vec2(velocity.x * Scene::DeltaTime(), 0);
	RectF rectX(nextPosX, size);

	// 壁との衝突判定（X方向）
	if (!map.CheckCollision(rectX))
	{
		pos.x = nextPosX.x;
	}

	// 向きの更新
	if (input.x != 0)
	{
		SetPlayerFaceRight(input.x > 0);
	}

	// ------------------------------
	// 重力処理
	// ------------------------------
	velocity.y += m_gravity * Scene::DeltaTime() * 90;

	// ------------------------------
	// 接地判定
	// ------------------------------
	RectF playerRect(pos.movedBy(0, -10), size); // 判定を少し下にずらす
	bool nowOnGround = map.CheckCollision(playerRect);

	// ------------------------------
	// ジャンプ処理（接地時＋押した瞬間）
	// ------------------------------
	if (m_onGround && (KeyW.down() || KeyUp.down()))
	{
		velocity.y = -GetPlayerJumpSpeed();
		m_onGround = false;
	}

	// ------------------------------
	// 縦移動処理（Y方向）
	// ------------------------------
	Vec2 nextPosY = pos + Vec2(0, velocity.y * Scene::DeltaTime());
	RectF rectY(nextPosY, size);

	if (!map.CheckCollision(rectY))
	{
		pos.y = nextPosY.y;
	}
	else
	{
		// 地面に衝突したら着地
		if (velocity.y > 0)
		{
			velocity.y = 0;
			nowOnGround = true;
		}
	}

	// 接地状態を更新
	m_onGround = nowOnGround;

	// ------------------------------
	// 攻撃処理
	// ------------------------------
	if (KeySpace.down() && !IsPlayerAttacking())
	{
		m_state = StateMode::Attack;
		SetPlayerAttackFlag(true);
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// ------------------------------
	// アニメーション処理
	// ------------------------------
	if (m_AttackFlag)
	{
		// 攻撃アニメーション
		const double attackFrameDuration = 0.08;
		if (animTime >= attackFrameDuration)
		{
			animTime -= attackFrameDuration;
			m_frameIndex++;

			// 攻撃判定
			RectF pBox(GetPlayerPosition(), GetPlayerAttackRengeBox());
			if (RectToRect(pBox, enemyRect))
			{
				Print << U"当たった！";
			}

			// 攻撃終了
			if (m_frameIndex >= m_attackPatterns.size())
			{
				m_frameIndex = 0;
				SetPlayerAttackFlag(false);
				m_state = StateMode::Idle;
			}
		}
	}
	else
	{
		// アイドルアニメーション
		const double idleFrameDuration = 0.15;
		if (animTime >= idleFrameDuration)
		{
			animTime -= idleFrameDuration;
			m_frameIndex = (m_frameIndex + 1) % m_idlePatterns.size();
		}
	}

	// ------------------------------
	// 更新結果を反映
	// ------------------------------
	SetPlayerVelocity(velocity);
	SetPlayerPosition(pos);
}

// ================================================================
// 描画処理
// ================================================================
void Player::draw() const
{
	const Texture& PlayerTex = TextureAsset(U"Player");

	// 各フレームサイズ
	constexpr int32 frameWidth = 564;
	constexpr int32 frameHeight = 523;

	// 行ごとのY座標（スプライトシート上の位置）
	const int32 idleY = 0;
	const int32 attackY = frameHeight * 1;

	// 現在のフレーム選択
	int32 n = 0;
	int32 y = idleY;

	if (m_AttackFlag)
	{
		n = m_attackPatterns[m_frameIndex];
		y = attackY - 70;
	}
	else
	{
		n = m_idlePatterns[m_frameIndex];
	}

	// ------------------------------
	// デバッグ用当たり判定表示
	// ------------------------------
	RectF attackBox{ GetPlayerPosition(), 200, 131 };
	attackBox.drawFrame(3, 0, ColorF{ 0.0, 1.0, 0.0, 0.5 });

	RectF playerBox{ GetPlayerPosition(), GetPlayerHitBox() };
	playerBox.drawFrame(3, 0, ColorF{ 1.0, 1.0, 0.0, 1.0 });

	// ------------------------------
	// プレイヤー描画
	// ------------------------------
	PlayerTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(GetPlayerScale())
		.drawAt(GetPlayerPosition());
}
