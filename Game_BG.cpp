#include "stdafx.h"
#include "Game_BG.hpp"


void  Game_BG::update()
{
	if (!m_loop || m_useSolid || m_texture_name.isEmpty()
		|| !TextureAsset::IsRegistered(m_texture_name)) {
		return;
	}
	m_offset += m_scrollSpeed * Scene::DeltaTime();

	const auto& tex = TextureAsset(m_texture_name);
	if (tex) {
		if (tex.width() > 0) m_offset.x = Math::Fmod(m_offset.x, static_cast<double>(tex.width()));
		if (tex.height() > 0) m_offset.y = Math::Fmod(m_offset.y, static_cast<double>(tex.height()));
	}
}

void Game_BG::draw() const
{
	if (m_useSolid || m_texture_name.isEmpty()
		|| !TextureAsset::IsRegistered(m_texture_name)) {
		Rect{ Scene::Rect() }.draw(m_solidColor);
		return;
	}

	const Texture tex = TextureAsset(m_texture_name);
	if (!tex) { Rect{ Scene::Rect() }.draw(m_solidColor); return; }

	switch (m_mode)
	{
	case BgMode::ScreenLocked:
		// 直接缩到窗口（1920x1080 → 1600x900）
		tex.resized(Scene::Size()).draw();               // ✅ 完全适配窗口
		break;

	case BgMode::WorldLocked:
	{
		// 整张背景按“关卡像素尺寸”拉伸，并随相机 1:1 移动
		const Size target = (m_worldSize.area() > 0) ? m_worldSize : Scene::Size();
		const Vec2 ofs = -m_cam;                      // 世界坐标 → 屏幕
		tex.resized(target).draw(ofs);                   // ✅ 和方块严格对齐
		break;
	}

	case BgMode::Parallax:
	{
		// 视差：根据相机做偏移（慢于相机）
		const double w = tex.width();
		double ox = Math::Fmod(-m_cam.x * m_parallax, w);
		if (ox > 0) ox -= w;
		tex.draw(ox, 0);
		if (w - ox < Scene::Width()) tex.draw(ox + w, 0);
		break;
	}
	}

	//TextureAsset tex(m_texture_name);
	//if (!tex) {
	//	Rect{ Scene::Rect() }.draw(m_solidColor);
	//	return;
	//}

	//const double drawWidth = tex.width();  // use native width
	//const double sceneWidth = Scene::Width();

	//// Draw first tile
	//tex.draw(-m_offset.x, 0);

	//// Draw second tile if needed
	//if (drawWidth - m_offset.x < sceneWidth)
	//{
	//	tex.draw(drawWidth - m_offset.x, 0);
	//}
}

bool  Game_BG::setAsset(const String& nameOrPath, const Color& fallback)
{
	if (TextureAsset::IsRegistered(nameOrPath)) {
		m_texture_name = nameOrPath;
		m_useSolid = false;
		return true;
	}

	if (FileSystem::Exists(nameOrPath)) {
		const String stem = FileSystem::BaseName(nameOrPath);
		const String key = U"_bg_" + stem;
		if (!TextureAsset::IsRegistered(key)) {
			TextureAsset::Register(key, nameOrPath);
		}
		m_texture_name = key;
		m_useSolid = false;
		m_loop = true;
		return true;
	}

	// 兜底纯色
	m_texture_name.clear();
	m_useSolid = true;
	m_solidColor = fallback;
	return false;
}

/*
	Game_BG Map_bg;
	Map_bg.setAsset(U"Windmill");
	Map_bg.setSize(Scene::Size());
	Map_bg.setScrollSpeed(Vec2{ 2, 0 });
	Map_bg.setLoop(true);
	Map_bg.resetOffset();
	--->update()
	{
		Map_bg.update();
	}
	--->draw() const
	{
		Map_bg.draw();
	}
*/
