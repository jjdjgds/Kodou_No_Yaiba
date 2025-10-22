#include "Player.hpp"
#include "Game.hpp"

Player player;
Player::Player()
	
{

	const String spriteSheetPath = U"example/texture/samurai_sheet.png";
	// スプライトシート上の1フレームの大きさ
	const Vec2 frameSize(64, 64);

	// AnimatedObject クラスのインスタンスを生成
	AnimatedObject animatedObject(spriteSheetPath, frameSize);

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
	TextureAsset(U"Player").draw(m_Position);
	
}
