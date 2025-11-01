#include "stdafx.h"
#include "Game_UI.hpp"
#include "Game.hpp"

void Game_UI::UIStun()
{
	// スタン用のアニメーション処理
	//心拍数のアニメーション再生速度はこの値をかえてねー
	const double NomalFrameDuration = STUN;
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
	const double NomalFrameDuration = WARNING;
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
	const double NomalFrameDuration = TIMECONTROL;
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
	const double NomalFrameDuration = NORMAL;
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
	const double NomalFrameDuration = DEAD;
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
	const double NomalFrameDuration = BERSERK;
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

void Game_UI::update(Player& player, const Game_Map& CameraPos)
{
	HeatanimTime += Scene::DeltaTime() * TimeStopManager::GetEnemyScale();

	const auto state = player.GetPlayerHeartState();
	const int bpm = player.GetPlayerBPM();

	// 赤フラグのオン・オフ制御
	if (bpm <= 60 || bpm >= 140)
		m_RedAutoFlag = true;
	else
		m_RedAutoFlag = false;

	// 状態別 UI 表示
	switch (state)
	{
	case HeartRateState::Stun:        UIStun(); break;
	case HeartRateState::Berserk:     UIBerserk(); break;
	case HeartRateState::TimeControl: UITimeControl(); break;
	case HeartRateState::Normal:      UINormal(); break;
	case HeartRateState::Dead:        UIDead(); break;
	default: break;
	}

	// 状態が変わったらBGM切り替え
	if (m_WasState != state)
	{
		const Audio& pHbSound = AudioAsset(U"FastBeat");
		const Audio& pLbSound = AudioAsset(U"SlowBeat");

		pHbSound.stop();
		pLbSound.stop();

		switch (state)
		{
		case HeartRateState::Berserk:
			pHbSound.setSpeed(1.0).play();
			break;
		case HeartRateState::Normal:
			pHbSound.setSpeed(0.7).play();
			break;
		case HeartRateState::TimeControl:
			pLbSound.setSpeed(1.0).play();
			break;
		default:
			break;
		}

		m_WasState = state;
	}
}


void Game_UI::draw(const Player& player, const Game_Map& CameraPos) const
{
	const Texture& BeatTex = TextureAsset(U"HeatBeat");
	const Texture& PlayerHP = TextureAsset(U"PlayerHP");
	const Texture& PlayerMedicle = TextureAsset(U"Medicine");
	
	const int32 frameWidth = 383;
	const int32 frameHeight = 158;

	const int32 medicleWidth = 400;
	const int32 medicleHeidht = 1090;
	auto state = player.GetPlayerHeartState();
	auto WasState = m_WasState;
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


	if( m_RedAutoFlag)
	{
		RectF{ 0,0,Scene::Width(),Scene::Height() }.draw(ColorF{ 1,0,0,0.1 });
	}
	
	
	// === 描画位置 ===
	Vec2 drawPos = Vec2{ 200, 200 };

	// === スプライト描画 ===
	BeatTex(n * frameWidth, y, frameWidth, frameHeight)
		.scaled(1.0)
		.drawAt(drawPos);

	for (int i = 0; i < player.GetPlayerHP(); i++)
	{
		PlayerHP(0, 0, 345, 300).scaled(0.25).drawAt(100 * i+100, 70);

	}
	int x = 0;
	switch (player.GetMedecine())
	{
	case 0:
		x = 5;
		break;

	case 1:
		x = 4;
	    break;
	case 2:
		x = 3;
	    break;
	case 3:
		x = 2;
	    break;
	case 4:
		x =1;
		break;
	case 5:
		x = 0;
		break;

	default:
		break;
	}
	
	PlayerMedicle(medicleWidth * x, 0, medicleWidth, medicleHeidht).scaled(0.18).rotated(3.14/6).drawAt(1500, 800);


	





	//Print << U"Heart Rate State: " << (state == HeartRateState::Stun ? U"Stun" :
	//		//state == HeartRateState::Warning ? U"Warning" :
	//		state == HeartRateState::Berserk ? U"Berserk" :
	//		state == HeartRateState::TimeControl ? U"TimeControl" :
	//		state == HeartRateState::Normal ? U"Normal" :
	//		state == HeartRateState::Dead ? U"Dead" : U"Unknown");
}

