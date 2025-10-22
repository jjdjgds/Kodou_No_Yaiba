# include "Game.hpp"



Game::Game(const InitData& init)
	: IScene{ init }
	
{
	
}

void Game::update()
{


}

void Game::draw() const
{

	Scene::SetBackground(ColorF(1.0,0.2,0.2,1.0));

	// テクスチャアセットを使用する
	//
	TextureAsset(U"Windmill").draw(0,0);
}


