/*==========================================================================
エフェクト管理[AllEfect.h]



													Author : hidetoshi muramatu

---------------------------------------------------------------------------



==========================================================================*/



#pragma once
#include <Siv3D.hpp>

#define EFFECT_MAX (500)

struct MyEffect
{
	Vec2 pos{ 0,0 };
	Vec2 scale{ 1,1 };
	bool pdirection = true;
	double frame = 0.0;
	double frameMax = 0.0;
	bool active = false;

	void Reset(const Vec2& p, const Vec2& s, double fmax, bool pd)
	{
		pdirection = pd;
		pos = p + Vec2{ pd ? +80.0 : -80.0, -10.0 };
		scale = s;
		frame = 0.0;
		frameMax = fmax;
		active = true;
	}

	void Update(double dt)
	{
		if (!active) return;

		frame += dt;
		double t = frame / frameMax;

		double baseSpeed = 5000.0;
		double speed = baseSpeed * (1.0 - t * 0.6);
		pos.x += (pdirection ? +2.0 : -2.0) * speed * dt;

		if (frame >= frameMax)
			active = false;
	}

	void Draw(const Texture& texObj, const Vec2& cam) const
	{
		if (!active) return;

		Vec2 drawPos = pos - cam; // カメラ補正を追加

		double t = frame / frameMax;
		double alpha = EaseOutExpo(1.0 - t);
		double stretch = 1.0 + (1.0 - t) * 1.5;
		Vec2 drawScale = Vec2{ scale.x * stretch, scale.y * 0.8 };
		double flip = (pdirection ? 1.0 : -1.0);

		ColorF startColor = ColorF(1.0, 0.0, 0.0);
		ColorF midColor = ColorF(0.0, 1.0, 0.0);
		ColorF endColor = ColorF(0.0, 0.0, 1.0);

		ColorF effectColor = (t < 0.3) ? startColor.lerp(midColor, t / 0.3)
			: midColor.lerp(endColor, (t - 0.3) / 0.7);
		effectColor.a = alpha;

		texObj.scaled(drawScale * 0.8)
			.mirrored(!pdirection)
			.drawAt(drawPos + Vec2{ flip * 45.0, 0 }, ColorF{ alpha * 0.4, alpha * 0.4, alpha * 0.4, alpha * 0.4 });

		texObj.scaled(drawScale)
			.mirrored(!pdirection)
			.drawAt(drawPos, effectColor);
	}
};

class AllEffect
{
private:
	Array<MyEffect> m_effects;

public:
	AllEffect() { m_effects.resize(EFFECT_MAX); }

	void SetEffect(const Vec2& pos, const Vec2& scale, double frameMax, bool dir);
	void UpdateEffect();
	void DrawEffect(const Vec2& cam) const; // camを引数で受け取る
};
