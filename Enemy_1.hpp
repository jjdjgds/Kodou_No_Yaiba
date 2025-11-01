#pragma once
#include <Siv3D.hpp>
#include "Player.hpp"
#include "TextShot.hpp"
#include "Game_Map.hpp"
#include "AllEffect.h"

enum class AnimState_Enemy1 { Idle, Run, Dead, Attack, };// アニメーション状態列挙型
enum class Behavior_Enemy1 { Patrol, Chase, Attack, };// 行動パターン列挙型
enum class PatrolPhase_Enemy1 { Move, Wait, };// 巡回フェーズ列挙型
// ↓ Player クラスの前方宣言を追加
class Player;


struct AnimDesc_Enemy1 {// アニメーションの説明構造体
	int   row;
	int   start;
	int  frames;
	double frameTime;
	bool   loop = true;
};

class Enemy_1 
{
private:

	TextShot text;
	bool textChase = false;
	bool textLoseCounting = false;
	bool textallowLose = false;

	bool m_debugDraw = true; //デバッグ描画フラグ

	Vec2 m_Position;		  //位置

	bool m_FaceRight;		  //向き
	float m_Speed = 150.0f;	  //移動速度

	bool   m_dead = false;              // 已进入死亡流程
	bool   m_pendingRemoval = false;

	double m_hitOffsetY = 0.0;// 当たり判定Y

	Vec2 m_Scale = { 140.0 ,125.0 };// 大きさ
	Vec2 m_hitBox = { 90.0 ,120.0 };// 当たり判定サイズ


	float m_gravity = 1800.0;// 重力
	float m_velY = 0.0;// Y方向速度
	bool   m_onGround = false;

	double m_stride = 0.0f; // 巡回範囲
	bool m_strideRandom = true;// 巡回範囲ランダムフラグ
	double m_strideMin = m_stride - 100.0;// 巡回範囲最小値
	double m_strideMax = m_stride + 100.0;// 巡回範囲最大値
	double m_budget = m_stride;// 巡回予算

	bool m_isRunning = false;

	Behavior_Enemy1 m_mode = Behavior_Enemy1::Patrol;
	PatrolPhase_Enemy1 m_phase = PatrolPhase_Enemy1::Wait;

	double m_phaseTimer = 0.0;// 巡回フェーズタイマー

	double m_waitMin = 0.8;// 待機フェーズ時間範囲
	double m_waitMax = 2.0;// 待機フェーズ時間範囲
	double m_moveMin = 1.5;// 移動フェーズ時間範囲
	double m_moveMax = 3.5;// 移動フェーズ時間範囲

	void enterPhase(PatrolPhase_Enemy1 p) {
		m_phase = p;
		if (p == PatrolPhase_Enemy1::Wait) m_phaseTimer = Random(m_waitMin, m_waitMax);
		else                        m_phaseTimer = Random(m_moveMin, m_moveMax);
	}


	float m_speedBase = m_Speed;// 元の移動速度


	bool m_takeDamage = false; // ダメージを受けたかどうか


	bool m_attackFlag = false; // 攻撃フラグ
	int m_Attack;			  //攻撃力
	double m_attackCooldown = 0.0;
	double m_attackCooldownMax = 0.6; // 可调
	float m_AttackRange;	  //攻撃範囲
	bool m_hasHitPlayer = false; // 1回の攻撃でプレイヤーに当てたかどうか
	//bool m_hitWindowActive = false;// 攻撃判定ウィンドウがアクティブかどうか
	bool m_hitWindowActive = false;
	bool   m_engaged = false;         // 是否处于交战（进入过 chaseRect 即置 true）
	double m_yLoseTimer = 0.0;        // 玩家在不同Y轴的累计时长
	double m_yLoseThresholdSec = 5.0; // 满足该秒数才允许脱战

	// 纵向判定阈值（像素）：|player.y - enemy.y| 超过才算“不同Y轴”
	double m_ySepThreshold = 48.0;    // 按你素材调，常用=半个身位~1格

	double m_flipThreshold = 12.0;     // 水平方向改向阈值(px)
	double m_yFacingGate = 48.0;     // 仅当 |dy| <= 该值才允许改向（同层判定）
	double m_faceFlipCooldownMax = 0.30; // 改向冷却(s)
	double m_faceFlipCooldown = 0.0;  // 当前冷却



	AnimState_Enemy1 m_state{ AnimState_Enemy1::Idle };	// 現在のアニメーション状態
	HashTable<AnimState_Enemy1, AnimDesc_Enemy1> m_anims{	// アニメーションの説明
		{ AnimState_Enemy1::Idle, {0, 0, 8, 0.09, true } },
		{ AnimState_Enemy1::Run,  { 1, 3, 9, 0.10, true } },
		{ AnimState_Enemy1::Dead,  { 4, 1, 3, 0.25, false } },
		{ AnimState_Enemy1::Attack,  { 3,2, 4, 0.20, false } },

	};
	int32  m_frameIndex{ 0 };	// 現在のフレームインデックス
	double m_time{ 0.0 };		// アニメーション時間管理用

	void setState(AnimState_Enemy1 s) {	// アニメーション状態を設定
		if (m_state != s)
		{
			m_state = s;
			m_time = 0.0;        // フレーム更新タイマーをリセット
 			m_frameIndex = 0;    // アニメーションの最初のフレームに戻す
		}
	}


public:
	Enemy_1(Vec2 pos, double stride)
		: m_Position(pos)
		, m_stride(stride) {
	}// コンストラクタ

	//getter 
	Vec2 getPosition() const { return m_Position; }
	Vec2 getScale() const { return m_Scale; }
	float getSpeed() const { return m_Speed; }
	bool isFacingRight() const { return m_FaceRight; }


	int getAttack() const { return m_Attack; }
	float getAttackRange() const { return m_AttackRange; }
	Vec2 getHitbox() const { return m_hitBox; }


	//setter
	Vec2 setPosition(const Vec2 pos) { return m_Position = pos; }
	Vec2 setScale(const Vec2 scale) { return m_Scale = scale; }
	void setSpeed(float speed) { m_Speed = speed; }
	void setFaceRight(bool faceRight) { m_FaceRight = faceRight; }


	void setAttack(int attack) { m_Attack = attack; }
	void setAttackRange(float range) { m_AttackRange = range; }

	void setHitbox(Vec2 hitbox) { m_hitBox = hitbox; }

	Enemy_1& GetEnemy() { return *this; }

	void update(Player& player, Game_Map& map, AllEffect& alleffe);

	void draw(const Game_Map& CameraPos) const;

	void die();// 死亡処理
	bool pendingRemoval() const { return m_pendingRemoval; }
	bool IsDead() const { return m_dead; }

	RectF hurtRect(const Vec2& cam) const; // ダメージ判定矩形を取得
	RectF hurtRectAt(const Vec2& pos) const;// 指定位置での当たり判定矩形取得

	//double forwardClearance(const Game_Map& map, double baseW, double baseH, double lead, double maxForward, int dir) const;
	RectF attackRect(const Game_Map& map) const; // 攻撃判定矩形を取得
	RectF chaseRect(const Game_Map& map) const;// 追跡判定矩形を取得

	Line makeGroundProbeLine(const Vec2& cam, bool debug) const;// 地面探査用の線分を作成
};

