#include "stdafx.h"
#include "AllEffect.h"

void AllEffect::SetEffect(const Vec2& pos, const Vec2& scale, double frameMax,bool dir)
{
	for (auto& e : m_effects)
	{
		if (!e.active)
		{
			e.Reset(pos, scale, frameMax,dir);
			Print << U"SetEffect 呼び出し成功！pos:" << pos;
			return;
		}
	}
	Print << U"SetEffect 呼ばれたけど空きなし";
}


void AllEffect::UpdateEffect()
{
	double dt = 0.0;
	 dt += Scene::DeltaTime();
	for (auto& e : m_effects)
	{
		if (!e.active) continue;  // returnではなくcontinue！

		e.Update(dt);
	}
}

void AllEffect::DrawEffect() const
{
	const Texture& AttackTex = TextureAsset(U"AttackEffect");
	for (const auto& e : m_effects)
	{
		if (!e.active) continue;
		
		AttackTex
			.scaled(e.scale)
			.drawAt(e.pos);
	}
}
