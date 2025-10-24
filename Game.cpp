#include "Game.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Collision.hpp"
using namespace Collision;
#include "Game_Map.hpp"


Game::Game(const InitData& init)
	: IScene{ init }
	, player( // ← 初期化リストで player を作成
		Vec2(100.0, 200.0), // position
		Vec2(0.5, 0.5),     // scale
		Vec2(0.0, 0.0),     // velocity
		Vec2(50.0, 60.0),  // HitBox
		3,                 // HP
		100,               // BPM
		3,                 // Attack
		1.0f,              // AttackRange
		0.5f,              // AttackSpeed
		30.0f,             // Speed
		3.0f,              // DamageTimeOut
		false,             // Jump
		true,              // FaceRight
		false              // Invincible
	)
{
	// マップ読み込み
	if (!map.loadStageFromFile(FileSystem::CurrentDirectory() + U"example/Map/stage1.txt"))
	{
		Print << U"Failed to load stage1";
		return;
	}

	// 敵初期化
	m_enemies.clear();
	m_enemies.reserve(8);

	m_enemies.emplace_back(Vec2{ 700,100 }, 200.0, 600.0, 900.0, true, Vec2{ 3,3 });
	m_enemies.emplace_back(Vec2{ 300,100 }, 200.0, 400.0, 900.0, true, Vec2{ 3,3 });
}

void Game::update()
{
	player.update(map);
	map.updateCamera(player.GetPlayerPosition());
	map.update();




	for (auto& e : m_enemies) e.update(player,map);

}

void Game::draw() const
{
	Scene::SetBackground(ColorF(0.2, 0.2, 0.2, 1.0));

	map.draw();                // ← マップを描画
	player.draw();             // ← プレイヤーを描画

	for (const auto& e : m_enemies) e.draw(); //敵描画
}
