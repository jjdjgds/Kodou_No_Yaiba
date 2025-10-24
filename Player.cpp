#include "Player.hpp"
#include "Game.hpp"
#include "Collision.hpp"

using namespace Collision;

// 仮の敵の当たり判定
RectF enemyRect{ 1600, 100, 64, 64 };

Player::Player() {}
Player::~Player() {}
RectF Player::getAttackRect() const
{
	return RectF{
		Arg::center = GetPlayerPosition().movedBy(0, -GetPlayerHitBox().y * 0.2),
		SizeF{ 200, 200 }
	};
}

void Player::takeDamage(int dmg)

{
	if (GetPlayerState() == StateMode::Hurt || GetPlayerState() == StateMode::Avoidance)
		return; // 無敵 or 被弾中なら無視
	//int HP = GetPlayerHP() - dmg;
	
	SetPlayerState(StateMode::Hurt);
	SetPlayerHP(GetPlayerHP() - dmg);

}


void Player::PlayerAttack()
{
	const double attackFrameDuration = 0.08;

	if (!m_AttackFlag) return; // 攻撃中でなければ終了

	if (animTime >= attackFrameDuration)
	{
		animTime -= attackFrameDuration;
		m_frameIndex++;

		// 攻撃中の判定
		const RectF pBox = getAttackRect();
		if (RectToRect(pBox, enemyRect))
		{
			Print << U"当たった！";
			// ★必要ならここで敵のtakeDamage()呼び出しなど
		}

		// 攻撃アニメーションが終わったらIdleへ
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
	// アイドルアニメーション
	const double idleFrameDuration = 0.15;
	if (animTime >= idleFrameDuration)
	{
		animTime -= idleFrameDuration;
		m_frameIndex = (m_frameIndex + 1) % m_idlePatterns.size();
	}
}

void Player::PlaeyrAvoidance()
{

	// 回避アニメーション
	// 当たり判定の無効化


}

void Player::PlayerHurt()
{

	if (GetPlayerState() != StateMode::Pareise)
	{
		// ダメージアニメーション
		const double hurtFrameDuration = 0.15;
		if (animTime >= hurtFrameDuration)
		{
			animTime -= hurtFrameDuration;
			m_frameIndex++;
			// ダメージ終了
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
	// デバッグ入力
	// ------------------------------
	if (KeyO.down())
	{
		SetPlayerState(StateMode::Hurt);
		
		m_frameIndex = 0; // ★これも追加
	}


	// ------------------------------
	// 現在の情報取得
	// ------------------------------
	Vec2 pos = GetPlayerPosition();
	Vec2 size = GetPlayerHitBox();

	Vec2 velocity = GetPlayerVelocity();

	// ------------------------------
	// 横移動処理
	// ------------------------------
	velocity.x = input.x * GetPlayerSpeed();
	Vec2 nextPosX = pos + Vec2(velocity.x * Scene::DeltaTime()*10, 0);
	RectF rectX(Arg::center = nextPosX, size); // 中心基準に変更


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
	// 接地判定は足元少し下をチェック
	RectF playerRect(Arg::center = pos.movedBy(0, size.y / 2 + 2),
					 SizeF{ size.x * 0.8, 5 }); // 幅少し狭めて地面チェック用に
	bool nowOnGround = map.CheckCollision(playerRect);
	// ------------------------------
	// ジャンプ処理（接地時＋押した瞬間）
	// ------------------------------
	if (m_onGround && (KeyW.down() || KeyUp.down()))
	{
		constexpr double JumpPowerScale = 100.0;
		constexpr double GravityScale = 45.0;
		velocity.y = -GetPlayerJumpSpeed() * JumpPowerScale;
		m_onGround = false;
	}
	// ------------------------------
	// 縦移動処理（Y方向）
	// ------------------------------
	Vec2 nextPosY = pos + Vec2(0, velocity.y * Scene::DeltaTime());
	RectF rectY(Arg::center = nextPosY, size.x,size.y);

	// 縦方向の当たり判定
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
			nowOnGround = true;

			// めり込み防止：地面の上に押し戻す
			while (map.CheckCollision(rectY))
			{
				nextPosY.y -= 1;
				rectY.setCenter(nextPosY);
			}
			pos.y = nextPosY.y;
		}
		else if (velocity.y < 0)
		{
			// 天井に衝突
			velocity.y = 0;

			// めり込み防止：天井の下に押し戻す
			while (map.CheckCollision(rectY))
			{
				nextPosY.y += 1;
				rectY.setCenter(nextPosY);
			}
			pos.y = nextPosY.y;

			// ★重要：次のフレームで落下を始めるようにする
			velocity.y += m_gravity * Scene::DeltaTime() * 90;

			// 状態を「空中」に維持
			nowOnGround = false;
		}
	}


	// 接地状態を更新
	m_onGround = nowOnGround;

	// ------------------------------
	// 攻撃処理
	// ------------------------------
	if (KeySpace.down() && !IsPlayerAttacking())
	{
		SetPlayerState(StateMode::Attack);
		
		SetPlayerAttackFlag(true);
		m_frameIndex = 0;
		animTime = 0.0;
	}
	
	// ------------------------------
	// アニメーション処理
	// ------------------------------



	switch (GetPlayerState())
	{
	case StateMode::Idle:
		// アイドルアニメーション
		PlayerIdle();
		break;
	case StateMode::Run:
		break;
	case StateMode::Jump:
		break;
	case StateMode::Attack:
		PlayerAttack();

		break;
	case StateMode::Hurt:
		PlayerHurt();
		break;
	case StateMode::Avoidance:
		break;
	case StateMode::Dead:
		break;
	case StateMode::Pareise:

		break;

	default:
		// アイドルアニメーション
		const double idleFrameDuration = 0.15;
		if (animTime >= idleFrameDuration)
		{
			animTime -= idleFrameDuration;
			m_frameIndex = (m_frameIndex + 1) % m_idlePatterns.size();
		}
		break;
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
void Player::draw(Game_Map CameraPos) const
{
	const Texture& PlayerTex = TextureAsset(U"Player");

	// 各フレームサイズ
	constexpr int32 frameWidth = 564;
	constexpr int32 frameHeight = 523;

	// 行ごとのY座標（スプライトシート上の位置）
	const int32 idleY = 0;
	const int32 attackY = frameHeight * 1;
	const int32 hurtX = frameWidth * 4;
	const int32 hurtY = frameHeight * 4;
	// 現在のフレーム選択
	int32 n = 0;
	int32 y = idleY;
	int32 x = 0;




	switch (GetPlayerState())
	{
	case StateMode::Idle:
		n = m_idlePatterns[m_frameIndex];
		break;
	case StateMode::Run:
		break;
	case StateMode::Jump:
		break;
	case StateMode::Attack:
		n = m_attackPatterns[m_frameIndex];
		y = attackY - 70;
		break;
	case StateMode::Hurt:
		n = m_hurtPatterns[m_frameIndex];
		
		y = hurtY - 300; // ←追加
		break;
	case StateMode::Avoidance:
		break;
	case StateMode::Pareise:
		break;
	case StateMode::Dead:
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
	// 攻撃範囲のデバッグ描画
	RectF attackBox = getAttackRect();
	attackBox.movedBy(-CameraPos.getCameraPos()).drawFrame(3, 0, ColorF{ 0.0, 1.0, 0.0, 0.5 });

	RectF playerBox{
		Arg::center = GetPlayerPosition().movedBy(0, -GetPlayerHitBox().y * 0.5),
		SizeF{ GetPlayerHitBox().x, GetPlayerHitBox().y * 1.7}
	};

	// カメラ補正して描画
	playerBox.movedBy(-CameraPos.getCameraPos()).drawFrame(3, 0, ColorF{ 1.0, 1.0, 0.0, 1.0 });


	// ------------------------------
	// デバッグ用　プレイヤー情報表示
	// ------------------------------

	Print << U"Player: " << GetPlayerPosition();
	Print << U"Box: " << playerBox.x;


	// ------------------------------
	// デバッグ用　プレイヤー情報表示
	// ------------------------------

	Print << U"Player: " << GetPlayerPosition();
	Print << U"Box: " << playerBox.x;

	// ------------------------------
	// プレイヤー描画
	// ------------------------------
	PlayerTex(n * frameWidth, y+90, frameWidth, frameHeight)
		.scaled(GetPlayerScale())
		.drawAt(GetPlayerPosition() - CameraPos.getCameraPos());
}
