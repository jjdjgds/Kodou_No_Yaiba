/*==========================================================================
HPや心拍数といったゲームUIクラス[Game_UI.hpp]



													Author : hidetoshi muramatu

---------------------------------------------------------------------------



==========================================================================*/

#pragma once
# include <Siv3D.hpp>
#include "Player.hpp"
#define  STUN (0.9)
#define  WARNING (0.035)
#define  BERSERK (0.025)
#define  TIMECONTROL (0.8)
#define  NORMAL (0.12)
#define  DEAD (0.1)

class Enemy_Boss;



class Game_UI
{
private:
	Font m_font{ 48 };
	RectF m_hpBarBase{ 40, 40, 400, 40 };
	size_t m_frameIndex = 0;  //アニメーションフレームインデックス
	//Beatアニメーション
	Array<int32> m_BeatPatterns{ 0,1,2,3,4,5,6,7,8,9,10 };
	Array<int32> m_deadBeatPatterns{ 11,12,13,14,15 };
	double HeatanimTime = 0.0;    //アニメーション時間管理用
	bool m_RedAutoFlag = false;
	
	HeartRateState m_WasState = HeartRateState::Normal;
public:
	void UIStun();
	void UIWarning();
	void UITimeControl();
	void UIBerserk();


	void hBgmStop();

	void UINormal();
	void UIDead();
	void update(Player& player, const Game_Map& CameraPos);
	void draw(const Player& player, const Game_Map& CameraPos)const;

	void drawBossHP(const Enemy_Boss& boss) const;

};


