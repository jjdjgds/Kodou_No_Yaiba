#pragma once
#include <Siv3D.hpp>

class Player;
class Game_Map;

enum class AnimState_Boss {Idle,Run, Dead,Attack,};

struct AnimDesc_Boss {
	int  row;
	int  start;
	int  frames;
	double frameTime;
	bool   loop = true;
};

enum class Boss_Pattern
{
	PATTERN_1 = 0,
	PATTERN_2,
	PATTERN_3,
	PATTERN_4,
	PATTERN_5,
	PATTERN_6,

	PATTERN_MAX
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
	bool m_debugDraw = true;

	Vec2 m_boss_pos;
	Vec2 m_boss_scale = { 140.0 ,120.0 };
	Vec2 m_hitBox = { 70.0  ,100.0 };
	Vec2 m_vel = { 0,0 };
	bool m_FaceRight = true;
	float m_gravity = 1000.0f;
	bool   m_onGround = false;

	int m_boss_hp = 50;
	float m_boss_bpm = 100;
	float m_base_bpm = 100;
	bool m_isAttacking = false;
	int m_boss_atk = 1;
	int m_boss_range = 400.0f;
	double m_hitOffsetY = 0.0;// 当たり判定Y
	float chaseRange = 700.0f;
;
	float dist = 0;
	float m_base_speed = 300.0f;
	float m_boss_speed = 300.0f;

	Boss_Behavior m_behavior = Boss_Behavior::idle;
	Boss_Pattern m_lastPattern = Boss_Pattern::PATTERN_1;
	Boss_Pattern m_pattern = Boss_Pattern::PATTERN_1;

	// Attack / pattern management
	double m_attackTimer = 0.0;
	double m_attackCooldown = 2.0; // seconds between attacks

	bool m_isDying = false;
	int m_deathPatternCounter = 0;  // Counts how many times we've cycled

	void handleAttackPattern(Player& player, Game_Map& map);
	void executePattern(Player& player, Game_Map& map, Boss_Pattern pattern);

	void Pattern_1(Player& player, Vec2 cam_pos);
	void Pattern_2(Player& player, Vec2 cam_pos);
	void Pattern_3(Player& player, Vec2 cam_pos);
	void Pattern_4(Player& player, Vec2 cam_pos);
	void Pattern_5(Player& player ,Vec2 cam_pos);
	int m_pattern5Phase = 0;
	double m_pattern5Timer = 0.0;
	void Pattern_6(Player& player, Vec2 cam_pos);
	void updateSpeedByBPM();

	AnimState_Boss m_state{ AnimState_Boss::Idle };	// 現在のアニメーション状態
	HashTable<AnimState_Boss, AnimDesc_Boss> m_anims{	// アニメーションの説明
		{ AnimState_Boss::Idle,   {0, 0, 8, 0.09, true  }  },
		{ AnimState_Boss::Run,    { 1, 3, 9, 0.10, true }  },
		{ AnimState_Boss::Dead,   { 4, 1, 3, 0.25, false } },
		{ AnimState_Boss::Attack, { 3,2, 4, 0.20, false  } },
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
	Enemy_Boss(Vec2 pos)
		: m_boss_pos(pos)
	{
	}

	void update(Player& player, Game_Map& map);


	void draw(const Game_Map& map) const;

	const Vec2& GetPosition() const { return m_boss_pos; }
	const Vec2& GetScale() const { return m_boss_scale; }
	int GetHP() const { return m_boss_hp; }
	int GetBPM() const { return m_boss_bpm; }
	int GetAttack() const { return m_boss_atk; }
	int GetRange() const { return m_boss_range; }
	float GetSpeed() const { return m_boss_speed; }
	Vec2 getHitbox() const { return m_hitBox; }

	void SetPosition(const Vec2& pos) { m_boss_pos = pos; }
	void SetScale(const Vec2& scale) { m_boss_scale = scale; }
	void SetHP(int hp) { m_boss_hp = hp; }
	void SetBPM(int bpm) { m_boss_bpm = bpm; }
	void SetAttack(int atk) { m_boss_atk = atk; }
	void SetRange(int range) { m_boss_range = range; }
	void SetSpeed(float speed) { m_boss_speed = speed; }
	void setHitbox(Vec2 hitbox) { m_hitBox = hitbox; }

	RectF BossRect(const Vec2& cam) const; 
	RectF BossRectAt(const Vec2& pos) const;
	RectF attackRect(const Vec2& cam) const;
	RectF chaseRect(const Vec2& cam) const;
};

//->boss no hp (bpm)
// player take damage when hit the boss
// bmp higher = higher speed
