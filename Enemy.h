#pragma once
#include <Siv3D.hpp>

class Enemy
{
private:
	Vec2 m_Position;		  //位置
	Vec2 m_Scale;			  //大きさ
	int m_HP;				  //体力
	int m_Attack;			  //攻撃力
	float m_AttackRange;	  //攻撃範囲
	float m_AttackSpeed;	  //攻撃速度
	float m_Speed;			  //移動速度
	bool m_FaceRight;		  //向き

public:
	Enemy();
	Enemy(
		Vec2 position,
		Vec2 scale,
		int hp,
		int attack,
		float attackRange,
		float attackSpeed,
		float speed,
		bool faceRight
	)
		: m_Position(position)
		, m_Scale(scale)
		, m_HP(hp)
		, m_Attack(attack)
		, m_AttackRange(attackRange)
		, m_AttackSpeed(attackSpeed)
		, m_Speed(speed)
		, m_FaceRight(faceRight)
	{
	}
	~Enemy();

	//getter 
	Vec2 getPosition() const { return m_Position; }
	Vec2 getScale() const { return m_Scale; }
	int getHP() const { return m_HP; }
	int getAttack() const { return m_Attack; }
	float getAttackRange() const { return m_AttackRange; }
	float getAttackSpeed() const { return m_AttackSpeed; }
	float getSpeed() const { return m_Speed; }
	bool isFacingRight() const { return m_FaceRight; }


	//setter
	Vec2 setPosition(const Vec2 pos) { return m_Position = pos; }
	Vec2 setScale(const Vec2 scale) { return m_Scale = scale; }
	void setHP(int hp) { m_HP = hp; }
	void setAttack(int attack) { m_Attack = attack; }
	void setAttackRange(float range) { m_AttackRange = range; }
	void setAttackSpeed(float speed) { m_AttackSpeed = speed; }
	void setSpeed(float speed) { m_Speed = speed; }
	void setFaceRight(bool faceRight) { m_FaceRight = faceRight; }


	Enemy& GetEnemy() { return *this; }
	void update();
	void draw() const;
};

