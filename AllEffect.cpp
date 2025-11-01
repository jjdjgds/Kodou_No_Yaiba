#include "stdafx.h"
#include "AllEffect.h"


AllEffect Alleffects[EFFECT_MAX];


void AllEffect::UpdateEffect()
{
	for (auto &all : Alleffects)
	{
		if (!all.IsEffectActive())return;
		all.SetEffectFrame(GetEffectFrame() + Scene::DeltaTime());
		if (all.GetEffectFrame() >= all.GetEffectFrameMax())
		{
			all.SetEffectActive(false);
		}
	}

}

void AllEffect::DrawEffect()
{
	const Texture& AttackTex = TextureAsset(U"AttackEffect");
	for (auto& all : Alleffects)
	{
		if (!all.IsEffectActive())return;
		AttackTex(2810, 30, 2810, 30)
			.scaled(0.5)
			.drawAt(GetEffectPos());
	}
}
