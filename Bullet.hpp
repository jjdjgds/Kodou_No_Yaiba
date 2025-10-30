#pragma once
#include <Siv3D.hpp>

class Game_Map;

class Bullet {
public:
	Bullet(const Vec2& worldPos, bool faceRight, double speed = 480.0,
		   const SizeF& hitSize = SizeF{ 20,12 }, double lifeSec = 3.0)
		: m_pos(worldPos)
		, m_vel((faceRight ? +1.0 : -1.0)* speed, 0.0)
		, m_faceRight(!faceRight)
		, m_hitSize(hitSize)
		, m_life(lifeSec)
	{
	}

	bool isAlive() const { return m_alive; }

	bool m_hasHitPlayer = false; // 1回の攻撃でプレイヤーに当てたかどうか

	void setDrawBiasLocal(const Vec2& v) { m_drawBiasLocal = v; }// 用于绘制偏移
	void setHitBiasLocal(const Vec2& v) { m_hitBiasLocal = v; }// 用于碰撞箱偏移

	RectF rectScreen(const Vec2& cam) const;// 画面座標系の矩形を取得
	RectF rectWorld() const;// ワールド座標系の矩形を取得

	void draw(const Game_Map& map) const;
	bool updateAndHit(
		double dt,
		const Game_Map& map,
        const RectF& pHitBoxScreen,
		const RectF& pAttackBoxScreen,
		const Vec2& cam,
		bool flg
	);// 更新とプレイヤー命中判定

private:
	Vec2 faceApply(const s3d::Vec2& local) const {
		return m_faceRight ? local : s3d::Vec2{ -local.x, local.y };
	}

	Vec2   m_pos;
	Vec2   m_vel;
	bool   m_faceRight = true;
	SizeF  m_hitSize{ 20,12 };
	double m_life = 3.0;
	bool   m_alive = true;

	// 偏移量
	Vec2   m_drawBiasLocal{ 0, 0 };// 描画用
	Vec2   m_hitBiasLocal{ 0, 0 };// 衝突箱用
};
