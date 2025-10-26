// StateMode.hpp
#pragma once

enum class StateMode {
	Idle,			// 0 : アイドル状態
	IdleToRun,		// 1 : 立ち状態から走る状態への遷移
	Run,			// 2 : 走る状態
	Jump,			// 3 : ジャンプ状態
	OnTheWall,      // 4 : 壁ずり状態
	Attack,			// 5 : 攻撃状態
	IdleToAttack,   // 6 : 待機状態から攻撃へ
	Hurt,			// 7 : ダメージ状態
	Doge,    		// 8 : 回避状態
	Pareise,		// 9 : 防御状態
	Dead			// 10 : 死亡状態
};
/*
enum class PlayerState { Idle, Jump, Attack };
enum class EnemyState { Idle, Patrol, Hurt };


*/
