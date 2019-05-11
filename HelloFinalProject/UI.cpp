#include "UI.h"
#include "GameManager.h"

UI::UI()
{
}

UI::~UI()
{
}

void UI::Load()
{
	mHealthBarTextures = new X::TextureId[11];
	mCurrentHealthTextureIndex = 10;
	mDistanceToEnd = ((GameManager::Get()->GetTotalBackgroundScrollNumber() + 1.0f) *(float)X::GetScreenWidth()) + 1.0f;
	GameManager::Get()->SetDistanceToEnd(mDistanceToEnd);
	for (uint32_t i = 0; i < 11; ++i)
	{
		char buffer[128];
		sprintf_s(buffer, "GameUI/HealthBar/Health-Bar_%02u.png", i);
		mHealthBarTextures[i] = X::LoadTexture(buffer);
	}

	mPauseWindowTexture = X::LoadTexture("GameUI/Pause_Window.png");

	mResumeButton.texture = X::LoadTexture("GameUI/Resume_Button.png");
	mResumeButton.position = { (float)X::GetScreenWidth()*0.5f - 175.0f,(float)X::GetScreenHeight()*0.5f + 125.0f };

	mExitToMainMenuButtonForPause.texture = X::LoadTexture("GameUI/Exit_To_Main_Menu_Button.png");
	mExitToMainMenuButtonForPause.position = { (float)X::GetScreenWidth()*0.5f + 50.0f,(float)X::GetScreenHeight()*0.5f + 125.0f };

	mPlayerDiedWindowTexture = X::LoadTexture("GameUI/You_Are_Dead_Window.png");
	mExitToMainMenuButtonForPlayerDied.texture = X::LoadTexture("GameUI/Exit_To_Main_Menu_Button.png");
	mExitToMainMenuButtonForPlayerDied.position = { (float)X::GetScreenWidth()*0.5f-75.0f,(float)X::GetScreenHeight()*0.5f + 125.0f };

	mAxePowerTexture = X::LoadTexture("GameUI/Axe_Power.png");
	mPlatformPowerTexture = X::LoadTexture("GameUI/Platform_Power.png");
	mGravityDownPowerTexture = X::LoadTexture("GameUI/Gravity_Down_Power.png");
	mGravityUpPowerTexture = X::LoadTexture("GameUI/Gravity_Up_Power.png");

	mExitLevelTexture = X::LoadTexture("GameUI/You_Survived_The_Level_Window.png");

	mAcceptButton.texture = X::LoadTexture("UI/Accept_Button.png");
	mAcceptButton.position = { (float)X::GetScreenWidth()*0.5f - 50.0f,(float)X::GetScreenHeight()*0.5f + 125.0f };
}

void UI::Unload()
{
	X::SafeDeleteArray(mHealthBarTextures);
}

void UI::Update(float deltaTime)
{
	if (GameManager::Get()->GetIsGamePaused() == false)
	{
		mCurrentHealthTextureIndex = (uint32_t)GameManager::Get()->GetPlayerHealth() / 10;

		if (X::IsKeyDown(X::Keys::D) && GameManager::Get()->GetPlayerPosition().x >= X::GetScreenWidth()*0.5f && GameManager::Get()->GetBackgroundScrollNumber() <= GameManager::Get()->GetTotalBackgroundScrollNumber())
		{
			mDistanceToEnd -= 1.25f;
			if (mDistanceToEnd < 3.0f)
			{
				mDistanceToEnd = 0.0f;
				GameManager::Get()->SetDistanceToEnd(mDistanceToEnd);
			}
		}
		GameManager::Get()->SetDistanceToEnd(mDistanceToEnd);
		sprintf_s(mDistanceToEndString, "Destination: %.02fm", mDistanceToEnd);
		sprintf_s(mPlatformPowerString, "x%u", GameManager::Get()->GetTotalPlatforms());
	}
	else
	{
		int mousePositionX = X::GetMouseScreenX();
		int mousePositionY = X::GetMouseScreenY();

		if (X::IsMousePressed(X::Mouse::LBUTTON))
		{
			if (mResumeButton.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				GameManager::Get()->SetIsGamePaused(false);
			}

			if (mExitToMainMenuButtonForPause.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				GameManager::Get()->SetReturnToMainMenu(true);
				//Might have to set it to false somewhere
			}
		}
	}

	if (GameManager::Get()->GetIsPlayerDead() == true)
	{
		int mousePositionX = X::GetMouseScreenX();
		int mousePositionY = X::GetMouseScreenY();

		if (X::IsMousePressed(X::Mouse::LBUTTON))
		{
			if (mExitToMainMenuButtonForPlayerDied.IsMouseColliding(mousePositionX, mousePositionY) == true)
			{
				GameManager::Get()->SetReturnToMainMenu(true);
			}
		}
	}

	if (GameManager::Get()->GetHasCompletedLevel() == true)
	{
		int mousePositionX = X::GetMouseScreenX();
		int mousePositionY = X::GetMouseScreenY();

		if (X::IsMousePressed(X::Mouse::LBUTTON))
		{
			if (mAcceptButton.IsMouseColliding(mousePositionX, mousePositionY))
			{
				GameManager::Get()->SetReturnToMainMenu(true);
			}
		}
	}
}

void UI::Render()
{
	X::DrawSprite(mHealthBarTextures[mCurrentHealthTextureIndex], { 25.0f,25.0f });

	if (GameManager::Get()->GetPowerMode() == PowerMode::AXE)
	{
		X::DrawSprite(mAxePowerTexture, { 25.0f,75.0f });
	}
	else if (GameManager::Get()->GetPowerMode() == PowerMode::PLATFORM)
	{
		X::DrawSprite(mPlatformPowerTexture, { 25.0f,75.0f });
		X::DrawScreenText(mPlatformPowerString, X::GetSpriteWidth(mPlatformPowerTexture) + 50.0f, 75.0f, 25.0f, X::Math::Vector4::Red());
	}
	else if (GameManager::Get()->GetPowerMode() == PowerMode::GRAVITYDOWN)
	{
		X::DrawSprite(mGravityDownPowerTexture, { 25.0f,75.0f });
	}
	else if (GameManager::Get()->GetPowerMode() == PowerMode::GRAVITYUP)
	{
		X::DrawSprite(mGravityUpPowerTexture, { 25.0f,75.0f });
	}

	//FIX IT
	X::DrawScreenText(mDistanceToEndString, X::GetScreenWidth() - 310.0f, 25.0f, 25.0f, X::Math::Vector4::Cyan());
	if (GameManager::Get()->GetShowColliders() == true)
	{
		X::DrawScreenText("Show Bounding Boxes", X::GetScreenWidth()*0.5f - 125.0f, 75.0f, 25.0f, X::Math::Vector4::Red());
	}

	if (GameManager::Get()->GetIsGamePaused() == true)
	{
		X::DrawSprite(mPauseWindowTexture, { (float)X::GetScreenWidth()*0.5f - 225.0f, (float)X::GetScreenHeight()*0.5f });
		X::DrawSprite(mResumeButton.texture, mResumeButton.position);
		X::DrawSprite(mExitToMainMenuButtonForPause.texture, mExitToMainMenuButtonForPause.position);
	}

	if (GameManager::Get()->GetIsPlayerDead() == true)
	{
		X::DrawSprite(mPlayerDiedWindowTexture, { (float)X::GetScreenWidth()*0.5f - 225.0f, (float)X::GetScreenHeight()*0.5f });
		X::DrawSprite(mExitToMainMenuButtonForPlayerDied.texture, mExitToMainMenuButtonForPlayerDied.position);
	}

	if (GameManager::Get()->GetHasCompletedLevel() == true && !GameManager::Get()->GetMultiPlayerMode())
	{
		X::DrawSprite(mExitLevelTexture, { (float)X::GetScreenWidth()*0.5f - 225.0f, (float)X::GetScreenHeight()*0.5f });
	}

	if (GameManager::Get()->GetHasCompletedLevel() == true)
	{
		X::DrawSprite(mAcceptButton.texture, mAcceptButton.position);
	}
}
