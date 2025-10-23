#pragma once
#include <Siv3D.hpp>

#define MAX_WEAPON (3)
class Player
{



private:
	Vec2 m_Position;		  //位置
	Vec2 m_Scale;			  //大きさ
	int m_HP;				  //体力
	int m_BPM;				  //心拍数
	int m_Attack;			  //攻撃力
	bool m_AttackFlag;		  //攻撃フラグ
	int m_Weapon[MAX_WEAPON]; //武器種別
	float m_AttackRange;	  //攻撃範囲
	float m_AttackSpeed;	  //攻撃速度
	float m_AttackTimer;	  //攻撃タイマー
	float m_Speed;			  //移動速度
	float m_DamageTimeOut;	  //ダメージ受けた後の無敵時間
	bool m_Jump;			  //ジャンプ状態
	bool m_FaceRight;		  //向き
	bool m_Invincible;		  //無敵状態 true:無敵 false:通常
	double animTime = 0.0;    //アニメーション時間管理用


	// 各アニメーションのフレーム番号
	Array<int32> m_idlePatterns{ 0, 1, 2, 3, 4, 5, 6, 7 };
	Array<int32> m_attackPatterns{ 0, 1, 2, 3, 4, 5, 6, 7 };
	
	double m_scale = 4.0;     //描画スケール
	size_t m_frameIndex = 0;  //アニメーションフレームインデックス


public:
	Player();


	Player(
		Vec2 position,
		Vec2 scale,
		int hp,
		int bpm,
		int attack,
		float attackRange,
		float attackSpeed,
		float speed,
		float damageTimeOut,
		bool jump,
		bool faceRight,
		bool invincible

	)
		: m_Position(position)
		, m_Scale(scale)
		, m_HP(hp)
		, m_BPM(bpm)
		, m_Attack(attack)
		, m_AttackRange(attackRange)
		, m_AttackSpeed(attackSpeed)
		, m_Speed(speed)
		, m_DamageTimeOut(damageTimeOut)
		, m_Jump(jump)
		, m_FaceRight(faceRight)
		, m_Invincible(invincible)
		, m_AttackFlag(false)
		, m_AttackTimer(0.0f)
		
		{
		}

	~Player();

	//getter 
	//
	Vec2 getPosition() const { return m_Position; }
	Vec2 getScale() const { return m_Scale; }
	int getHP() const { return m_HP; }
	int getBPM() const { return m_BPM; }
	int getAttack() const { return m_Attack; }
	bool getAttackFlag() const { return m_AttackFlag; }
	float getAttackRange() const { return m_AttackRange; }
	float getAttackSpeed() const { return m_AttackSpeed; }
	float getSpeed() const { return m_Speed; }
	float getDamageTimeOut() const { return m_DamageTimeOut; }
	bool isJumping() const { return m_Jump; }
	bool isFacingRight() const { return m_FaceRight; }
	bool isInvincible() const { return m_Invincible; }


	//setter
	Vec2 setPosition(const Vec2 pos) { return m_Position = pos; }
	Vec2 setScale(const Vec2 scale) { return m_Scale = scale; }
	void setHP(int hp) { m_HP = hp; }
	void setBPM(int bpm) { m_BPM = bpm; }
	void setAttack(int attack) { m_Attack = attack; }
	void setAttackFlag(bool flag) { m_AttackFlag = flag; }
	void setAttackRange(float range) { m_AttackRange = range; }
	void setAttackSpeed(float speed) { m_AttackSpeed = speed; }
	void setSpeed(float speed) { m_Speed = speed; }
	void setDamageTimeOut(float timeout) { m_DamageTimeOut = timeout; }
	void setJump(bool jump) { m_Jump = jump; }
	void setFaceRight(bool faceRight) { m_FaceRight = faceRight; }
	void setInvincible(bool invincible) { m_Invincible = invincible; }

	
	
	Player& GetPlayer() { return *this; }
	void update();
	void draw() const;
};
