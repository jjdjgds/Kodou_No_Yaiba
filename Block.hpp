#pragma once
#include "Siv3D.hpp"

class Block
{
private:
	Vec2 m_pos;
	Vec2 m_size;
	int	 m_type = 0;
	bool m_use = false;

	// Shared textures (declared only once globally)
	static Texture m_texGrass;
	static Texture m_texWall;
	static Texture m_texDirt;
	static Texture m_texWater;

public:
	Block() = default;
	~Block() = default;
	void UpdateBlock(void);
	void DrawBlock (void) const;
	void SetBlock(const Vec2& pos, const Vec2& size, int type);

	bool IsUsed() const;
	Vec2 GetPos() const;
	int  getType() const;

	// Load all textures once at the start of the game
	static void LoadTextures();
};

