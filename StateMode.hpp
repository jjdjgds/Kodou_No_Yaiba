// StateMode.hpp
#pragma once

enum class StateMode {
	Idle,			// 0 : アイドル状態
	IdleToRun,		// 1 : 立ち状態から走る状態への遷移
	RunStart,       // 2 : 走りはじめ状態
	Run,			// 3 : 走る状態
	Jump,			// 4 : ジャンプ状態
	JumpAttack,     // 5 : ジャンプ攻撃
	Fall,           // 6 : 落下状態
	OnTheWall,      // 7 : 壁ずり状態
	Attack,			// 8 : 攻撃状態
	IdleToAttack,   // 9 : 待機状態から攻撃へ
	Hurt,			// 10: ダメージ状態
	Doge,    		// 11: 回避状態
	Medecine,       // 12: 薬決め状態
	Pareise,		// 13: 防御状態
	TheWorld,       // 14: 時止め状態
	Dead			// 15: 死亡状態
};
/*
enum class PlayerState { Idle, Jump, Attack };
enum class EnemyState { Idle, Patrol, Hurt };


*/
