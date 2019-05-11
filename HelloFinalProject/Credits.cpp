#include "Credits.h"

Credits::Credits()
{

}

Credits::~Credits()
{

}

void Credits::Load(Socket& sock)
{
	mCreditsScreenTexture = X::LoadTexture("UI/Credits.png");
	mGoBackButton.texture = X::LoadTexture("UI/Go_Back.png");

	mGoBackButton.position = { 25.0f,25.0f };
}

void Credits::Unload()
{

}

Transition Credits::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{
	Transition nextState = Transition::None;
	if (X::IsMousePressed(X::Mouse::LBUTTON))
	{
		int mousePositionX = X::GetMouseScreenX();
		int mousePositionY = X::GetMouseScreenY();

		if (mGoBackButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
		{
			nextState = Transition::GoToMainMenu;
		}
	}
	return nextState;
}

void Credits::Render()
{
	X::DrawSprite(mCreditsScreenTexture, { 0.0f,0.0f });
	X::DrawSprite(mGoBackButton.texture, mGoBackButton.position);
}
