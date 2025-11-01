#pragma once
#include <Siv3D.hpp>
#include "Player.hpp"
#include "Bullet.hpp"
#include "TextShot.hpp"
#include "AllEffect.h"
class Bullet;

enum class AnimState_Enemy2 {Idle,Run,Dead,Attack,};// アニメーション状態列挙型
enum class Behavior_Enemy2 { Patrol, Chase, Attack,Elude };// 行動パターン列挙型
enum class PatrolPhase_Enemy2 { Move, Wait, };// 巡回フェーズ列挙型

struct AnimDesc_Enemy2 {// アニメーションの説明構造体
	int   row;
	int   start;
	int  frames;
	double frameTime;
	bool   loop = true;
};



class Enemy_2
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
	float m_speedBase = m_Speed;// 元の移動速度
	double m_eludeSpeedMul = 1.0;

	bool   m_dead = false;              // 已进入死亡流程
	bool   m_pendingRemoval = false;

	double m_hitOffsetY = 0.0;// 当たり判定Y

	Vec2 m_Scale = { 100.0 ,100.0 };// 大きさ
	Vec2 m_hitBox = { 70.0 ,100.0 };// 当たり判定サイズ


	float m_gravity = 1800.0;// 重力
	float m_velY = 0.0;// Y方向速度
	bool   m_onGround = false;

	double m_stride = 0.0f; // 巡回範囲
	bool m_strideRandom = true;// 巡回範囲ランダムフラグ
	double m_strideMin = m_stride - 100.0;// 巡回範囲最小値
	double m_strideMax = m_stride + 100.0;// 巡回範囲最大値
	double m_budget = m_stride;// 巡回予算

	bool m_isRunning = false;

	Behavior_Enemy2 m_mode = Behavior_Enemy2::Patrol;
	PatrolPhase_Enemy2 m_phase = PatrolPhase_Enemy2::Wait;

	double m_phaseTimer = 0.0;// 巡回フェーズタイマー

	double m_waitMin = 0.8;// 待機フェーズ時間範囲
	double m_waitMax = 2.0;// 待機フェーズ時間範囲
	double m_moveMin = 1.5;// 移動フェーズ時間範囲
	double m_moveMax = 3.5;// 移動フェーズ時間範囲

	void enterPhase(PatrolPhase_Enemy2 p) {
		m_phase = p;
		if (p == PatrolPhase_Enemy2::Wait) m_phaseTimer = Random(m_waitMin, m_waitMax);
		else                        m_phaseTimer = Random(m_moveMin, m_moveMax);
	}




	bool m_takeDamage = false; // ダメージを受けたかどうか


	bool m_attackFlag = false; // 攻撃フラグ
	int m_Attack;			  //攻撃力
	double m_attackCooldown = 0.0;
	double m_attackCooldownMax = 1.5;// 攻撃クールダウン最大値
	float m_AttackRange;	  //攻撃範囲
	bool m_hasHitPlayer = false; // 1回の攻撃でプレイヤーに当てたかどうか

	bool   m_engaged = false;// 交戦モードフラグ
	double m_yLoseTimer = 0.0;// Y方向見失いタイマー
	double m_yLoseThresholdSec = 5.0;// Y方向見失いタイム閾値


	double m_ySepThreshold = 48.0;// Y方向分離閾値

	double m_flipThreshold = 12.0;// 向き変更閾値
	double m_yFacingGate = 48.0;// Y向き変更ゲート
	double m_faceFlipCooldownMax = 0.30;// 向き反転クールダウン最大値
	double m_faceFlipCooldown = 0.0;// 向き反転クールダウンタイマー



	AnimState_Enemy2 m_state{ AnimState_Enemy2::Idle };	// 現在のアニメーション状態
	HashTable<AnimState_Enemy2, AnimDesc_Enemy2> m_anims{	// アニメーションの説明
		{ AnimState_Enemy2::Idle, {0, 0, 9, 0.09, true } },
		{ AnimState_Enemy2::Run,  { 1, 4, 7, 0.10, true } },
		{ AnimState_Enemy2::Dead,  { 3, 3, 3, 0.25, false } },
		{ AnimState_Enemy2::Attack,  { 3,1, 2, 0.13, false } },

	};
	int32  m_frameIndex{ 0 };	// 現在のフレームインデックス
	double m_time{ 0.0 };		// アニメーション時間管理用

	void setState(AnimState_Enemy2 s) {	// アニメーション状態を設定
		if (m_state != s)
		{
			m_state = s;
			m_time = 0.0;        // フレーム更新タイマーをリセット
			m_frameIndex = 0;    // アニメーションの最初のフレームに戻す
		}
	}

	Array<class Bullet> m_bullets;// 弾丸配列
	double m_bulletSpeed = 700.0;// 弾丸速度
	SizeF  m_bulletHit = SizeF{ 20,12 };// 弾丸当たり判定サイズ
	double m_bulletLife = 2.0;// 弾丸寿命(秒)

	bool   m_firedThisAttack = false;// この攻撃アニメーションで弾を発射したかどうか
	int    m_fireFrame = 2;

	void fireBullet();// 弾丸発射処理
	void updateBullets(double dt, Player& player, Game_Map& map);// 弾丸更新処理

public:
	Enemy_2(Vec2 pos, double stride)
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

	Enemy_2& GetEnemy() { return *this; }

	void update(Player& player, Game_Map& map, AllEffect& alleffe);

	void draw(const Game_Map& CameraPos) const;

	void die();// 死亡処理
	bool pendingRemoval() const { return m_pendingRemoval; }
	bool IsDead() const { return m_dead; }


	RectF hurtRect(const Vec2& cam) const; // ダメージ判定矩形を取得
	RectF hurtRectAt(const Vec2& pos) const;// 指定位置での当たり判定矩形取得

	RectF attackRect(const Game_Map& map) const; // 攻撃判定矩形を取得
	RectF eludeRect(const Game_Map& map) const; // 回避判定矩形を取得
	RectF chaseRect(const Game_Map& map) const;// 追跡判定矩形を取得

	Line makeGroundProbeLine(const Vec2& cam, bool debug) const;// 地面探査用の線分を作成

	static bool groundBehind(const Enemy_2& self, const Game_Map& map)// 敵の背後に地面があるかどうか
	{
		// 使用“角色尺寸”而不是“世界坐标值”来决定探测长度
		const double back = 1.5 * self.m_hitBox.x;   // 向后的水平探测长度（略小于宽度）
		const double down = 0.9 * self.m_hitBox.y;   // 向下的垂直探测长度（约等于高度）

		// 从“脚边”起笔更稳：m_Position + 脚底偏移
		const Vec2 foot = self.m_Position.movedBy(0, self.m_hitOffsetY);

		// 背后方向：面朝右→向左后（-x,+y）；面朝左→向右后（+x,+y）
		const Vec2 dir = (self.m_FaceRight ? Vec2{ -back, +down }
		: Vec2{ +back, +down });

		// 世界坐标线段；map.CheckCollision_Line 也要吃“世界坐标”的线
		const Line probe(foot, foot + dir);
		const Line probe1(foot- map.getCameraPos(), foot - map.getCameraPos() + dir);
		probe1.draw(2, Palette::Aqua);
		
		return map.CheckCollision_Line(probe);
	};

	static bool groundBehind_1(const Enemy_2& self, const Game_Map& map)// 敵の背後に地面があるかどうか
	{
		// 使用“角色尺寸”而不是“世界坐标值”来决定探测长度
		const double back = 0.8 * self.m_hitBox.x;   // 向后的水平探测长度（略小于宽度）

		// 从“脚边”起笔更稳：m_Position + 脚底偏移
		const Vec2 foot = self.m_Position.movedBy(0, self.m_hitOffsetY);

		// 背后方向：面朝右→向左后（-x,+y）；面朝左→向右后（+x,+y）
		const Vec2 dir = (self.m_FaceRight ? Vec2{ -back, 0 }
		: Vec2{ +back, 0 });

		// 世界坐标线段；map.CheckCollision_Line 也要吃“世界坐标”的线
		const Line probe(foot, foot + dir);
		const Line probe1(foot - map.getCameraPos(), foot - map.getCameraPos() + dir);
		probe1.draw(2, Palette::Orangered);

		return map.CheckCollision_Line(probe);
	};

	static bool hasLineOfSight(const Enemy_2& self, Game_Map& map, const RectF& playerBox)
	{
		const Vec2 from = self.m_Position;
		const Vec2 to = playerBox.center();
		Vec2 dir = to - from;
		const double dist = dir.length();
		if (dist <= 0.0001) {
			return true;
		}
		dir /= dist;


		const double step = 6.0;// 探査ステップ
		const SizeF  probeSize{ 8, 8 };// 视线探测用矩形サイズ
		for (double t = 0.0; t <= dist; t += step) {
			const Vec2 p = from + dir * t;// 探査点

			if (playerBox.intersects(Circle{ p, 3.0 })) {// プレイヤーに到達
				return true;
			}

			if (map.CheckCollision_RecF(RectF{ Arg::center = p, probeSize })) {// 障害物に当たった
				return false;
			}
		}

		return true;
	}

};

