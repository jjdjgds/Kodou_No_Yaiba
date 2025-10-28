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
		TextureAsset::Register(U"Player", U"example/spritesheet/samurai.png");
		TextureAsset::Register(U"HeatBeat", U"example/spritesheet/HEARTBEAT.png");
	}


	{
		TextureAsset::Register(U"EnemyIdle", U"example/spritesheet/Enemy_IDLE.png");
		TextureAsset::Register(U"EnemyRun", U"example/spritesheet/Enemy_RUN.png");
		TextureAsset::Register(U"EnemyHurt", U"example/spritesheet/HURT.png");
		TextureAsset::Register(U"EnemyAttack", U"example/spritesheet/ATTACK_1.png");
	}

	TextureAsset::Register(U"Wall", U"example/Wall.jpg");
	//ATTACK_1.png
	//TextureAsset::Register(U"Player", U"example/texture/samurai_sheet.png");

	

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
	manager.add<Game>(State::Game);
	manager.add<Title>(State::Title);

	
	//現在のシーンを更新・描画し、必要に応じてシーンを切り替え
	while (System::Update())
	{
		
		if (not manager.update())
		{
			break;
		}
	}
}
