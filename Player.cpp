#include "Player.hpp"
#include "Game.hpp"
#include "Collision.hpp"
//Player player;

Player::Player() {}
Player::~Player() {}
RectF enemyRect{ 1600, 100, 64, 64 }; // 仮の敵の当たり判定
using namespace Collision;
void Player::update(Game_Map& map)
{
	animTime += Scene::DeltaTime();

	Vec2 input{
		(KeyD.pressed() ? 1.0 : 0.0) - (KeyA.pressed() ? 1.0 : 0.0),
		0.0
	};

	Vec2 velocity = GetPlayerVelocity();

	// --- 横移動 ---
	velocity.x = input.x * GetPlayerSpeed() * 10;
	if (input.x != 0)
		SetPlayerFaceRight(input.x > 0);

	// --- 重力 ---
	velocity.y += m_gravity * Scene::DeltaTime()*90 ;

	Vec2 pos = GetPlayerPosition();
	Vec2 size = GetPlayerHitBox();

	// 現在位置とサイズ
	
	 size = GetPlayerHitBox();
 // 当たり判定のRectFだけに微調整を適用する場合は別変数を使う
	 RectF playerRect(pos.movedBy(0, -10), size);

	bool nowOnGround = map.CheckCollision(playerRect);
	// --- ジャンプ処理（押した瞬間のみ、かつ接地中のみ） ---
	if (m_onGround && (KeyW.down() || KeyUp.down()))
	{
		velocity.y = -GetPlayerJumpSpeed();
		m_onGround = false; // 空中状態に移行
	}

	// --- Y軸移動（重力・衝突） ---
	Vec2 nextPosY = pos + Vec2(0, velocity.y * Scene::DeltaTime());
	RectF rectY(nextPosY, size);

	if (!map.CheckCollision(rectY))
	{
		pos.y = nextPosY.y;
	}
	else
	{
		if (velocity.y > 0) // 落下中にぶつかったら
		{
			velocity.y = 0;
			nowOnGround = true; // 着地
		}
	}

	// 最後に現在の接地状態を更新
	m_onGround = nowOnGround;
	// --- 攻撃処理 ---
	if (KeySpace.down() && !IsPlayerAttacking()) // Jキーで攻撃
	{
		m_state = StateMode::Attack;
		SetPlayerAttackFlag(true);
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// 攻撃アニメーション
	if (m_AttackFlag)
	{
		const double attackFrameDuration = 0.08;
		if (animTime >= attackFrameDuration)
		{
			animTime -= attackFrameDuration;
			m_frameIndex++;

			// 攻撃判定ボックス
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

	// 更新結果を反映
	SetPlayerVelocity(velocity);
	SetPlayerPosition(pos);
}

void Player::draw() const
{
	const Texture& Idle = TextureAsset(U"Idle");
	const Texture& Attack = TextureAsset(U"Attack");
	const Texture& Run = TextureAsset(U"Run");
	const Texture& Hurt = TextureAsset(U"Hurt");
	const Texture& PlayerTex = TextureAsset(U"Player");

	constexpr int32 frameWidth = 564;
	constexpr int32 frameHeight = 523;
	
	
	// 行ごとにY座標を明示
	const int32 idleY = 0;
	const int32 attackY = frameHeight * 1; // 2段目（0-based）

	int32 n = 0;
	int32 y = idleY;
	
	if (m_AttackFlag)
	{
		n = m_attackPatterns[m_frameIndex];
		y = attackY-70;
		
		/*Attack(n * frameWidth, 0, frameWidth, frameHeight)
			.scaled(4.0)
			.drawAt(m_Position);*/
	}
	else
	{
		n = m_idlePatterns[m_frameIndex];
		

		/*Idle(n * frameWidth, 0, frameWidth, frameHeight)
			.scaled(4.0)
			.drawAt(m_Position);*/
	}

	
	//RectF{player.getPosition().x ,player.getPosition().y, frameHeight}.draw();
	// 位置を固定して描画（Yは変えない）
	//debug用当たり判定表示
	RectF AttackHitBox{ GetPlayerPosition (), 200, 131};
	AttackHitBox.drawFrame(3, 0, ColorF{ 0.0, 1.0, 0.0, 0.5 });//仮の当たり判定表示

	RectF PlayerHitBox{ GetPlayerPosition(), GetPlayerHitBox()};
	PlayerHitBox.drawFrame(3, 0, ColorF{ 1.0, 1.0, 0.0, 1.0 });//仮の当たり判定表示



	// サイズと位置を別々に設定
	PlayerTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(GetPlayerScale())
		.drawAt(GetPlayerPosition());

}
