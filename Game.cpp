#include "Game.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
<<<<<<< HEAD
#include "Collision.hpp"
using namespace Collision;

=======
#include "Game_Map.hpp"
>>>>>>> c494077ae074cd3093d5f1246eede207b47988ef

Game::Game(const InitData& init)
	: IScene{ init }
	, player( // ← 初期化リストで player を作成
		Vec2(100.0, 100.0), // position
		Vec2(0.5, 0.5),     // scale
		Vec2(0.0, 0.0),     // velocity
		Vec2(50.0, 70.0),  // HitBox
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

	m_enemies.emplace_back(Vec2{ 700,600 }, 80.0, 600.0, 900.0, false, Vec2{ 3,3 });
	m_enemies.emplace_back(Vec2{ 700,100 }, 200.0, 600.0, 900.0, true, Vec2{ 5,5 });
}

void Game::update()
{
<<<<<<< HEAD
	RectF pBox(player.getPosition(), player.getScale());
	pBox.setPos(player.getPosition()).setSize(player.getAttackRengeBox());

	


	player.update();
<<<<<<< HEAD

	for (auto& e : m_enemies) {

		RectF eBox(e.getPosition(), e.getScale());
		eBox.setPos(e.getPosition()).setSize(e.getHitbox());

		if (RectToRect(pBox,eBox)) e.takeDamage(0);// ダメージを与える

		e.update();
	}
=======
	
	
	// 全ての敵キャラクターの状態を1フレーム分進める
	for (auto& e : m_enemies) e.update(player);// 敵キャラクターを更新
>>>>>>> 72fd5ce0486a6937be8b43edcce319793524be64
=======
	player.update(map);
	map.updateCamera(player.GetPlayerPosition());
	map.update();

	for (auto& e : m_enemies)
		e.update(player);
>>>>>>> c494077ae074cd3093d5f1246eede207b47988ef
}

void Game::draw() const
{
	Scene::SetBackground(ColorF(0.2, 0.2, 0.2, 1.0));

	map.draw();                // ← マップを描画
	player.draw();             // ← プレイヤーを描画

	for (const auto& e : m_enemies)
		e.draw();
}
