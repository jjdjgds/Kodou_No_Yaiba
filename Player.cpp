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


	const uint64 t = Time::GetMillisec();
	m_frameIndex = (t / 150) % m_patterns.size();

}

void Player::draw() const
{
	const Texture& Tex = TextureAsset(U"Player");
	const int32 n = m_patterns[m_frameIndex];
	Tex(n * 564, 0, 564, 523)
		.scaled(2.0)
		.draw(m_Position);

	Print << U"a" << m_HP;
	//TextureAsset(U"Player").draw(m_Position);
	
}
