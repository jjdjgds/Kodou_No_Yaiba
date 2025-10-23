#pragma once
#include <Siv3D.hpp>

enum class AnimState {// アニメーション状態列挙型
	Idle,
	Run
};

struct AnimDesc {// アニメーションの説明構造体
	String asset;
	int32  frames;
	double frameTime;
	bool   loop = true;
};

class Enemy
{
private:
	Vec2 m_Position;		  //位置
	Vec2 m_Scale;			  //大きさ
	bool m_FaceRight;		  //向き
	float m_Speed;			  //移動速度

	float m_speedBase = m_Speed;// 元の移動速度

	int m_HP;				  //体力
	int m_Attack;			  //攻撃力
	float m_AttackRange;	  //攻撃範囲
	float m_AttackSpeed;	  //攻撃速度

	float m_patrolL{ 0.0 }, m_patrolR{ 0.0 }; // 巡回範囲

	AnimState m_state{ AnimState::Idle };	// 現在のアニメーション状態
	HashTable<AnimState, AnimDesc> m_anims{	// アニメーションの説明
		{ AnimState::Idle, { U"EnemyIdle", 10, 0.12, true } },
		{ AnimState::Run,  { U"EnemyRun",  16, 0.07, true } }
	};
	int32  m_frameIndex{ 0 };	// 現在のフレームインデックス
	double m_time{ 0.0 };		// アニメーション時間管理用

	void setState(AnimState s) {	// アニメーション状態を設定
		if (m_state == s) return;
		m_state = s; m_frameIndex = 0; m_time = 0.0;
	}


public:
	Enemy(Vec2 pos, double speed, double patrolL, double patrolR,
			 bool faceRight, Vec2 scale)
		: m_Position(pos)
		, m_Speed(speed)
		, m_patrolL(patrolL)
		, m_patrolR(patrolR)
		, m_FaceRight(faceRight)
		, m_Scale(scale) {
	}// コンストラクタ

	//getter 
	Vec2 getPosition() const { return m_Position; }
	Vec2 getScale() const { return m_Scale; }
	float getSpeed() const { return m_Speed; }
	bool isFacingRight() const { return m_FaceRight; }

	int getHP() const { return m_HP; }
	int getAttack() const { return m_Attack; }
	float getAttackRange() const { return m_AttackRange; }
	float getAttackSpeed() const { return m_AttackSpeed; }
	

	//setter
	Vec2 setPosition(const Vec2 pos) { return m_Position = pos; }
	Vec2 setScale(const Vec2 scale) { return m_Scale = scale; }
	void setSpeed(float speed) { m_Speed = speed; }
	void setFaceRight(bool faceRight) { m_FaceRight = faceRight; }

	void setHP(int hp) { m_HP = hp; }
	void setAttack(int attack) { m_Attack = attack; }
	void setAttackRange(float range) { m_AttackRange = range; }
	void setAttackSpeed(float speed) { m_AttackSpeed = speed; }
	


	Enemy& GetEnemy() { return *this; }
	void update();
	void draw() const;
};

