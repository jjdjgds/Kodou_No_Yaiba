#pragma once
#include "Player.hpp"

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
	Vec2 m_boss_pos;
	Vec2 m_boss_scale;

	bool m_FaceRight;

	int m_boss_hp;
	int m_boss_bpm;
	int m_boss_atk;
	int m_boss_rang;
	float m_boss_speed;

	Vec2 m_hitBox = { 22.0 ,35.0 };  

	Boss_Behavior m_behavior = Boss_Behavior::idle;
	Boss_Pattern m_pattern = Boss_Pattern::PATTERN_1;

public:
	Enemy_Boss(Vec2 pos, double stride,
			 bool faceRight, Vec2 scale)
		: m_boss_pos(pos)
		, m_FaceRight(faceRight)
		, m_boss_scale(scale) {
	}

	void update(Player& player, Game_Map& map);


	void draw(const Game_Map& CameraPos) const;

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
