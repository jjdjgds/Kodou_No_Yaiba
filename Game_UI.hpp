#pragma once
# include <Siv3D.hpp>
#include "Player.hpp"

class Game_UI
{
private:
	Font m_font{ 48 };
	RectF m_hpBarBase{ 40, 40, 400, 40 };
public:
	
	void draw(Player player)const; 
};
enum class HeartRateState
{
	Stun,          // スタン（60以下 or 140以上）
	Warning,       // 警告（61-70 or 130-139）
	Berserk,       // バーサーカー（120-129）
	TimeControl,   // ザ・ワールド（71-80）
	Normal         // 通常（81-119）
};
HeartRateState GetHeartRateState(int bpm);
