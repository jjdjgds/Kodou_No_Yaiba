#pragma once

#include <Siv3D.hpp>
#define EFFECT_MAX (500)//エフェクト最大数
class AllEffect
{

private:
	Vec2 m_EffectPos;
	Vec2 m_EffectScale;
	int  m_EffectTex;
	float m_EffectFrame;
	float m_EffectFrameMax;
	bool m_EffectActive;


public:
	Vec2 GetEffectPos() const { return m_EffectPos; }
	Vec2 GetEffectScale() const { return m_EffectScale; }
	int GetEffectTex() const { return m_EffectTex; }
	void SetEffectTex(int tex) { m_EffectFrame = tex; }
	float GetEffectFrame()const { return m_EffectFrame; }
	void SetEffectFrame(float frame) { m_EffectFrame = frame; }
	float GetEffectFrameMax() const { return m_EffectFrameMax; }
	void SetEffectFrameMax(float a) { m_EffectFrameMax = a; }
	bool IsEffectActive() const { return m_EffectActive; }
	void SetEffectActive(bool acv) { m_EffectActive = acv; }

	void SetEffect(Vec2 pos, Vec2 scale, int tex, float frameMax)
	{
		m_EffectPos = pos;
		m_EffectScale = scale;
		m_EffectTex = tex;
		m_EffectFrame = 0.0f;
		m_EffectFrameMax = frameMax;
		m_EffectActive = true;
	}

	void UpdateEffect();
	void DrawEffect();
};
