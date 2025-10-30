#pragma once
#include <Siv3D.hpp>
#include "Player.hpp"

enum class AnimState {// アニメーション状態列挙型
	Idle,
	Run,
	Hurt,
	Attack,
};

enum class Behavior { Patrol, Chase, Attack };// 行動パターン列挙型
enum class PatrolPhase { Move, Wait, };// 巡回フェーズ列挙型

struct AnimDesc {// アニメーションの説明構造体
	String asset;
	int32  frames;
	double frameTime;
	bool   loop = true;
};

class Enemy
{
private:
	bool m_debugDraw = true; //デバッグ描画フラグ


	Vec2 m_Position;		  //位置
	Vec2 m_Scale;			  //大きさ
	bool m_FaceRight;		  //向き
	float m_Speed = 150.0f;	  //移動速度
	int m_HP;				  //体力

	double m_hitOffsetY = 16.0;// 当たり判定Y
	Vec2 m_hitBox = { 22.0 ,35.0 };// 当たり判定サイズ


	float m_gravity = 1800.0;// 重力
	float m_velY = 0.0;// Y方向速度
	bool   m_onGround = false;

	double m_stride = 0.0f; // 巡回範囲
	bool m_strideRandom = true;// 巡回範囲ランダムフラグ
	double m_strideMin = m_stride - 100.0;// 巡回範囲最小値
	double m_strideMax = m_stride + 100.0;// 巡回範囲最大値
	double m_budget = m_stride;// 巡回予算

	bool isRuning = false;

	bool m_isFrozen = false;
	Behavior m_mode = Behavior::Patrol;
	PatrolPhase m_phase = PatrolPhase::Wait;

	double m_phaseTimer = 0.0;// 巡回フェーズタイマー

	double m_waitMin = 0.8;// 待機フェーズ時間範囲
	double m_waitMax = 2.0;// 待機フェーズ時間範囲
	double m_moveMin = 1.5;// 移動フェーズ時間範囲
	double m_moveMax = 3.5;// 移動フェーズ時間範囲

	void enterPhase(PatrolPhase p) {
		m_phase = p;
		if (p == PatrolPhase::Wait) m_phaseTimer = Random(m_waitMin, m_waitMax);
		else                        m_phaseTimer = Random(m_moveMin, m_moveMax);
	}


	float m_speedBase = m_Speed;// 元の移動速度


	bool m_takeDamage = false; // ダメージを受けたかどうか


	bool AttackFlag = false; // 攻撃フラグ
	int m_Attack;			  //攻撃力
	double m_attackCooldown = 0.0;
	double m_attackCooldownMax = 0.6; // 可调
	float m_AttackRange;	  //攻撃範囲
	bool m_hasHitPlayer = false; // 1回の攻撃でプレイヤーに当てたかどうか



	AnimState m_state{ AnimState::Idle };	// 現在のアニメーション状態
	HashTable<AnimState, AnimDesc> m_anims{	// アニメーションの説明
		{ AnimState::Idle, { U"EnemyIdle", 10, 0.12, true } },
		{ AnimState::Run,  { U"EnemyRun",  16, 0.07, true } },
		{ AnimState::Hurt,  { U"EnemyHurt", 4, 0.15, false } },
		{ AnimState::Attack,  { U"EnemyAttack", 7, 0.08, false } },

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


public:
	Enemy(Vec2 pos, double stride,
			 bool faceRight, Vec2 scale)
		: m_Position(pos)
		, m_stride(stride)
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
	Vec2 getHitbox() const { return m_hitBox; }


	//setter
	Vec2 setPosition(const Vec2 pos) { return m_Position = pos; }
	Vec2 setScale(const Vec2 scale) { return m_Scale = scale; }
	void setSpeed(float speed) { m_Speed = speed; }
	void setFaceRight(bool faceRight) { m_FaceRight = faceRight; }



	void setHP(int hp) { m_HP = hp; }
	void setAttack(int attack) { m_Attack = attack; }
	void setAttackRange(float range) { m_AttackRange = range; }

	void setHitbox(Vec2 hitbox) { m_hitBox = hitbox; }


	Enemy& GetEnemy() { return *this; }

	void update(Player& player, Game_Map& map);

	void draw(const Game_Map& CameraPos) const;

<<<<<<< HEAD:Enemy_1.hpp
	void die();// 死亡処理
	bool pendingRemoval() const { return m_pendingRemoval; }
	bool IsDead() const { return m_dead; }
=======
	void takeDamage(int damage);
>>>>>>> f43e84f90f86a93fec393bdf9d1b45c4ec9c3251:Enemy.hpp

	RectF hurtRect(const Vec2& cam) const; // ダメージ判定矩形を取得
	RectF hurtRectAt(const Vec2& pos) const;// 指定位置での当たり判定矩形取得

<<<<<<< HEAD:Enemy_1.hpp
	double forwardClearance(const Game_Map& map, double baseW, double baseH, double lead, double maxForward, int dir) const;
	RectF attackRect(const Game_Map& map) const; // 攻撃判定矩形を取得
	RectF chaseRect(const Game_Map& map) const;// 追跡判定矩形を取得
=======
	RectF attackRect(const Vec2& cam) const; // 攻撃判定矩形を取得
	RectF chaseRect(const Vec2& cam) const;

	Line makeGroundProbeLine(const Vec2& cam) const;

	
>>>>>>> f43e84f90f86a93fec393bdf9d1b45c4ec9c3251:Enemy.hpp

};

