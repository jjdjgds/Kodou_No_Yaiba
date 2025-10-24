#include "stdafx.h"
#include "Game_BG.hpp"


void  Game_BG::update()
{
	if (!m_loop || m_texture_name.isEmpty()) return;

	m_offset.x += m_scrollSpeed.x;

	// Wrap horizontally
	const double drawWidth = m_size.x;
	m_offset.x = fmod(m_offset.x, drawWidth);
	if (m_offset.x < 0) m_offset.x += drawWidth;
}

void Game_BG::draw() const
{
	if (m_texture_name.isEmpty()) return;

	TextureAsset tex(m_texture_name);

	const double drawWidth = tex.width();  // use native width
	const double sceneWidth = Scene::Width();

	// Draw first tile
	tex.draw(-m_offset.x, 0);

	// Draw second tile if needed
	if (drawWidth - m_offset.x < sceneWidth)
	{
		tex.draw(drawWidth - m_offset.x, 0);
	}
}

void  Game_BG::setAsset(const String& assetName)
{
	if (!TextureAsset::IsRegistered(assetName))
		throw Error(U"TextureAsset \"{}\" is not registered!"_fmt(assetName));

	m_texture_name = assetName;
}

/*
	Game_BG bg;
	bg.setAsset(U"Windmill");
	bg.setSize(Scene::Size());
	bg.setScrollSpeed(Vec2{ 2, 0 });
	bg.setLoop(true);
	bg.resetOffset();
	--->update()
	{
		bg.update();
	}
	--->draw() const
	{
		bg.draw();
	}
*/
