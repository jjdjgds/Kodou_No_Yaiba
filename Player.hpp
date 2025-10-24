#pragma once
#include <Siv3D.hpp>
#include "StateMode.hpp"
#include "Game_Map.hpp"
#define MAX_WEAPON (3)
class Player
{



private:
	Vec2 m_Position;		  //位置
	Vec2 m_Scale;			  //大きさ
	Vec2 m_Velocity;		  //速度
	Vec2 m_Acceleration;	  //加速度
	Vec2 m_AttackRengeBox;  //攻撃範囲矩形
	Vec2 m_HitBox;          //当たり判定矩形
	int m_HP;				  //体力
	int m_BPM;				  //心拍数
	int m_Attack;			  //攻撃力
	bool m_AttackFlag;		  //攻撃フラグ
	int m_Weapon[MAX_WEAPON]; //武器種別
	float m_AttackRange;	  //攻撃範囲
	float m_AttackSpeed;	  //攻撃速度
	float m_Speed;			  //移動速度
	float m_DamageTimeOut;	  //ダメージ受けた後の無敵時間
	bool m_Jump;			  //ジャンプ状態
	float m_JumpSpeed;		  //ジャンプ速度
	bool m_FaceRight;		  //向き true:右 false:左
	bool m_Invincible;		  //無敵状態 true:無敵 false:通常
	double animTime = 0.0;    //アニメーション時間管理用
	RectF m_srcRect;		  //描画元矩形
	RectF m_HitRect;          //当たり判定矩形
	float m_gravity = 0.98f;  //重力
	bool m_onGround = false;

	// 各アニメーションのフレーム番号
	Array<int32> m_idlePatterns{ 0, 1, 2, 3, 4, 5, 6, 7 };
	Array<int32> m_attackPatterns{ 0, 1, 2, 3, 4, 5, 6 };
	
	double m_scale = 4.0;     //描画スケール
	size_t m_frameIndex = 0;  //アニメーションフレームインデックス


public:
	StateMode m_state = StateMode::Idle;
	Player();


	Player(
		Vec2 position,
		Vec2 scale,
		Vec2 velocity,
		Vec2 HitBox,
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
		, m_Velocity(velocity)
		, m_HitBox(HitBox)
		, m_HP(hp)
		, m_BPM(bpm)
		, m_Attack(attack)
		, m_AttackRange(attackRange)
		, m_AttackSpeed(attackSpeed)
		, m_Speed(speed)
		, m_DamageTimeOut(damageTimeOut)
		, m_Jump(jump)
		, m_JumpSpeed(5)
		, m_FaceRight(faceRight)
		, m_Invincible(invincible)
		, m_AttackFlag(false)
		, m_AttackRengeBox(200, 131)//ここかえれば攻撃範囲変わる
		,m_gravity(9.8)
		{
		//m_srcRect.setPos(m_Position.x + 150, m_Position.y).setSize(150, 131);
		}

	~Player();

	//getter 
	//
	 Vec2 GetPlayerPosition() const { return m_Position; }
	 Vec2 GetPlayerScale() const { return m_Scale; }
	 Vec2 GetPlayerVelocity() const { return m_Velocity; }
	 Vec2 GetPlayerAcceleration() const { return m_Acceleration; }
	 Vec2 GetPlayerAttackRengeBox() const { return m_AttackRengeBox; }
	 Vec2 GetPlayerHitBox() const { return m_HitBox; }
	 int   GetPlayerHP() const { return m_HP; }
	 int   GetPlayerBPM() const { return m_BPM; }
	 int   GetPlayerAttack() const { return m_Attack; }
	 float GetPlayerAttackRange() const { return m_AttackRange; }
	 float GetPlayerAttackSpeed() const { return m_AttackSpeed; }
	 float GetPlayerSpeed() const { return m_Speed; }
	 float GetPlayerDamageTimeOut() const { return m_DamageTimeOut; }
	 float GetPlayerGravity() const { return m_gravity; }
	 bool  IsPlayerJumping() const { return m_Jump; }
	 float GetPlayerJumpSpeed() const { return m_JumpSpeed; }
	 bool  IsPlayerFacingRight() const { return m_FaceRight; }
	 bool  IsPlayerInvincible() const { return m_Invincible; }
	 bool  IsPlayerAttacking() const { return m_AttackFlag; }
	 RectF GetPlayerHitRect() const { return m_HitRect; }

	//setter
	Vec2 SetPlayerPosition(const Vec2 pos) { return m_Position = pos; }
	Vec2 SetPlayerScale(const Vec2 scale) { return m_Scale = scale; }
	Vec2 SetPlayerVelocity(const Vec2 vel) { return m_Velocity = vel; }
	Vec2 SetPlayerAcceleration(const Vec2 acc) { return m_Acceleration = acc; }
	Vec2 SetPlayerAttackRengeBox(const Vec2 box) { return m_AttackRengeBox = box; }
	Vec2 SetPlayerHitBox(const Vec2 box) { return m_HitBox = box; }
	void SetPlayerHP(int hp) { m_HP = hp; }
	void SetPlayerBPM(int bpm) { m_BPM = bpm; }
	void SetPlayerAttack(int attack) { m_Attack = attack; }
	void SetPlayerAttackRange(float range) { m_AttackRange = range; }
	void SetPlayerAttackSpeed(float speed) { m_AttackSpeed = speed; }
	void SetPlayerSpeed(float speed) { m_Speed = speed; }
	void SetPlayerDamageTimeOut(float timeout) { m_DamageTimeOut = timeout; }
	void SetPlayerJump(bool jump) { m_Jump = jump; }
	void SetPlayerFaceRight(bool faceRight) { m_FaceRight = faceRight; }
	void SetPlayerInvincible(bool invincible) { m_Invincible = invincible; }
	bool SetPlayerjumpFlag(bool flag) { return m_Jump = flag; }
	float SetPlayerJumpSpeed(float jumpSpeed) { return m_JumpSpeed = jumpSpeed; }
	bool SetPlayerAttackFlag(bool flag) { return m_AttackFlag = flag; }
	float SetPlayerGravity(float gravity) { return m_gravity = gravity; }
	RectF SetPlayerHitRect(const RectF rect) { return m_HitRect = rect; }
	
	Player& GetPlayer() { return *this; }
	void update(Game_Map& map);
	void draw() const;
};
