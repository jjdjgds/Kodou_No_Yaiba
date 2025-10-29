#include "Game.hpp"
#include "Player.hpp"
#include "Enemy_1.hpp"
#include "Enemy_2.hpp"
#include "Collision.hpp"
using namespace Collision;


Game::Game(const InitData& init)
	: IScene{ init }
	, player(
	Vec2(800, 750), // 位置
	Vec2(100, 100), // スプライトスケール(px)
	Vec2(0.0, 0.0),
	Vec2(6.0, 10.0),  // ← 当たり判定（体の中心付近を覆うサイズ）
		3,
		3,
		150,
		3,
		1.0f,
		0.5f,
		player.GetPlayerDefoSpeed(),
		3.0f,
	false, true, false
	)


{

	// マップ読み込み
	if (!map.loadStageFromFile(FileSystem::CurrentDirectory() + U"example/Map/stage2.txt"))
	{
		Print << U"Failed to load stage1";
		return;
	}
	Boss_spawner.loadFromMap(map.getBlocks(), map.getChipWidth(), map.getChipHeight());

	// 敵初期化
	m_enemies1.clear();
	m_enemies1.reserve(8);
	//m_enemies1.emplace_back(Vec2{ 200,700 },300.0);

	m_enemies2.clear();
	m_enemies2.reserve(8);
	//m_enemies2.emplace_back(Vec2{ 200,700 }, 600.0);

}

void Game::update()
{
	bg.update();
	map.updateCamera(player.GetPlayerPosition() + player.GetPlayerScale() / 2);
	map.update();
	Ui.update(player, map);
	player.update(map);
	Boss_spawner.update(player, map);

	for (auto& e : m_enemies1) e.update(player, map);
	for (auto& e : m_enemies2) e.update(player, map);

}

void Game::draw() const
{
	Scene::SetBackground(ColorF(0.5, 0.5, 0.5, 1.0));
	bg.draw();
	map.draw();                // ← マップを描画
	player.draw(map);             // ← プレイヤーを描画

	for (const auto& e : m_enemies1) e.draw(map); //敵描画
	for (const auto& e : m_enemies2) e.draw(map); //敵描画

	Ui.draw(player,map);

	Boss_spawner.draw(map);
	Ui.draw(player,map);


}







