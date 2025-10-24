#pragma once
#include <Siv3D.hpp>
#include "Player.hpp"
#include "Game_Map.hpp"

enum class AnimState {// アニメーション状態列挙型
	Idle,
	Run,
	Hurt,
	Attack,

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

	double m_hitOffsetY = 16.0;// 当たり判定Y

	float m_gravity = 1800.0;// 重力
	float m_velY = 0.0;// Y方向速度
	bool   m_onGround = false;

	bool AttackFlag = false; // 攻撃フラグ


	float m_speedBase = m_Speed;// 元の移動速度
	bool m_takeDamage = false; // ダメージを受けたかどうか
	Vec2 m_hitBox = { 22.0 ,35.0 };// 当たり判定サイズ

	int m_HP;				  //体力
	int m_Attack;			  //攻撃力
	float m_AttackRange;	  //攻撃範囲
	float m_AttackSpeed;	  //攻撃速度

	float m_patrolL{ 0.0 }, m_patrolR{ 0.0 }; // 巡回範囲

	AnimState m_state{ AnimState::Idle };	// 現在のアニメーション状態
	HashTable<AnimState, AnimDesc> m_anims{	// アニメーションの説明
		{ AnimState::Idle, { U"EnemyIdle", 10, 0.12, true } },
		{ AnimState::Run,  { U"EnemyRun",  16, 0.07, true } },
		{ AnimState::Hurt,  { U"EnemyHurt", 4, 0.15, false } },
		{ AnimState::Attack,  { U"EnemyAttack", 7, 0.10, false } },

	};
	int32  m_frameIndex{ 0 };	// 現在のフレームインデックス
	double m_time{ 0.0 };		// アニメーション時間管理用

	void setState(AnimState s) {	// アニメーション状態を設定
		if (m_state != s)
		{
			m_state = s;
			m_time = 0.0;        // フレーム更新タイマーをリセット
			m_frameIndex = 0;    // アニメーションの最初のフレームに戻す
		}
	}


	bool m_debugDraw = true; //デバッグ描画フラグ


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
	Vec2 getHitbox() const { return m_hitBox; }
	

	//setter
	Vec2 setPosition(const Vec2 pos) { return m_Position = pos; }
	Vec2 setScale(const Vec2 scale) { return m_Scale = scale; }
	void setSpeed(float speed) { m_Speed = speed; }
	void setFaceRight(bool faceRight) { m_FaceRight = faceRight; }



	void setHP(int hp) { m_HP = hp; }
	void setAttack(int attack) { m_Attack = attack; }
	void setAttackRange(float range) { m_AttackRange = range; }
	void setAttackSpeed(float speed) { m_AttackSpeed = speed; }

	void setHitbox(Vec2 hitbox) { m_hitBox = hitbox; }


	Enemy& GetEnemy() { return *this; }
	void update(const Player& player, Game_Map& map);
	void draw() const;
	void takeDamage(int damage);

	RectF hurtRect() const; // ダメージ判定矩形を取得
	RectF  hurtRectAt(const Vec2& pos) const;// 指定位置での当たり判定矩形取得

	RectF attackRect() const; // 攻撃判定矩形を取得
	Line makeGroundProbeLine() const;

};

