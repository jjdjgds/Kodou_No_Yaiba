#pragma once
#include <Siv3D.hpp>
#include "StateMode.hpp"
#include "Game_Map.hpp"
#include "Enemy_1.hpp"
#include "Enemy_2.hpp"

#define MAX_WEAPON (3)
#define ATTACKSPEED (0.08)
#define BERSARKATTACKSPEED (0.05)
#define BERSARKEMOVESPEED (100) //バーサーカーモード中の移動速度

class Enemy_1;
class Enemy_2;
enum class HeartRateState
{
	Stun,          // スタン（60以下 or 140以上）
	HightWarning,  // 警告（61-70 or 130-139）
	Berserk,       // バーサーカー（120-129）
	TimeControl,   // ザ・ワールド（71-80）
	Normal,        // 通常（81-119）
	LowWarning,    //警告（61-70 or 130-139）
	Dead           // 死亡(000)

};


class Player
{



private:
	static constexpr float NormalPlayerSpeed = 800.0f;
	static constexpr float DogePlayerSpeed = 1600.0f;
	Vec2 m_Position;	  //位置
	Vec2 m_Scale;			  //大きさ
	Vec2 m_Velocity;		  //速度
	Vec2 m_Acceleration;	  //加速度
	Vec2 m_AttackRengeBox;  //攻撃範囲矩形
	Vec2 m_HitBox;          //当たり判定矩形
	int m_HP;				  //体力
	int m_MaxHP = 3;
	int m_BPM;				  //心拍数
	int m_Attack;			  //攻撃力
	int m_Medicle = 5;
	int m_MaxMedicle = 5;
	bool m_AttackFlag;		  //攻撃フラグ
	bool m_AttackStart;      //攻撃開始フラグ
	int m_Weapon[MAX_WEAPON]; //武器種別
	float m_AttackRange;	  //攻撃範囲
	float m_AttackSpeed;	  //攻撃速度

	float m_Speed;			  //移動速度
	float m_DamageTimeOut;	  //ダメージ受けた後の無敵時間
	bool  m_Jump;			  //ジャンプ状態
	float m_JumpSpeed;		  //ジャンプ速度
	bool  m_FaceRight;		  //向き true:右 false:左
	bool  m_Invincible;		  //無敵状態 true:無敵 false:通常
	double animTime = 0.0;    //アニメーション時間管理用
	RectF m_srcRect;		  //描画元矩形
	Vec2  m_HitRect = { 6.0 ,5.0 };          //当たり判定矩形
	float m_gravity = 9.8;  //重力
	bool  m_onGround = false;
	double m_hitOffsetY = 20.0;// 当たり判定Y
	double m_DogelstTimer = 0.0;
	double m_DogeCoolTimer = 0.0;   // クールタイムの経過時間
	double m_DogeCooldown = 1.0;    // クールタイム時間（秒）

	bool m_IsStunned = false;     // スタン中かどうか
	double m_StunTimer = 0.0;     // スタン経過時間
	const double m_StunDuration = 2.0; // 2秒スタン

	bool   m_isDodging = false;     // 現在ドッジ中か

	double m_DogeTimer = 0.0;       // ドッジ中の経過時間
	bool   m_HeartCoolFlg = false;  // 行動後の心拍数低下時間  Trueでカウントダウン開始
	double m_HeartCoolTimer = 0.0;  //クールタイムの経過時間 
	double m_HeartCooldown = 1.0;   //クールタイム時間（秒）
	double m_HeartTimer = 0.0;
	bool   m_TheWorldFlg = 0.0;     //true=使用
	bool   m_BersarkFlg = false;    //true=バーサーカーモード
	double m_BersarkTimer = 0.0;    //クールタイムの経過時間 
	double m_Bersarkdown = 2.0;     //クールタイム時間（秒）
	double m_WallKickTimer = 0.0;
	bool m_IsInvincible = false;       // 無敵フラグ
	double m_AttackSpeedBoost = 1.0;   // 攻撃速度倍率
	//double m_BersarkTimer = 0.0;       // 残り時間
	//bool m_BersarkFlg = false;         // バーサーク状態中か
	bool m_ParrySuccess = false;   // 弾を跳ね返した瞬間だけtrue
	double m_ParryTimer = 0.0;     // パリィ後の短時間クールダウン
	// ノックバック関連
	Vec2 m_KnockbackVelocity{ 650, -900 };
	double m_KnockbackTimer = 0.35;
	double m_InvincibleTimer = 0.0;
	bool m_IsKnockback = false;
	bool m_TimeAutoFlag = false;

	bool m_isDead = false;

	StateMode m_PlayerState; //プレイヤーの状態管理用
	StateMode m_PlayerLastState;

	// 各アニメーションのフレーム番号
	Array<int32> m_idlePatterns{ 0, 1, 2, 3, 4, 5, 6, 7 };
	// 立ち状態から走る状態への遷移アニメーション（横8枚のうち、0〜2枚目を使う）
	Array<int32> m_idleToRunPatterns{ 0, 1, 2 };



	//走る状態のアニメーション
	Array<int32> m_runPatterns{ 0,1,2,3,4,5 };

	// 攻撃アニメーション（横8枚のうち、0〜6枚目を使う）
	Array<int32> m_attackPatterns{ 0, 1, 2, 3,4 };

	// ダメージアニメーション（横8枚のうち、4〜7枚目を使う）
	Array<int32> m_hurtPatterns{ 0,1,2 };

	//Jumpアニメーション
	Array<int32> m_jumpPatterns{ 1,1,1,1,1 };

	Array<int32>m_jumpAttackPatterns{ 0,1,2,3,4 };

	//IDLEATTACK
	Array<int32> m_IdleAttackPatterns{ 0, 1, 2, 3 };

	//回避アニメーション
	Array<int32> m_dogePatterns{ 0,0,0,0,0 };

	//壁ズリアニメーション
	Array<int32> m_onTheWallPatterns{ 3 };

	//死亡アニメーション
	Array<int32>m_deadPatterns{ 0,1,2,3,4,5 };

	//落下アニメーション
	Array<int32> m_FallPatterns{ 2,2,2,2 };

	//薬ブッキメアニメーション
	Array<int32> m_medecinePatterns{ 0,1,2,3 };
	Array<int32> m_stunPatterns{ 0,1,2,3 };

	double m_scale = 4.0;     //描画スケール
	size_t m_frameIndex = 0;  //アニメーションフレームインデックス
	size_t m_frameIndexY = 0;
	HeartRateState m_HeartRateState = HeartRateState::Berserk;
public:

	//Player();


	Player(
		Vec2 position,
		Vec2 scale,
		Vec2 velocity,
		Vec2 HitBox,
		int hp,
		int maxHP,
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
		, m_MaxHP(maxHP)
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
		, m_gravity(9.8)
		, m_PlayerState(StateMode::Idle)
		, m_HeartRateState(HeartRateState::Dead)
		, m_IsKnockback(false)
		, m_KnockbackTimer(0.0)
		, m_KnockbackVelocity(Vec2{ 0, 0 })

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
	int   GetPlayerMaxHP() const { return m_MaxHP; }
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
	float GetPlayerDefoSpeed() const { return NormalPlayerSpeed; }
	StateMode GetPlayerState() const { return m_PlayerState; }
	StateMode GetPlayerLastState()const { return m_PlayerLastState; }
	HeartRateState GetPlayerHeartState()const { return m_HeartRateState; }
	HeartRateState GetHeartRateState(int bpm);
	bool GetIsInvincible() const { return m_IsInvincible; }
	bool IsDogeging() const { return m_isDodging; }
	int GetMedecine()const { return m_Medicle; }
	bool IsTimeStoped() const { return m_TimeAutoFlag; }
	bool IsDead() const { return m_isDead; }
	//setter
	//float SetPlayerDefoSpeed( float defospe)  { return NormalPlayerSpeed = defospe; }
	Vec2 SetPlayerPosition(const Vec2 pos) { return m_Position = pos; }
	Vec2 SetPlayerScale(const Vec2 scale) { return m_Scale = scale; }
	Vec2 SetPlayerVelocity(const Vec2 vel) { return m_Velocity = vel; }
	Vec2 SetPlayerAcceleration(const Vec2 acc) { return m_Acceleration = acc; }
	Vec2 SetPlayerAttackRengeBox(const Vec2 box) { return m_AttackRengeBox = box; }
	Vec2 SetPlayerHitBox(const Vec2 box) { return m_HitBox = box; }
	void SetPlayerHP(int hp) { m_HP = hp; }
	void SetPlayerMaxHP(int maxHp) { m_MaxHP = maxHp; }
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
	HeartRateState SetPlayerHeartState(HeartRateState a) { return m_HeartRateState = a; }
	void UpdateHeartState();
	bool SetIsInvincible(bool invincible) { return m_IsInvincible = invincible; }
	bool SetIsDodging(bool dodging) { return m_isDodging = dodging; }
	void SetMedecine(int med) { m_Medicle = med; }
	void SetMaxMedecine() { m_Medicle = m_MaxMedicle; }
	bool SetTimeStoped(bool st) { return  m_TimeAutoFlag = st; }

	void Revive();

	// 状態設定
	void SetPlayerState(const StateMode state) {
		m_PlayerState = state;
		m_frameIndex = 0;
		animTime = 0.0;
	}

	void SetPlayerLastState(const StateMode state) {
		m_PlayerLastState = state;
		m_frameIndex = 0;
		animTime = 0.0;
	}
	Player& GetPlayer() { return *this; }

	// Player.hpp に追加
	void takeDamage(int damage, bool fromRight);

	void takeDamage(int dmg);
	RectF getAttackRectWorld()const;
	RectF getAttackRect(const Vec2& camera) const;
	RectF getHitRect(const Vec2& camera)const;

	RectF getHitRectWorld() const;
	//RectF getTheWorld(const Vec2& camera)const;
	///*
	//Array<Enemy_1> m_enemies1
	//Array<Enemy_2> m_enemies2

	//*/
	void OnParrySuccess();
	void PlayerAttack(const Vec2& camera);
	void PlayerIdle();
	void PlayerIdleToRun();
	void PlayerIdleToAttack(const Vec2& camera);
	void PlayerRun();
	void PlayerJump();
	void PlayerOnTheWall();
	void PlayerFall();
	void PlayerDoge();
	void PlayerHurt();
	void PlayerJumpAttack();
	void ApplyHeartEffects();
	void PlayerMedecine();
	void PlayerBerserk();
	void PlayerStun();
	void PlayerDead();
	void update(Game_Map& map, Array<Enemy_1>& m_enemies1, Array<Enemy_2>& m_enemies2);
	void draw(const Game_Map& CameraPos) const;
};
