#include "stdafx.h"
#include "AllEffect.h"
#include "Game.hpp"

void AllEffect::SetEffect(const Vec2& pos, const Vec2& scale, double frameMax, bool dir)
{
	for (auto& e : m_effects)
	{
		if (!e.active)
		{
			e.Reset(pos, scale, frameMax, dir);
			//Print << U"SetEffect 呼び出し成功！pos:" << pos;
			return;
		}
	}
	//Print << U"SetEffect 呼ばれたけど空きなし";
}

void AllEffect::UpdateEffect()
{
	double dt = Scene::DeltaTime() * TimeStopManager::GetEnemyScale();
	for (auto& e : m_effects)
	{
		if (!e.active) continue;
		e.Update(dt);
	}
}

void AllEffect::DrawEffect(const Vec2& cam) const
{
	const Texture& AttackTex = TextureAsset(U"AttackEffect");
	for (const auto& e : m_effects)
	{
		if (!e.active) continue;
		e.Draw(AttackTex, cam); // カメラ補正付き描画
	}
}
