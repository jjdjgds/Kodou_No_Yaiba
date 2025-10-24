// StateMode.hpp
#pragma once

enum class StateMode {
	Idle,	  // 0 : アイドル状態
	Run,	  // 1 : 走る状態
	Jump,	  // 2 : ジャンプ状態
	Attack,	  // 3 : 攻撃状態
	Hurt,	  // 4 : ダメージ状態
	Avoidance,// 5 : 回避状態
	Pareise,  // 6 : 防御状態
	Dead      // 7 : 死亡状態
};
/*
enum class PlayerState { Idle, Jump, Attack };
enum class EnemyState { Idle, Patrol, Hurt };


*/
