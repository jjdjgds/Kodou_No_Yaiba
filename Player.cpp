#include "Player.hpp"
#include "Game.hpp"

Player player; // ゲーム全体で共有する1体のプレイヤー

Player::Player() {}
Player::~Player() {}

void Player::update()
{
	animTime += Scene::DeltaTime();

	// 攻撃中か確認
	bool isAttacking = getAttackFlag();

	// 攻撃開始：スペースキーを押していて、かつ攻撃中でない場合のみ
	if (KeySpace.down() && !isAttacking)
	{
		setAttackFlag(true);
		m_frameIndex = 0;
		animTime = 0.0;
	}

	// 攻撃アニメーション中
	if (getAttackFlag())
	{
		const double attackFrameDuration = 0.1; // フレーム間隔
		if (animTime >= attackFrameDuration)
		{
			animTime -= attackFrameDuration;
			m_frameIndex++;

			if (m_frameIndex >= m_attackPatterns.size())
			{
				m_frameIndex = 0;
				setAttackFlag(false); // 攻撃終了
			}
		}
	}
	else // 待機アニメーション中
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
	const Texture& Tex = TextureAsset(U"Player");

	constexpr int32 frameWidth = 564;
	constexpr int32 frameHeight = 523;

	const int32 idleY = 0;
	const int32 attackY = frameHeight * 1; // 2段目に攻撃アニメがある場合

	int32 n = 0;
	int32 y = idleY;

	if (getAttackFlag())
	{
		n = m_attackPatterns[m_frameIndex];
		y = attackY - 75; // 攻撃時に位置補正したいなら調整
	}
	else
	{
		n = m_idlePatterns[m_frameIndex];
	}

	Tex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(1.0)
		.draw(m_Position);
}
