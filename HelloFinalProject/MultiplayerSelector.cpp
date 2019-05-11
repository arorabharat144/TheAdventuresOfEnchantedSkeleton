#include "MultiplayerSelector.h"

MultiplayerSelector::MultiplayerSelector()
{

}

MultiplayerSelector::~MultiplayerSelector()
{

}

void MultiplayerSelector::Load(Socket& sock)
{
	mMenuScreenTexture = X::LoadTexture("UI/Menu.png");

	mCreateSessionButton.texture = X::LoadTexture("UI/Create_Session_Button.png");
	mCreateSessionButton.position = { (float)X::GetScreenWidth()*0.5f - 25.0f, (float)X::GetScreenHeight()*0.5f };

	mJoinSessionButton.texture = X::LoadTexture("UI/Join_Session_Button.png");
	mJoinSessionButton.position = { (float)X::GetScreenWidth()*0.5f - 25.0f , (float)X::GetScreenHeight()*0.5f + X::GetSpriteHeight(mJoinSessionButton.texture) + 25.0f };
}

void MultiplayerSelector::Unload()
{

}

Transition MultiplayerSelector::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{
	Transition nextState = Transition::None;
	int mousePositionX = X::GetMouseScreenX();
	int mousePositionY = X::GetMouseScreenY();

	if (X::IsMousePressed(X::Mouse::LBUTTON))
	{
		if (mCreateSessionButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
		{
			nextState = Transition::GoToMultiplayerCreateSession;
		}
		if (mJoinSessionButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
		{
			nextState = Transition::GoToMultiplayerJoinSession;
		}
	}

	return nextState;
}

void MultiplayerSelector::Render()
{
	X::DrawSprite(mMenuScreenTexture, { 0.0f, 0.0f });
	X::DrawSprite(mCreateSessionButton.texture, mCreateSessionButton.position);
	X::DrawSprite(mJoinSessionButton.texture, mJoinSessionButton.position);
}