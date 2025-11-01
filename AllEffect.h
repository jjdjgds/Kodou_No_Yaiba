#pragma once
#include <Siv3D.hpp>

#define EFFECT_MAX (500)

struct MyEffect
{
	Vec2 pos{ 0,0 };
	Vec2 scale{ 1,1 };
	bool pdirection = true; // true:右向き、false:左向き
	double frame = 0.0;
	double frameMax = 0.0;
	bool active = false;

	void Reset(const Vec2& p, const Vec2& s, double fmax, bool pd)
	{
		pdirection = pd;
		pos = p + Vec2{ pd ? +80.0 : -80.0, -10.0 }; // プレイヤー中心よりちょい前・少し上
		scale = s;
		frame = 0.0;
		frameMax = fmax;
		active = true;
	}

	void Update(double dt)
	{
		if (!active) return;

		frame += dt;
		double t = frame / frameMax; // 0.0～1.0

		// 速度変化（最初速く→減速）
		double baseSpeed = 5000.0;
		double speed = baseSpeed * (1.0 - t * 0.6);

		// 進行方向に移動
		pos.x += (pdirection ? +2.0 : -2.0) * speed * dt;

		// 消滅判定
		if (frame >= frameMax)
			active = false;
	}

	void Draw(const Texture& texObj) const
	{
		if (!active) return;

		double t = frame / frameMax; // 0.0～1.0
		double alpha = EaseOutExpo(1.0 - t);

		// スケール変化（横に伸び→収束）
		double stretch = 1.0 + (1.0 - t) * 1.5;
		Vec2 drawScale = Vec2{ scale.x * stretch, scale.y * 0.8 };

		double flip = (pdirection ? 1.0 : -1.0);

		//------------------------------------
		//  色変化部分（グラデーション）
		//------------------------------------
		// 白→赤→オレンジ（R=1.0, G上昇→減衰）
		ColorF startColor = ColorF(1.0, 0.0, 0.0);     // 白
		ColorF midColor = ColorF(0.0, 1.0, 0.0);     // 赤
		ColorF endColor = ColorF(0.0, 0.0, 1.0);     // オレンジ

		// 段階的補間（最初→中盤→終盤）
		ColorF effectColor;
		if (t < 0.3)
			effectColor = startColor.lerp(midColor, t / 0.3);
		else
			effectColor = midColor.lerp(endColor, (t - 0.3) / 0.7);

		effectColor.a = alpha; // 透明度適用

		//------------------------------------
		// 本体描画（2重ブラー風）
		//------------------------------------
		texObj
			.scaled(drawScale * 0.8)
			.mirrored(!pdirection)
			.drawAt(pos + Vec2{ flip * 45.0, 0 }, ColorF{ alpha * 0.4,alpha * 0.4, alpha * 0.4, alpha * 0.4 });

		texObj
			.scaled(drawScale)
			.mirrored(!pdirection)
			.drawAt(pos, effectColor);

		// 後光のような尾を少し白で
		}
};


class AllEffect
{
private:
	Array<MyEffect> m_effects;

public:
	AllEffect()
	{
		m_effects.resize(EFFECT_MAX);
	}

	void SetEffect(const Vec2& pos, const Vec2& scale, double frameMax, bool dir);

	void UpdateEffect();

	void DrawEffect() const;
};
