#include "ControlScheme.h"

ControlScheme::ControlScheme()
{

}

ControlScheme::~ControlScheme()
{

}

void ControlScheme::Load(Socket& sock)
{
	mControlSchemeTexture = X::LoadTexture("UI/ControlScheme.png");

	mAcceptButton.texture = X::LoadTexture("UI/Accept_Button.png");
	mAcceptButton.position = { (float)X::GetScreenWidth()*0.5f - 25.0f, (float)X::GetScreenHeight() - 100.0f };
}

void ControlScheme::Unload()
{

}

Transition ControlScheme::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{
	Transition nextState = Transition::None;
	int mousePositionX = X::GetMouseScreenX();
	int mousePositionY = X::GetMouseScreenY();

	if (X::IsMousePressed(X::Mouse::LBUTTON))
	{
		if (mAcceptButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
		{
			nextState = Transition::GoToGameplaySinglePlayer;
		}
	}

	return nextState;
}

void ControlScheme::Render()
{
	X::DrawSprite(mControlSchemeTexture, { 0.0f,0.0f });
	X::DrawSprite(mAcceptButton.texture, mAcceptButton.position);
}
