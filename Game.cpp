# include "Game.hpp"
#include "Player.hpp"


Game::Game(const InitData& init)
	: IScene{ init }
	, player( // ← 初期化リストで player を作成
		Vec2(100.0, 100.0), // position
		Vec2(1.0, 1.0),     // scale
		3,                 // HP
		100,               // BPM
		3,                 // Attack
		1.0f,              // AttackRange
		1.0f,              // AttackSpeed
		5.0f,              // Speed
		3.0f,              // DamageTimeOut
		false,             // Jump
		true,              // FaceRight
		false              // Invincible
	)
{
}


void Game::update()
{
	
	player.update();

}

void Game::draw() const
{
	
	Scene::SetBackground(ColorF(1.0,0.2,0.2,1.0));
	
	// テクスチャアセットを使用する
	// 登録した名前で呼び出せる
	TextureAsset(U"Windmill").scaled(player.getScale()).draw(0,0);
	player.draw();
}


