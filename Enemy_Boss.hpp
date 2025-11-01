#pragma once
#include <Siv3D.hpp>

class Player;
class Game_Map;

enum class AnimState_Boss
{
	Idle,
	Battle_Idle,
	Charge_Atk,
	Fly,
	Throw_star,
	Dash,
	Meditate,
	P2_1_Atk,
	P2_2_Atk,
	P2_3_Atk,
	P2_4_Atk,
	Throw_Gas,
	Parry,
	Dead,
};

struct AnimDesc_Boss {
	int  row;
	int  start;
	int  frames;
	double frameTime;
	bool   loop = true;
};

enum class Boss_Pattern
{
	PATTERN_0 = 0,
	PATTERN_1,
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
private:
	bool m_debugDraw = true;

	Vec2 m_boss_pos;
	Vec2 m_boss_scale = { 140.0 ,120.0 };
	Vec2 m_hitBox = { 70.0  ,100.0 };
	Vec2 m_vel = { 0,0 };
	bool m_FaceRight = true;
	float m_gravity = 1000.0f;
	bool   m_onGround = false;

	int m_boss_hp = 30;
	float m_boss_bpm = 100;
	float m_base_bpm = 100;
	bool m_isAttacking = false;
	int m_boss_atk = 1;
	int m_boss_range = 400.0f;
	bool m_hasTakenHit = false;
	double m_hitOffsetY = 0.0;// 当たり判定Y
	float chaseRange = 500.0f;

	float dist = 0;
	float m_base_speed = 300.0f;
	float m_boss_speed = 300.0f;

	Boss_Behavior m_behavior = Boss_Behavior::idle;
	Boss_Pattern m_pattern = Boss_Pattern::PATTERN_0;

	bool m_hasHitPlayer = false;

	// Attack / pattern management
	double m_attackTimer = 0.0;
	double m_attackCooldown = 2; // seconds between attacks

	bool m_isDying = false;
	int m_deathPatternCounter = 0;  // Counts how many times we've cycled
	bool m_deathanimation = false;

	void handleAttackPattern(Player& player, Game_Map& map);
	void executePattern(Player& player, Game_Map& map, Boss_Pattern pattern);

	void Pattern_1(Player& player, Vec2 cam_pos);
	int m_pattern1Phase = 0;          // Phase tracker: 0=moveToTop, 1=attack, 2=return
	double m_pattern1Timer = 0.0;     // Timer for transitions
	Vec2 m_startPos = Vec2{ 0, 0 };   // Starting position before moving to top
	Vec2 m_projectilePos = Vec2{ 0, 0 }; // Current projectile position
	Vec2 m_projectileDir = Vec2{ 0, 0 }; // Direction projectile travels
	bool m_projectileActive = false;  // Whether projectile is currently active
	bool m_projectileReflected = true;    // now it is heading toward the boss
	int  m_currentWaypoint = 0;

	void Pattern_2(Player& player, Vec2 cam_pos);
	double m_pattern2Timer = 0.0;
	int m_pattern2Phase = 0;
	int m_pattern2Count = 0;
	Vec2 m_pattern2Dir = { 1.0, 0.0 };  // saved direction

	void Pattern_3(Player& player, Vec2 cam_pos);
	bool m_pattern3Done = false;
	//struct SmokeProjectile
	//{
	//	Vec2 position;
	//	Vec2 velocity;
	//	bool active = false;
	//	bool exploded = false; // true when it hits the ground
	//};
	struct SmokeData
	{
		Vec2 position;
		double timeInSmoke = 0.0;
		double lifetime = 0.0;
		bool active = false;
	};
	//SmokeProjectile m_pattern3Projectile;
	double m_pattern3Timer = 0.0;
	SmokeData m_smoke; // one smoke instance
	void UpdateSmoke(Vec2 cam_pos, Player& player); // manages smoke life/draw

	void Pattern_4(Player& player, Vec2 cam_pos);
	void executeCounterAttack(Player& player, Vec2 cam_pos);
	bool m_counterReady = false;      // true when in counter stance
	double m_pattern4Timer = 0.0;    // timer to exit counter stance

	void Pattern_5(Player& player, Vec2 cam_pos);
	int m_pattern5Phase = 0;
	double m_pattern5Timer = 0.0;

	void Pattern_6(Player& player, Vec2 cam_pos);
	double m_pattern6Timer = 0.0;
	void updateSpeedByBPM();
	bool m_OverBPM = false;
	double m_OverBPMTimer = 0.0;

	AnimState_Boss m_state{ AnimState_Boss::Idle };	// 現在のアニメーション状態


	HashTable<AnimState_Boss, AnimDesc_Boss> m_anims{	// アニメーションの説明
		{ AnimState_Boss::Idle,			{ 0, 0, 7, 0.10, true }  },
		{ AnimState_Boss::Battle_Idle,  { 0, 7, 4, 0.10, true }  },
		{ AnimState_Boss::Charge_Atk,   { 1, 3, 8, 0.10, false}  },
		{ AnimState_Boss::Fly,			{ 2, 3, 1, 0.01, true }  },
		{ AnimState_Boss::Throw_star,	{ 2, 4, 2, 0.30, false}  },
		{ AnimState_Boss::Dash,			{ 2, 6, 1, 0.10, true }  },
		{ AnimState_Boss::Meditate,		{ 2, 7, 1, 2.00, true }  },
		{ AnimState_Boss::P2_1_Atk,		{ 3, 0, 6, 0.05, false }  },
		{ AnimState_Boss::P2_2_Atk,		{ 3, 6, 5, 0.05, false }  },
		{ AnimState_Boss::P2_3_Atk,		{ 4, 3, 6, 0.05, false }  },
		{ AnimState_Boss::P2_4_Atk,		{ 5, 2, 7, 0.05, false }  },
		{ AnimState_Boss::Throw_Gas,	{ 6, 0, 5, 0.10, false }  },
		{ AnimState_Boss::Parry,		{ 6, 5, 5, 0.10, false }  },
		{ AnimState_Boss::Dead,			{ 7, 2, 5, 0.10, false }  },
	};

	//For Drawing sprite sheet
	size_t m_frameIndex = 0;  //アニメーションフレームインデックス
	size_t m_frameIndexY = 0;
	double m_time{ 0.0 };		// アニメーション時間管理用

	void setState(AnimState_Boss s) {	// アニメーション状態を設定
		if (m_state != s)
		{
			m_state = s;
			m_frameIndex = 0;    // アニメーションの最初のフレームに戻す
			m_time = 0.0;        // フレーム更新タイマーをリセット
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
	float GetTimeScale() const {
		return m_base_speed / m_boss_speed;
		// If m_boss_speed > base_speed → faster attacks (shorter time)
	}
	AnimState_Boss GetPlayerState() const { return m_state; }

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



// 4 (only player needed now)
// animation
// merge with player -> player take damage when hit the boss

