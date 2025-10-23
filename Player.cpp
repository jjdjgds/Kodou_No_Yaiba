#include "Player.hpp"
#include "Game.hpp"

Player player;

Player::Player() {}
Player::~Player() {}
 
void Player::update()
{
	animTime += Scene::DeltaTime();

	

	// スペースで攻撃モードへ
	if (KeySpace.down()&& (!m_AttackFlag))
	{

		m_AttackFlag = true;
		m_frameIndex = 0;
		animTime = 0.0;
	}
	if (KeyA.pressed())
	{
		m_FaceRight = false;
		m_Position.x -= m_Speed;
	}
	if (KeyD.pressed())
	{
		m_FaceRight = true;
		m_Position.x += m_Speed;
	}

	// 攻撃アニメーション中
	if (m_AttackFlag)
	{
		const double attackFrameDuration = 0.1;
		if (animTime >= attackFrameDuration)
		{
			animTime -= attackFrameDuration;
			m_frameIndex++;

			if (m_frameIndex >= m_attackPatterns.size())
			{
				m_frameIndex = 0;
				m_AttackFlag = false;
			}
		}
	}
	else
	{
		const double idleFrameDuration = 0.15;
		if (animTime >= idleFrameDuration)
		{
			animTime -= idleFrameDuration;
			m_frameIndex = (m_frameIndex + 1) % m_idlePatterns.size();
		}
	}
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
	
	/*constexpr int frameWidth = 96;
	constexpr int frameHeight = 96;*/
	// 行ごとにY座標を明示
	const int32 idleY = 0;
	const int32 attackY = frameHeight * 1; // 2段目（0-based）

	int32 n = 0;
	int32 y = idleY;
	RectF{ getPosition().x ,getPosition().y, 100 }.draw();
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
	RectF{ getPosition().x ,getPosition().y, 100 }.draw();
	PlayerTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(1.0)
		.drawAt(m_Position);
	

	



}
