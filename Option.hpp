#pragma once
# include <Siv3D.hpp>
class Option
{
public:
	enum class Mode
	{
		Game,   // TITLE + EXIT
		Title,  // BACK only
	};
private:
	bool m_isOpen = false;

	Mode m_mode = Mode::Title;

	RectF m_btnTitle;
	RectF m_btnExit;
	RectF m_btnBack;

	double m_volume = 0.7; // default volume (0.0 ~ 1.0)
	RectF m_sliderBar;
	Circle m_knob;
	
	bool m_dragging = false;

public:
	Option();

	void setMode(Mode mode) { m_mode = mode; }

	void update();
	void draw() const;

	bool isOpen() const { return m_isOpen; }
	void toggle() { m_isOpen = !m_isOpen; }

	void setVolume(double v);
	double& volume() { return m_volume; }

	bool shouldGoTitle() const;
	bool shouldExit() const;
	bool shouldBack() const;
};

