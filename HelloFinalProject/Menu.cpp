#include "Menu.h"

Menu::Menu()
{

}

Menu::~Menu()
{

}

void Menu::Load(Socket& sock)
{
	mMenuScreenTexture = X::LoadTexture("UI/Menu.png");

	mPlayButton.texture = X::LoadTexture("UI/Play_Button.png");
	mPlayButton.position = { (float)X::GetScreenWidth()*0.5f - 25.0f, (float)X::GetScreenHeight()*0.5f };

	mMultiplayerButton.texture = X::LoadTexture("UI/Multiplayer_Button.png");
	mMultiplayerButton.position = { (float)X::GetScreenWidth()*0.5f + 100.0f, (float)X::GetScreenHeight()*0.5f + X::GetSpriteHeight(mMultiplayerButton.texture) - 25.0f };

	mOptionsButton.texture = X::LoadTexture("UI/Options_Button.png");
	mOptionsButton.position = { (float)X::GetScreenWidth()*0.5f - 25.0f , (float)X::GetScreenHeight()*0.5f + X::GetSpriteHeight(mPlayButton.texture) + 25.0f };

	mExitButton.texture = X::LoadTexture("UI/Exit_Button.png");
	mExitButton.position = { 25.0f,25.0f };

	mQuitConfirmationTexture = X::LoadTexture("UI/Quit_Confirmation.png");

	mAcceptButton.texture = X::LoadTexture("UI/Accept_Button.png");
	mAcceptButton.position = { (float)X::GetScreenWidth()*0.5f - 175.0f,(float)X::GetScreenHeight()*0.5f + 125.0f };

	mRejectButton.texture = X::LoadTexture("UI/Reject_Button.png");
	mRejectButton.position = { (float)X::GetScreenWidth()*0.5f + 100.0f,(float)X::GetScreenHeight()*0.5f + 125.0f };

	mIsQuitting = false;
}

void Menu::Unload()
{

}

Transition Menu::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{
	Transition nextState = Transition::None;
	int mousePositionX = X::GetMouseScreenX();
	int mousePositionY = X::GetMouseScreenY();
	if (mIsQuitting == false)
	{
		if (X::IsMousePressed(X::Mouse::LBUTTON))
		{
			if (mPlayButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				nextState = Transition::GoToControlScheme;
			}
			if (mOptionsButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				nextState = Transition::GoToCredits;
			}

			if (mMultiplayerButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				nextState = Transition::GoToMultiplayerSelector;
			}

			if (mExitButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				mIsQuitting = true;
			}
		}
	}
	else
	{
		if (X::IsMousePressed(X::Mouse::LBUTTON))
		{
			if (mAcceptButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				nextState = Transition::QuitGame;
			}

			if (mRejectButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				mIsQuitting = false;
			}

		}
	}
	return nextState;
}

void Menu::Render()
{
	X::DrawSprite(mMenuScreenTexture, { 0.0f,0.0f });
	X::DrawSprite(mPlayButton.texture, mPlayButton.position);
	X::DrawSprite(mOptionsButton.texture, mOptionsButton.position);
	X::DrawSprite(mExitButton.texture, mExitButton.position);
	X::DrawSprite(mMultiplayerButton.texture, mMultiplayerButton.position);

	if (mIsQuitting == true)
	{
		X::DrawSprite(mQuitConfirmationTexture, { (float)X::GetScreenWidth()*0.5f-225.0f,(float)X::GetScreenHeight()*0.5f});
		X::DrawSprite(mAcceptButton.texture, mAcceptButton.position);
		X::DrawSprite(mRejectButton.texture, mRejectButton.position);
	}
}