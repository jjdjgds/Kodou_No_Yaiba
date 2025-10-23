#include "stdafx.h"
#include "Block.hpp"

//Texture Block::m_texGrass;
//Texture Block::m_texWall;
//Texture Block::m_texDirt;
//Texture Block::m_texWater;

void Block::LoadTextures()
{
	//m_texGrass = Texture(U"assets/grass.png");
	//m_texWall = Texture(U"assets/wall.png");
}

void Block::SetBlock(const Vec2& pos, const Vec2& size, int type)
{
	m_pos = pos;
	m_size = size;
	m_type = type;
	m_use = true;
}

void Block::UpdateBlock(void)
{
}
void Block::DrawBlock(void) const
{
	if (!m_use) return;

	switch (m_type)
	{
	case BLOCK_EMPTY:// emty
		break;

	case BLOCK_SOLID: // wall
		//m_texWall.resized(m_size).draw(m_pos);
		TextureAsset(U"Windmill").resized(m_size).draw(m_pos);
		//RectF(m_pos, m_size).draw(ColorF(0.8, 0.2, 0.2)); // red for debug
		break;
	default:
		RectF(m_pos, m_size).draw(ColorF(0.8, 0.2, 0.2)); // red for debug
		break;
	}
}

bool Block::IsUsed() const
{
	return m_use;
}
Vec2 Block::GetPos() const
{
	return m_pos;
}

int  Block::getType() const
{
	return m_type;
}
