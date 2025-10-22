# pragma once
# include <Siv3D.hpp>

// シーンのステート
//ここにシーンを追加していく
enum class State
{
	Title,
	Game,
	//Ranking,
};

// 共有するデータ
//シーン間で共有したいデータをここに追加していく
struct GameData
{
	// 直前のゲームのスコア
	int32 lastScore = 0;

	// ハイスコア
	Array<int32> highScores = { 10, 8, 6, 4, 2 };
};

using App = SceneManager<State, GameData>;
