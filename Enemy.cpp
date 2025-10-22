#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::update()
{

}

void Enemy::draw() const
{
	Print << U"enemy" << m_HP;
	TextureAsset(U"enemy").draw(m_Position);
}
