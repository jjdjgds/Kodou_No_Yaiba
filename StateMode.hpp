// StateMode.hpp
#pragma once

enum class StateMode {
	Idle,			// 0 : アイドル状態
	IdleToRun,		// 1 : 立ち状態から走る状態への遷移
	RunStart,       // 2 :走りはじめ状態
	Run,			// 3 : 走る状態
	Jump,			// 4 : ジャンプ状態
	Fall,           // 5 : 落下状態
	OnTheWall,      // 6 : 壁ずり状態
	Attack,			// 7 : 攻撃状態
	IdleToAttack,   // 8 : 待機状態から攻撃へ
	Hurt,			// 9 : ダメージ状態
	Doge,    		// 10 : 回避状態
	Pareise,		// 11 : 防御状態
	Dead			// 12 : 死亡状態
};
/*
enum class PlayerState { Idle, Jump, Attack };
enum class EnemyState { Idle, Patrol, Hurt };


*/
