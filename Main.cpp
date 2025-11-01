#include "Common.hpp"
#include "Title.hpp"
#include "Game.hpp"
#include "Player.hpp"

void Main()
{
	// ウィンドウサイズの設定
	Window::Resize(1600, 900);

	// テクスチャアセットを登録する
	//第1引数　呼び出す時に使う名前
	//第2引数　画像ファイルのパス
	/*TextureAsset::Register(U"Windmill", U"example/texture/haikei.jpg");
	TextureAsset::Register(U"Idle", U"example/spritesheet/IDLE.png");
	TextureAsset::Register(U"Run", U"example/spritesheet/RUN.png");
	TextureAsset::Register(U"Attack", U"example/spritesheet/ATTACK_1.png");
	TextureAsset::Register(U"Hurt", U"example/spritesheet/HURT.png");*/

	
	{
		//TextureAsset::Register(U"Player", U"example/spritesheet/samurai.png");
		TextureAsset::Register(U"HeatBeat", U"example/spritesheet/HEARTBEAT.png");
		TextureAsset::Register(U"AttackEffect", U"example/spritesheet/AttackEffect.png");
		TextureAsset::Register(U"Player", U"example/spritesheet/PLAYER.png");
		TextureAsset::Register(U"PlayerHP", U"example/spritesheet/hp.png");
		TextureAsset::Register(U"Medicine", U"example/spritesheet/medicine.png");

	}
	{


		AudioAsset::Register(U"FastBeat", U"example/Audio/Heartbeat-Fast-Loop.MP3", Loop::Yes);
		AudioAsset::Register(U"SlowBeat", U"example/Audio/Heartbeat-Slow-Loop.MP3", Loop::Yes);
		AudioAsset::Register(U"Sowrd1", U"example/Audio/Sword1.WAV");
		AudioAsset::Register(U"Sowrd2", U"example/Audio/Sword2.MP3");
		AudioAsset::Register(U"Sowrd3", U"example/Audio/Sword3.WAV");
		AudioAsset::Register(U"Sowrd4", U"example/Audio/Sword4.WAV");


	}

	{
		TextureAsset::Register(U"Enemy1", U"example/spritesheet/Enemy1.png");
		TextureAsset::Register(U"Enemy2", U"example/spritesheet/Enemy2.png");
		TextureAsset::Register(U"Enemy2Bullet", U"example/spritesheet/BULLET.png");
	}

	TextureAsset::Register(U"TitleBg", U"example/bg/title_bg.png");
	TextureAsset::Register(U"Wall", U"example/Wall.jpg");
	//ATTACK_1.png
	//TextureAsset::Register(U"Player", U"example/texture/samurai_sheet.png");
	{
		TextureAsset::Register(U"Boss", U"example/spritesheet/boss_1.png");
		TextureAsset::Register(U"Smoke_bomb", U"example/spritesheet/smoke_bomb.png");
		TextureAsset::Register(U"Smoke", U"example/spritesheet/smoke.png");
		TextureAsset::Register(U"shuriken", U"example/spritesheet/shuriken.png");
		AudioAsset::Register(U"boss_bgm", U"example/Audio/boss_bgm.MP3", Loop::Yes);
		AudioAsset::Register(U"pattern_1_throw", U"example/Audio/pattern_1_throw.MP3");
		AudioAsset::Register(U"pattern_3", U"example/Audio/pattern_3.MP3");
		AudioAsset::Register(U"pattern_4", U"example/Audio/pattern_4.MP3");
		AudioAsset::Register(U"pattern_6", U"example/Audio/pattern_6.MP3", Loop::Yes);
	}
	

	//玉ねぎ楷書_激_無料版v7
	FontAsset::Register(U"TitleFont", FontMethod::MSDF, 48, U"example/font/玉ねぎ楷書「激」無料版v7/玉ねぎ楷書激無料版v7改.ttf");

	// "TitleFont" という名前でフォントを登録
	//FontAsset::Register(U"TitleFont", FontMethod::MSDF, 48, U"example/font/RocknRoll/RocknRollOne-Regular.ttf");

	//上で登録した TitleFont に対して、描画時の文字周囲のバッファの厚さ
	FontAsset(U"TitleFont").setBufferThickness(4);

	//"Bold" という名前で太字フォントを登録
	FontAsset::Register(U"Bold", FontMethod::MSDF, 48, Typeface::Bold);

	// シーンマネージャの作成
	//ここに追加していく
	App manager;
	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);

	
	//現在のシーンを更新・描画し、必要に応じてシーンを切り替え
	while (System::Update())
	{
		
		if (not manager.update())
		{
			break;
		}
	}
}
