#pragma once
#include <Siv3D.hpp>

class Player;
class Game_Map;

enum class AnimState_Boss {// アニメーション状態列挙型
	Idle,
	Run,
	Hurt,
	Attack,
};

struct AnimDesc_Boss {// アニメーションの説明構造体
	String asset;
	int32  frames;
	double frameTime;
	bool   loop = true;
};

enum class Boss_Pattern
{
	PATTERN_1,
	PATTERN_2,
	PATTERN_3,
	PATTERN_4,
	PATTERN_5,
	PATTERN_6,
};

enum class Boss_Behavior
{
	idle,
	Chase,
	Attack
};

class Enemy_Boss
{
private :
	Vec2 m_boss_pos = { 0,0 };
	Vec2 m_boss_scale = { 75,75 };
	Vec2 m_vel = { 0,0 };
	bool m_FaceRight = true;
	float m_gravity = 1000.0f;

	int m_boss_hp = 50;
	int m_boss_bpm = 100;
	int m_boss_atk = 10;
	int m_boss_rang = 100;
	float tex_offsetY = 45.0f;
	float dist = 0;
	float m_boss_speed = 1.0f;

	Vec2 m_hitBox = { 75.0  ,100.0 };

	Boss_Behavior m_behavior = Boss_Behavior::idle;
	Boss_Pattern m_pattern = Boss_Pattern::PATTERN_1;

	AnimState_Boss m_state{ AnimState_Boss::Idle };	// 現在のアニメーション状態
	HashTable<AnimState_Boss, AnimDesc_Boss> m_anims{	// アニメーションの説明
		{ AnimState_Boss::Idle, { U"EnemyIdle", 10, 0.12, true } },
		{ AnimState_Boss::Run,  { U"EnemyRun",  16, 0.07, true } },
		{ AnimState_Boss::Hurt,  { U"EnemyHurt", 4, 0.15, false } },
		{ AnimState_Boss::Attack,  { U"EnemyAttack", 7, 0.08, false } },

	};

	//For Drawing sprite sheet
	int32  m_frameIndex{ 0 };	// 現在のフレームインデックス
	double m_time{ 0.0 };		// アニメーション時間管理用

	void setState(AnimState_Boss s) {	// アニメーション状態を設定
		if (m_state != s)
		{
			m_state = s;
			m_time = 0.0;        // フレーム更新タイマーをリセット
			m_frameIndex = 0;    // アニメーションの最初のフレームに戻す
		}
	}

public:
	Enemy_Boss() = default;
	Enemy_Boss(Vec2 pos, double stride,
			 bool faceRight, Vec2 scale)
		: m_boss_pos(pos)
		, m_FaceRight(faceRight)
		, m_boss_scale(scale)
	{
	}

	void update(Player& player, Game_Map& map);


	void draw(Vec2 pos , Vec2 size) const;

	const Vec2& GetPosition() const { return m_boss_pos; }
	const Vec2& GetScale() const { return m_boss_scale; }
	int GetHP() const { return m_boss_hp; }
	int GetBPM() const { return m_boss_bpm; }
	int GetAttack() const { return m_boss_atk; }
	int GetRange() const { return m_boss_rang; }
	float GetSpeed() const { return m_boss_speed; }
	Vec2 getHitbox() const { return m_hitBox; }

	void SetPosition(const Vec2& pos) { m_boss_pos = pos; }
	void SetScale(const Vec2& scale) { m_boss_scale = scale; }
	void SetHP(int hp) { m_boss_hp = hp; }
	void SetBPM(int bpm) { m_boss_bpm = bpm; }
	void SetAttack(int atk) { m_boss_atk = atk; }
	void SetRange(int range) { m_boss_rang = range; }
	void SetSpeed(float speed) { m_boss_speed = speed; }
	void setHitbox(Vec2 hitbox) { m_hitBox = hitbox; }

	Enemy_Boss& GetEnemy_Boss() { return *this; }
};

//->hp
//->boss no hp (bpm)
// boss hp = 50
// player take damage when hit the boss
// when hp == 0 -> closing to dead 1 -> 2 -> 5 (3 times)
// when hp != 0 1 -> 2 -> 3 -> 4 -> 5 -> 6
// bmp higher = higher speed
