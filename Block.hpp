#pragma once
#include "Siv3D.hpp"

enum BLOCK_TYPE
{
	BLOCK_EMPTY = 0,
	BLOCK_SOLID = 1,
};

class Block
{
private:
	Vec2 m_pos;
	Vec2 m_size;
	int	 m_type = BLOCK_EMPTY;
	bool m_use = false;

public:
	Block() = default;
	void UpdateBlock(void);
	void DrawBlock (const Vec2& cameraOffset) const;
	void SetBlock(const Vec2& pos, const Vec2& size, int type);

	bool IsUsed() const;
	Vec2 GetPos() const;
	int  getType() const;

	RectF GetRect() const { return RectF(m_pos, m_size); }
	// Load all textures once at the start of the game
	static void LoadTextures();
};

