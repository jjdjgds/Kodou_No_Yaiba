#include "stdafx.h"
#include "Game_UI.hpp"

void Game_UI::draw( Player player)const
{
	//// Draw HP bar background
	//m_hpBarBase.draw(ColorF(0.2));

	//double hpRatio = static_cast<double>(player.GetPlayerHP()) / player.GetPlayerMaxHP();

	//// Draw HP bar fill directly
	//RectF(m_hpBarBase.x, m_hpBarBase.y, m_hpBarBase.w * hpRatio, m_hpBarBase.h)
	//	.draw(HSV(0, 0.7, 0.9));

	//// Draw HP text
	//m_font(U"HP: {} / {}"_fmt(player.GetPlayerHP(), player.GetPlayerMaxHP())).draw(Arg::topLeft = Vec2{40, 90}, Palette::White);

	//// Draw score
	//m_font(U"BPM: {}"_fmt(player.GetPlayerBPM())).draw(Arg::topLeft = Vec2{ 40, 150 }, Palette::Red);
	int pBpm = player.GetPlayerBPM();
	//BPMを貰ってきて
	






}

HeartRateState GetHeartRateState(int bpm)
{
	if (bpm <= 60 || bpm >= 140)
		return HeartRateState::Stun;

	if ((bpm >= 61 && bpm <= 70) || (bpm >= 130 && bpm <= 139))
		return HeartRateState::Warning;

	if (bpm >= 120 && bpm <= 129)
		return HeartRateState::Berserk;

	if (bpm >= 71 && bpm <= 80)
		return HeartRateState::TimeControl;

	return HeartRateState::Normal;
}
