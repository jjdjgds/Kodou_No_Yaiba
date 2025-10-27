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
	default:
		break;
	}
}

void Game_UI::draw(Player player, const Game_Map& CameraPos) const
{
	const Texture& BeatTex = TextureAsset(U"HeatBeat");

	int pBpm = player.GetPlayerBPM();

	const int32 frameWidth = 379;
	const int32 frameHeight = 158;

	const auto state = player.GetPlayerHeartState();

	//  修正：4x3のスプライトシートでのインデックス計算 ★★★
	int32 n = 0;  // X方向のインデックス（0-3）
	int32 y = 0;  // Y座標


	// バーサーカー用のアニメーション
	if (!m_BeatPatterns.empty() && m_frameIndex < m_BeatPatterns.size())
	{
		int patternIndex = m_BeatPatterns[m_frameIndex];

		// 4x3グリッドでの位置計算
		n = patternIndex % 4;           // 横方向（0-3）
		int row = patternIndex / 4;     // 縦方向（0-2）
		y = row * frameHeight;
	}
	
	// === 描画位置 ===
	Vec2 drawPos = Vec2{ 200, 200 };  // カメラ補正を削除（UIは固定位置）

	// === スケール ===
	const double scaleX = 1.0;
	const double scaleY = 1.0;

	// === スプライト描画 ===
	BeatTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(scaleX, scaleY)
		.drawAt(drawPos);

	
}
