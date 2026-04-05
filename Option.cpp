#include "stdafx.h"
#include "Option.hpp"

Option::Option()
{
	const double margin = 20;
	const SizeF btnSize{ 160, 50 };

	m_btnTitle = RectF(
		margin,
		Scene::Height() - btnSize.y - margin,
		btnSize
	);

	m_btnExit = RectF(
	Scene::Width() - btnSize.x - margin,
	Scene::Height() - btnSize.y - margin,
	btnSize
	);

	m_btnBack = RectF(
		margin,
		Scene::Height() - btnSize.y - margin,
		btnSize
	);

	Vec2 center = Scene::CenterF();

	m_sliderBar = RectF(Arg::center = center, SizeF{ 400, 10 });

	Vec2 knobPos = m_sliderBar.leftCenter().movedBy(m_sliderBar.w * m_volume, 0);
	m_knob = Circle(knobPos, 12);
}

bool Option::shouldGoTitle() const
{
	return m_btnTitle.leftClicked();
}

bool Option::shouldExit() const
{
	return m_btnExit.leftClicked();
}

bool Option::shouldBack() const
{
	return m_btnBack.leftClicked();
}

void Option::update()
{

	if (!m_isOpen) return;

	const Point mouse = Cursor::Pos();

	// Start dragging
	if (MouseL.down() && m_knob.mouseOver())
	{
		m_dragging = true;
	}

	// Stop dragging
	if (MouseL.up())
	{
		m_dragging = false;
	}

	// Dragging logic
	if (m_dragging)
	{
		double x = Clamp<double>(mouse.x, m_sliderBar.leftX(), m_sliderBar.rightX());

		m_knob.center.x = x;

		// Convert position → volume (0~1)
		m_volume = (x - m_sliderBar.leftX()) / m_sliderBar.w;
	}
}

void Option::draw() const
{
	if (!m_isOpen) return;

	// Dark overlay
	RectF{ Scene::Rect() }.draw(ColorF(0, 0, 0, 0.6));

	auto drawButton = [](const RectF& r, String text)
		{
			bool hover = r.mouseOver();

			r.rounded(10).draw(hover ? ColorF(1, 1, 1, 0.25) : ColorF(1, 1, 1, 0.1));
			r.rounded(10).drawFrame(2, hover ? Palette::Orange : Palette::Gray);

			FontAsset(U"Bold")(text).drawAt(r.center(), Palette::White);
		};

	if (m_mode == Mode::Game)
	{
		drawButton(m_btnTitle, U"TITLE");
		drawButton(m_btnExit, U"EXIT");
	}
	else // Mode::Title
	{
		drawButton(m_btnBack, U"BACK");
	}

	FontAsset(U"Bold")(U"Volume").drawAt(Scene::CenterF().movedBy(0, -80));

	// Slider bar
	m_sliderBar.draw(ColorF(0.3));

	// Filled part
	RectF filled = m_sliderBar;
	filled.w *= m_volume;
	filled.draw(ColorF(0.8, 0.5, 0.2));

	// Knob
	m_knob.draw(Palette::White);

	// Percentage text
	int percent = static_cast<int>(m_volume * 100);
	FontAsset(U"Bold")(Format(percent, U"%"))
		.drawAt(Scene::CenterF().movedBy(0, 50));
}

void Option::setVolume(double v)
{
	m_volume = Clamp(v, 0.0, 1.0);
}
