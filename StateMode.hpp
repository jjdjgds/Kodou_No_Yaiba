// StateMode.hpp
#pragma once

enum class StateMode {
	Idle,	  // 0 : アイドル状態
	IdleToRun,// 1 : 立ち状態から走る状態への遷移
	Run,	  // 2 : 走る状態
	Jump,	  // 3 : ジャンプ状態
	Attack,	  // 4 : 攻撃状態
	Hurt,	  // 5 : ダメージ状態
	Avoidance,// 6 : 回避状態
	Pareise,  // 7 : 防御状態
	Dead      // 8 : 死亡状態
};
/*
enum class PlayerState { Idle, Jump, Attack };
enum class EnemyState { Idle, Patrol, Hurt };


*/
