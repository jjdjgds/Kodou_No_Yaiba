# include "Game.hpp"
#include "Player.hpp"
#include "Enemy.hpp"


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
		100.0f,              // Speed
		3.0f,              // DamageTimeOut
		false,             // Jump
		true,              // FaceRight
		false              // Invincible
	)
{
	m_enemies.clear();// 配列をクリア
	m_enemies.reserve(8);// 敵キャラクター用

	// 敵キャラクターを追加(位置、スピード、巡回範囲.左、巡回範囲.右、大きさ)
	m_enemies.emplace_back(Vec2{ 700,600 }, 80.0, 600.0, 900.0, false, Vec2{ 3,3 });
	m_enemies.emplace_back(Vec2{ 700,100 }, 200.0, 600.0, 900.0, true, Vec2{ 5,5 });
}



void Game::update()
{
	
	player.update();
	for (auto& e : m_enemies) e.update();// 敵キャラクターを更新
}

void Game::draw() const
{
	
	Scene::SetBackground(ColorF(1.0,0.2,0.2,1.0));
	
	// テクスチャアセットを使用する
	// 登録した名前で呼び出せる
	TextureAsset(U"Windmill").scaled(player.getScale()).draw(0,0);
	player.draw();
	for (const auto& e : m_enemies) e.draw();// 敵キャラクターを描画
}


