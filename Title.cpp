# include "Title.hpp"


// Title シーンのコンストラクタ（IScene の初期化）

Title::Title(const InitData& init)
	: IScene{ init }
{

}

void Title::update()
{
	const Audio& audio = AudioAsset(U"TitleBgm");
	if(!audio.isPlaying())
	{
		audio.setVolume(0.7);
		audio.play();
	}

	const Vec2  center = Scene::CenterF();
	const SizeF btnSize{ 240, 60 };
	const double offsetY = 160;
	const double gapY = 200;

	RectF startBtn(Arg::center = center.movedBy(0, offsetY), btnSize);
	RectF exitBtn(Arg::center = center.movedBy(0, offsetY + gapY), btnSize);

	if (startBtn.mouseOver()) m_selected = 0;
	if (exitBtn.mouseOver())  m_selected = 1;

	if (startBtn.leftClicked()) {
		changeScene(State::Game);
		return;
	}
	if (exitBtn.leftClicked()) {
		System::Exit();
		return;
	}

	if (KeyUp.down() || KeyW.down()) { m_selected = (m_selected + 2 - 1) % 2; }
	if (KeyDown.down() || KeyS.down()) { m_selected = (m_selected + 1) % 2; }

	if (KeyEnter.down() || KeySpace.down())
	{
		if (m_selected == 0) {
			changeScene(State::Game);
			audio.stop();
		}
		else {
			System::Exit();
		}
		return;
	}
}

void Title::draw() const
{
	if (TextureAsset::IsRegistered(U"TitleBg")) {
		TextureAsset(U"TitleBg").resized(Scene::Size()).draw();
	}
	else {
		Scene::SetBackground(Color{ 20, 20, 20 });
	}

	const Vec2  center = Scene::CenterF();
	const SizeF btnSize{ 240, 60 };
	const double offsetY = 140;
	const double gapY = 90;

	const RectF startBtn(Arg::center = center.movedBy(0, offsetY), btnSize);
	const RectF exitBtn(Arg::center = center.movedBy(0, offsetY + gapY), btnSize);

	auto drawButton = [](const RectF& r, StringView text, bool selected)
		{
			const ColorF fill = selected ? ColorF{ 1.0, 1.0, 1.0, 0.25 }
			: ColorF{ 1.0, 1.0, 1.0, 0.12 };
			const ColorF frame = selected ? ColorF{ 1.0 } : ColorF{ 0.7 };
			r.rounded(12).draw(fill).drawFrame(2, 0, frame);

			String label = selected ? U"▶ " + String{ text } : String{ text };
			FontAsset(U"Bold")(label).drawAt(28, r.center(), Palette::White);
		};

	drawButton(startBtn, U"START", m_selected == 0);
	drawButton(exitBtn, U"EXIT", m_selected == 1);
}
