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

