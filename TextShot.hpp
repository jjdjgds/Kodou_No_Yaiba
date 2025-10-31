#pragma once
#include <Siv3D.hpp>


struct TextShot {

	double  showDurationSec = 0.6;
	double  cooldownSec = 0.0;
	Vec2 baseOffset = { 0, 0 };
	String fontName = U"TitleFont";

	bool    prevCond = false;
	double  timer = 0.0;
	double  cooldown = 0.0;
	char32  currentCh = U'\0';

	
	bool update(bool condition, double dt, char32 ch = U'\0', double durationOverride = -1.0)
	{
		if (timer > 0.0) { timer = Max(0.0, timer - dt); }
		if (cooldown > 0.0) { cooldown = Max(0.0, cooldown - dt); }

		bool fired = false;

		if (condition && !prevCond && cooldown <= 0.0) {
			currentCh = (ch != U'\0' ? ch : currentCh != U'\0' ? currentCh : U'!');
			timer = (durationOverride > 0.0 ? durationOverride : showDurationSec);
			cooldown = cooldownSec;
			fired = true;
		}

		prevCond = condition;
		return fired;
	}

	void draw(const Vec2& worldPos, bool faceRight, const Vec2& cam,
			  const ColorF& color = ColorF{ 1.0 }, double size = 32.0, Vec2 baseOffset = Vec2{0,0}) const
	{
		if (timer <= 0.0 || currentCh == U'\0') return;

		const double shiftX = (faceRight ? -baseOffset.x : +baseOffset.x);
		const Vec2 screenPos = { worldPos.x + shiftX - cam.x, worldPos.y - baseOffset.y - cam.y };

		if (FontAsset::IsRegistered(fontName)) {
			FontAsset(fontName)(String{ currentCh }).draw(size, screenPos, color);
		}
		else {
			Font{ 24 }(String{ currentCh }).drawAt(screenPos, color);
		}
	}

	void trigger(char32 ch = U'\0', double durationOverride = -1.0)
	{
		currentCh = (ch != U'\0' ? ch : U'\0');
		timer = (durationOverride > 0.0 ? durationOverride : showDurationSec);
		// 不改 cooldown；如需触发后进入冷却，可在外部把 cooldown= cooldownSec
	}

	void reset() {
		prevCond = false;
		timer = 0.0;
		cooldown = 0.0;
		currentCh = U'\0';
	}
};
