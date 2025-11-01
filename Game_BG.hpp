#pragma once
#include "Siv3D.hpp"

enum class BgMode { ScreenLocked, WorldLocked, Parallax };


class Game_BG
{
private:
	String m_texture_name;    // The background image
	Vec2 m_size;          // Draw size (e.g. Scene::Size())
	Vec2 m_offset;        // Current offset for scrolling
	Vec2  m_scrollSpeed; // Scroll speed (pixels per frame)
	bool m_loop = false;          // Whether to tile/loop horizontally

	bool   m_useSolid = false;
	Color  m_solidColor{ 20, 20, 20 };
	BgMode m_mode = BgMode::WorldLocked;
	double m_parallax = 0.3;        // Parallax 用，0.1~0.7 越小越“远”
	Size   m_worldSize{ 0, 0 };     // WorldLocked 用：整张背景在“关卡像素尺寸”
	Vec2   m_cam{ 0, 0 };

public:
	Game_BG(){}

	bool setAsset(const String& nameOrPath, const Color& fallback = Color{ 20,20,20 });

	void setMode(BgMode m) { m_mode = m; }
	void setParallax(double k) { m_parallax = k; }
	void setWorldSize(const Size& s) { m_worldSize = s; }
	void syncWithCamera(const Vec2& cam) { m_cam = cam; }

	void update();
	void draw() const;

	void setSize(const Vec2& size) { m_size = size; }
	void setScrollSpeed(Vec2 speed) { m_scrollSpeed = speed; }
	void setLoop(bool loop) { m_loop = loop; }
	void resetOffset() { m_offset = Vec2{ 0,0 }; }
};
