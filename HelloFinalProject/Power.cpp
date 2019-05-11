#include "Power.h"
#include "GameManager.h"

Power::Power()
{

}

Power::~Power()
{

}

void Power::Load()
{
	mPowerMode = PowerMode::AXE;
	mGravityInput = 0;
}

void Power::Unload()
{

}

void Power::Update(float deltaTime)
{
	if (GameManager::Get()->GetIsGamePaused() == false && GameManager::Get()->GetIsPlayerDead() == false)
	{
		if (X::IsKeyPressed(X::Keys::ONE))
		{
			mPowerMode = PowerMode::AXE;

			mGravityInput = 0;
		}
		if (X::IsKeyPressed(X::Keys::TWO))
		{
			mPowerMode = PowerMode::PLATFORM;
			mGravityInput = 0;
		}

		if (X::IsKeyPressed(X::Keys::THREE))
		{
			mGravityInput++;
			if (mGravityInput == 3)
			{
				mGravityInput = 1;
			}
		}
		if (mGravityInput == 1)
		{
			mPowerMode = PowerMode::GRAVITYDOWN;
		}
		if (mGravityInput == 2)
		{
			mPowerMode = PowerMode::GRAVITYUP;
		}
	}

	GameManager::Get()->SetPowerMode(mPowerMode);
}

void Power::Render()
{

}
