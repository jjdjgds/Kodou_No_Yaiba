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
void Block::DrawBlock(const Vec2& cameraOffset) const
{

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
