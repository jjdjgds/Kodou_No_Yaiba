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
	if (pBpm>=120)//バーサーカーモード
	{

	}
	else if (pBpm < 60 && pBpm >= 80)//ザ・ワールドモード
	{

	}
	else if (pBpm < 70 && pBpm >= 130)//警告
	{

	}
	else if (pBpm < 60 || pBpm >= 140)
	{

	}






}
