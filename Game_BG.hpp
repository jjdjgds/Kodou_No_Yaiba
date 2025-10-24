#pragma once
#include "Siv3D.hpp"

class Game_BG
{
private:
	String m_texture_name;    // The background image
	Vec2 m_size;          // Draw size (e.g. Scene::Size())
	Vec2 m_offset;        // Current offset for scrolling
	Vec2  m_scrollSpeed; // Scroll speed (pixels per frame)
	bool m_loop;          // Whether to tile/loop horizontally
public:
	Game_BG()
		: m_size(Scene::Size()), m_offset(0, 0), m_scrollSpeed(0, 0), m_loop(true) {
	}

	void setAsset(const String& assetName);
	void update();
	void draw() const;

	void setSize(const Vec2& size) { m_size = size; }
	void setScrollSpeed(Vec2 speed) { m_scrollSpeed = speed; }
	void setLoop(bool loop) { m_loop = loop; }
	void resetOffset() { m_offset = Vec2{ 0,0 }; }
};
