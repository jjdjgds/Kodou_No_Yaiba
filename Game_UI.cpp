#include "stdafx.h"
#include "Game_UI.hpp"
#include "Game.hpp"

void Game_UI::UIStun()
{
	// スタン用のアニメーション処理
	//心拍数のアニメーション再生速度はこの値をかえてねー
	const double NomalFrameDuration = 1.0;
	if (HeatanimTime >= NomalFrameDuration)
	{
		HeatanimTime -= NomalFrameDuration;

		// m_BeatPatterns の総フレーム数で循環させる
		if (!m_BeatPatterns.empty())
		{
			m_frameIndex = (m_frameIndex + 1) % m_BeatPatterns.size();
		}
		else
		{
			m_frameIndex = 0;
		}
	}
}

void Game_UI::UIWarning()
{
	//心拍数のアニメーション再生速度はこの値をかえてねー
	const double NomalFrameDuration = 0.98;
	if (HeatanimTime >= NomalFrameDuration)
	{
		HeatanimTime -= NomalFrameDuration;

		// m_BeatPatterns の総フレーム数で循環させる
		if (!m_BeatPatterns.empty())
		{
			m_frameIndex = (m_frameIndex + 1) % m_BeatPatterns.size();
		}
		else
		{
			m_frameIndex = 0;
		}
	}
}

void Game_UI::UITimeControl()
{
	// タイムコントロール用のアニメーション処理
	//心拍数のアニメーション再生速度はこの値をかえてねー
	const double NomalFrameDuration = 0.8;
	if (HeatanimTime >= NomalFrameDuration)
	{
		HeatanimTime -= NomalFrameDuration;

		// m_BeatPatterns の総フレーム数で循環させる
		if (!m_BeatPatterns.empty())
		{
			m_frameIndex = (m_frameIndex + 1) % m_BeatPatterns.size();
		}
		else
		{
			m_frameIndex = 0;
		}
	}
}

void Game_UI::UINormal()
{
	//心拍数のアニメーション再生速度はこの値をかえてねー
	const double NomalFrameDuration = 0.3;
	if (HeatanimTime >= NomalFrameDuration)
	{
		HeatanimTime -= NomalFrameDuration;

		// m_BeatPatterns の総フレーム数で循環させる
		if (!m_BeatPatterns.empty())
		{
			m_frameIndex = (m_frameIndex + 1) % m_BeatPatterns.size();
		}
		else
		{
			m_frameIndex = 0;
		}
	}
}

void Game_UI::UIDead()
{


	//心拍数のアニメーション再生速度はこの値をかえてねー
	const double NomalFrameDuration = 0.1;
	if (HeatanimTime >= NomalFrameDuration)
	{
		HeatanimTime -= NomalFrameDuration;

		//// まだ最後のフレームに到達していない場合のみ進める
		//if (m_frameIndex < static_cast<int>(m_deadBeatPatterns.size()) - 1)
		//{
		//	m_frameIndex++;
		//}
		// m_BeatPatterns の総フレーム数で循環させる
		if (!m_deadBeatPatterns.empty())
		{
			m_frameIndex = (m_frameIndex + 1) % m_deadBeatPatterns.size();
		}
		else
		{
			m_frameIndex = 0;
		}
	}

}

void Game_UI::UIBerserk()
{
	// バーサーカー用のアニメーション処理
	//心拍数のアニメーション再生速度はこの値をかえてねー
	const double NomalFrameDuration = 0.12;
	if (HeatanimTime >= NomalFrameDuration)
	{
		HeatanimTime -= NomalFrameDuration;

		// m_BeatPatterns の総フレーム数で循環させる
		if (!m_BeatPatterns.empty())
		{
			m_frameIndex = (m_frameIndex + 1) % m_BeatPatterns.size();
		}
		else
		{
			m_frameIndex = 0;
		}
	}
}

void Game_UI::update(Player player, const Game_Map& CameraPos)
{
	HeatanimTime += Scene::DeltaTime();

	const auto state = player.GetPlayerHeartState();

	switch (state)
	{
	case HeartRateState::Stun:
		UIStun();
		break;
	case HeartRateState::Warning:
		UIWarning();
		break;
	case HeartRateState::Berserk:
		UIBerserk();
		break;
	case HeartRateState::TimeControl:
		UITimeControl();
		break;
	case HeartRateState::Normal:
		UINormal();
		break;
	case HeartRateState::Dead:
		UIDead();
		break;

	default:
		break;
	}
}

void Game_UI::draw(Player player, const Game_Map& CameraPos) const
{
	const Texture& BeatTex = TextureAsset(U"HeatBeat");

	const int32 frameWidth = 383;
	const int32 frameHeight = 158;

	const auto state = player.GetPlayerHeartState();

	int32 n = 0;
	int32 y = 0;

	// === Dead以外 ===
	if (state != HeartRateState::Dead)
	{
		if (!m_BeatPatterns.empty() && m_frameIndex < m_BeatPatterns.size())
		{
			int patternIndex = m_BeatPatterns[m_frameIndex];

			n = patternIndex % 4;
			int row = patternIndex / 4;
			y = row * frameHeight;
		}
	}
	// === Dead専用 ===
	else
	{
		if (!m_deadBeatPatterns.empty() && m_frameIndex < m_deadBeatPatterns.size())
		{
			int patternIndex = m_deadBeatPatterns[m_frameIndex];
			int row = 0;

			if (patternIndex == 11)
			{
				row = 2; n = 3;
			}
			else if (patternIndex == 12)
			{
				row = 3; n = 0;
			}
			else if (patternIndex == 13)
			{
				row = 3; n = 1;
			}
			else if (patternIndex == 14)
			{
				row = 3; n = 2;
			}
			else if (patternIndex == 15)
			{
				row = 3; n = 3;
			}

			y = row * frameHeight;
		}
	}

	// === 描画位置 ===
	Vec2 drawPos = Vec2{ 200, 200 };

	// === スプライト描画 ===
	BeatTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(1.0)
		.drawAt(drawPos);
}
