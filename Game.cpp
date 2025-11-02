#include "Game.hpp"
#include "Player.hpp"
#include "Enemy_1.hpp"
#include "Enemy_2.hpp"
#include "Collision.hpp"
#include "TimeStopManager.h"
#include "AllEffect.h"
using namespace Collision;


Game::Game(const InitData& init)
	: IScene{ init }
	, player(
	Vec2(800, 750), // 位置
	Vec2(100, 100), // スプライトスケール(px)
	Vec2(0.0, 0.0),
	Vec2(8.0, 10.0),  // ← 当たり判定（体の中心付近を覆うサイズ）
		5,
		3,
		90,
		3,
		1.0f,
		0.5f,
		player.GetPlayerDefoSpeed(),
		3.0f,
	false, true, false
	)


{

	// マップ読み込み
	if (!map.loadStageFromFile(FileSystem::CurrentDirectory()+U"example/Map/stage1.txt",1))
	{
		Print << U"Failed to load stage1";
		return;
	}
	Boss_spawner.loadFromMap(map.getBlocks(), map.getChipWidth(), map.getChipHeight());

	if (auto spawn = map.findPlayerSpawn()) {
		player.SetPlayerPosition(*spawn);
	}
	else {
		player.SetPlayerPosition(Vec2{ 800, 750 });
	}

}

void Game::update()
{
	const RectF pBoxWorld(Arg::center = player.GetPlayerPosition(),
					  player.GetPlayerHitBox());

	
	map.updateCamera(player.GetPlayerPosition() + player.GetPlayerScale() / 2);
	map.update();
	Ui.update(player, map);
	player.update(map, m_enemies1,m_enemies2);
	Boss_spawner.update(player, map,effects);
	effects.UpdateEffect();


	if (map.intersectsGoal(pBoxWorld)) {
		map.loadNextStage();
		map.updateCamera(player.GetPlayerPosition() + player.GetPlayerScale() / 2);
		map.update();
		if (auto spawn = map.findPlayerSpawn()) {
			player.SetPlayerPosition(*spawn);
		}
		else {
			player.SetPlayerPosition({ 100, 100 });
		}

		Boss_spawner.loadFromMap(map.getBlocks(), map.getChipWidth(), map.getChipHeight());
	}

}

void Game::draw() const
{
	Scene::SetBackground(ColorF(0.5, 0.5, 0.5, 1.0));
	map.draw();                // ← マップを描画
	Boss_spawner.draw(map);
	if (player.IsTimeStoped())
	{
		RectF{ 0,0,Scene::Width(),Scene::Height() }.draw(ColorF{ 0,0,0,0.7 });

	}
	player.draw(map);             // ← プレイヤーを描画




	

	Ui.draw(player,map);
	ScopedRenderStates2D blend{ BlendState::Additive };
	effects.DrawEffect(map.getCameraPos());


}







