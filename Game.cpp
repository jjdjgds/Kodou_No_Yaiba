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
		3,
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

	const Vec2 center = Scene::CenterF();
	const SizeF btnSize{ 200, 56 };
	m_btnRetry = RectF{ Arg::center = center.movedBy(0, +230), btnSize };
	m_btnTitle = RectF{ Arg::center = center.movedBy(0, +320), btnSize };

	if (!FontAsset::IsRegistered(U"Menu")) {
		FontAsset::Register(U"Menu", 36, Typeface::Bold);
	}

}

void Game::update()
{
	const int stage = map.getCurrentStage();

	if (!audio_battle.isPlaying() && stage !=4)
	{
		audio_battle.play();
		audio_battle.setVolume(0.7);
	}
	else if (!audio_boss.isPlaying() && stage == 4)
	{
		audio_battle.stop();
		audio_boss.play();
		audio_boss.setVolume(0.7);
	}

	const RectF pBoxWorld(Arg::center = player.GetPlayerPosition(),
					  player.GetPlayerHitBox());

	
	map.updateCamera(player.GetPlayerPosition() + player.GetPlayerScale() / 2);
	map.update();
	Ui.update(player, map);
	player.update(map, m_enemies1,m_enemies2);
	Boss_spawner.update(player, map,effects);
	effects.UpdateEffect();


	if (player.GetPlayerState() == StateMode::Dead && player.IsDead()) {
		m_showDeath = true;
	}

	//Print << U"dead" << Boss_spawner.isBossDead();
	if (Boss_spawner.isBossDead() && !player.IsDead())
	{
		m_bossDeath = true;
	}

	


	if (m_showDeath) {
		updateDeathOverlay();
		return;
	}
	if (m_bossDeath) {
		updateClearOverlay();
		return;
	}
	
	if (Boss_spawner.areAllCleared()&& map.intersectsGoal(pBoxWorld)) {
		map.loadNextStage();

		map.updateCamera(player.GetPlayerPosition() + player.GetPlayerScale() / 2);
		map.update();
		if (auto spawn = map.findPlayerSpawn()) {
			player.SetPlayerHP(player.GetPlayerMaxHP());
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

	{
		ScopedRenderStates2D blend{ BlendState::Additive };
		effects.DrawEffect(map.getCameraPos());
	}

	if (m_showDeath) {
		drawDeathOverlay();
	}
	if (m_bossDeath) {
		drawClearOverlay();
	}
}


void Game::restartCurrentStage() {
	const int stage = map.getCurrentStage();
	const FilePath path = U"example/Map/stage{}"_fmt(stage);
	map.loadStageFromFile(path, stage);
	map.updateCamera(player.GetPlayerPosition() + player.GetPlayerScale() / 2);
	map.update();

	if (auto spawn = map.findPlayerSpawn()) {
		player.SetPlayerPosition(*spawn);
	}
	else {
		player.SetPlayerPosition(Vec2{ 800, 750 });
	}
	player.SetPlayerHP(player.GetPlayerMaxHP());
	player.SetPlayerBPM(80);
	player.Revive();

	Boss_spawner.loadFromMap(map.getBlocks(), map.getChipWidth(), map.getChipHeight());

	resetDeathOverlay();
}

void Game::updateDeathOverlay() {
	Ui.hBgmStop();
	if (KeyW.down()||KeyUp.down())   m_deathSel = DeathChoice::Retry;
	if (KeyS.down()||KeyDown.down()) m_deathSel = DeathChoice::Title;

	const Point mpos = Cursor::Pos();
	if (m_btnRetry.intersects(mpos)) m_deathSel = DeathChoice::Retry;
	if (m_btnTitle.intersects(mpos)) m_deathSel = DeathChoice::Title;

	const bool trigger = KeyEnter.down() || MouseL.down()|| KeySpace.down();
	if (!trigger) return;

	if (m_deathSel == DeathChoice::Retry) {
		restartCurrentStage();
	}
	else {
		resetDeathOverlay();
		audio_battle.stop();
		audio_boss.stop();
		changeScene(State::Title);
	}
}

void Game::drawDeathOverlay() const {
	if (TextureAsset::IsRegistered(U"DeathBg")) {
		TextureAsset(U"DeathBg").resized(Scene::Size()).draw();
	}
	else {
		FontAsset(U"TitleFont")(U"DEATH").drawAt(Scene::CenterF().movedBy(0, -40), Palette::Red);
	}

	const auto drawBtn = [&](const RectF& r, StringView label, bool focused) {
		r.draw(focused ? ColorF(1, 1, 1, 0.15) : ColorF(1, 1, 1, 0.05));
		r.drawFrame(2, focused ? Palette::Orange : Palette::Gray);
		FontAsset(U"Menu")(label).drawAt(r.center(), focused ? Palette::Orange : Palette::White);
		};

	drawBtn(m_btnRetry, U"Retry", m_deathSel == DeathChoice::Retry);
	drawBtn(m_btnTitle, U"Title", m_deathSel == DeathChoice::Title);
}

void Game::updateClearOverlay() {

	static bool s_audioStopped = false;
	if (!s_audioStopped) {
		audio_battle.stop();
		audio_boss.stop();
		Ui.hBgmStop();
		s_audioStopped = true;
	}

	const Vec2  center = Scene::CenterF();
	const SizeF btnSize{ 240, 60 };
	const RectF startBtn(Arg::center = center.movedBy(0, +390), btnSize);

	const Point mpos = Cursor::Pos();
	if (m_btnRetry.intersects(startBtn)) m_selected = true;

	m_selected = m_btnTitle.intersects(Cursor::PosF());

	m_selected = startBtn.intersects(Cursor::PosF());
	if (KeySpace.down()) m_selected = true;

	const bool trigger = KeyEnter.down() || KeySpace.down() || MouseL.down();
	if (!trigger) return;

	if(m_selected)
	{
		changeScene(State::Title);
		s_audioStopped = false;
		m_bossDeath = false;
	}

}

void Game::drawClearOverlay() const {
	if (TextureAsset::IsRegistered(U"ClearBg")) {
		TextureAsset(U"ClearBg").resized(Scene::Size()).draw();
	}
	else {
		FontAsset(U"TitleFont")(U"CLEAR").drawAt(Scene::CenterF().movedBy(0, -40), Palette::Red);
	}

	const Vec2  center = Scene::CenterF();
	const SizeF btnSize{ 240, 60 };
	const RectF startBtn(Arg::center = center.movedBy(0, +390), btnSize);

	auto drawButton = [](const RectF& r, StringView text, bool selected)
		{
			const ColorF fill = selected ? ColorF{ 1.0, 1.0, 1.0, 0.25 }
			: ColorF{ 1.0, 1.0, 1.0, 0.12 };
			const ColorF frame = selected ? ColorF{ 1.0 } : ColorF{ 0.7 };
			r.rounded(12).draw(fill).drawFrame(2, 0, frame);

			String label = selected ? U"▶ " + String{ text } : String{ text };
			FontAsset(U"Bold")(label).drawAt(28, r.center(), Palette::White);
		};

	drawButton(startBtn, U"TITLE", m_selected == true);
}




