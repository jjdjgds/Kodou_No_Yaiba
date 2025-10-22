#include "Player.hpp"
#include "Game.hpp"

Player player;
Player::Player()
	
{
}



Player::~Player()
{
	
}

void Player::update()
{
	
}

void Player::draw() const
{


	Print << U"a" << m_HP;
	TextureAsset(U"Akkarin").draw(m_Position);
}
