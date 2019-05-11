#include "GameManager.h"

namespace
{
	GameManager* sGameManager = nullptr;
}

void GameManager::StaticInitialize()
{
	sGameManager = new GameManager();
}

void GameManager::StaticTerminate()
{
	X::SafeDelete(sGameManager);


}

GameManager* GameManager::Get()
{
	XASSERT(sGameManager != nullptr, "[GameManager] Must call StaticInitialize() first.");
	return sGameManager;
}

GameManager::GameManager()
{
	mShowColliders = false;
	mShowCollidersInput = 0;
	mIsPlayerDead = false;
	mPlayerHealth = 100.0f;
	mTotalBackgroundScrollNumber = 10;
	mIsGamePaused = false;
	mIsGamePausedInput = 0;
	mPower = PowerMode::AXE;

	mExit.texture = X::LoadTexture("ExitCondition/Exit.png");
	mExit.active = false;
	mHasCompletedLevel = false;

	mDeadEnemies = 0;
}

GameManager::~GameManager()
{

}

void GameManager::Update(float deltaTime)
{
	if (X::IsKeyPressed(X::Keys::F1))
	{
		mShowCollidersInput++;
		if (mShowCollidersInput > 1)
		{
			mShowCollidersInput = 0;
			mShowColliders = false;
		}
	}

	if (mShowCollidersInput == 1)
	{
		mShowColliders = true;
	}

	if (X::IsKeyPressed(X::Keys::ESCAPE) && mIsPlayerDead != true)
	{
		mIsGamePausedInput++;
		if (mIsGamePausedInput > 1)
		{
			mIsGamePausedInput = 0;
			mIsGamePaused = false;
		}
	}

	if (mIsGamePausedInput == 1)
	{
		mIsGamePaused = true;
		mIsGamePausedInput = 0;
	}

	if (mDistanceToEnd == 0.0f)
	{
		mExit.active = true;
		mExit.position = { (float)X::GetScreenWidth(),(float)X::GetScreenHeight() - 175.0f };
	}

	if (mExit.active == true)
	{
		mExit.position.x -= mPlayerSpeedToReset;
	}

	mExit.collider = { mExit.position.x, mExit.position.y, mExit.position.x + (float)X::GetSpriteWidth(mExit.texture), mExit.position.y + (float)X::GetSpriteHeight(mExit.texture) };

}

void GameManager::Render()
{
	if (mExit.active == true)
	{
		X::DrawSprite(mExit.texture, mExit.position);
	}

	if (mShowColliders == true && mExit.active == true)
	{
		X::DrawScreenRect(mExit.collider, X::Math::Vector4::Red());
	}
}

X::Math::Vector2 GameManager::GetPlayerPosition() const
{
	return mPlayerPosition;
}

void GameManager::SetPlayerPosition(const X::Math::Vector2& position)
{
	mPlayerPosition = position;
}

PlayerState GameManager::GetPlayerState() const
{
	return mPlayerState;
}

void GameManager::SetPlayerState(PlayerState playerState)
{
	mPlayerState = playerState;
}

float GameManager::GetPlayerSpeed() const
{
	return mPlayerSpeed;
}

void GameManager::SetPlayerSpeed(float playerSpeed)
{
	mPlayerSpeed = playerSpeed;
}

float GameManager::GetPlayerHealth() const
{
	return mPlayerHealth;
}
void GameManager::SetPlayerHealth(float playerHealth)
{
	mPlayerHealth = playerHealth;
}

uint32_t GameManager::GetBackgroundScrollNumber() const
{
	return mBackgroundScrollNumber;
}

void GameManager::SetBackgroundScrollNumber(uint32_t backgroundScrollNumber)
{
	mBackgroundScrollNumber = backgroundScrollNumber;
}

uint32_t GameManager::GetTotalBackgroundScrollNumber() const
{
	return mTotalBackgroundScrollNumber;
}

float GameManager::GetPlayerSpeedToReset() const
{
	return mPlayerSpeedToReset;
}

void GameManager::SetPlayerSpeedToReset(float playerSpeedToReset)
{
	mPlayerSpeedToReset = playerSpeedToReset;
}

X::Math::Rect GameManager::GetGroundCollider() const
{
	return mGroundCollider;
}

void GameManager::SetGroundCollider(const X::Math::Rect& groundCollider)
{
	mGroundCollider = groundCollider;
}

float GameManager::GetBackgroundSpeed() const
{
	return mBackgroundSpeed;
}

void GameManager::SetBackgroundSpeed(float backgroundSpeed)
{
	mBackgroundSpeed = backgroundSpeed;
}

bool GameManager::GetShowColliders() const
{
	return mShowColliders;
}

bool GameManager::GetIsPlayerDead() const
{
	return mIsPlayerDead;
}
void GameManager::SetIsPlayerDead(bool status)
{
	mIsPlayerDead = status;
}

bool GameManager::GetIsGamePaused() const
{
	return mIsGamePaused;
}

void GameManager::SetIsGamePaused(bool status)
{
	mIsGamePaused = status;
}

bool GameManager::GetReturnToMainMenu() const
{
	return mReturnToMainMenu;
}

void GameManager::SetReturnToMainMenu(bool status)
{
	mReturnToMainMenu = status;
}

PowerMode GameManager::GetPowerMode() const
{
	return mPower;
}

void GameManager::SetPowerMode(PowerMode powerMode)
{
	mPower = powerMode;
}

uint32_t GameManager::GetTotalPlatforms() const
{
	return mTotalPlatforms;
}
void GameManager::SetTotalPlatforms(uint32_t platforms)
{
	mTotalPlatforms = platforms;
}

float GameManager::GetDistanceToEnd() const
{
	return mDistanceToEnd;
}
void GameManager::SetDistanceToEnd(float distanceToEnd)
{
	mDistanceToEnd = distanceToEnd;
}

X::Math::Rect GameManager::GetExitBoundingBox() const
{
	return mExit.collider;
}

bool GameManager::GetHasCompletedLevel() const
{
	return mHasCompletedLevel;
}
void GameManager::SetHasCompletedLevel(bool status)
{
	mHasCompletedLevel = status;
}

uint32_t GameManager::GetNumberOfDeadEnemies() const
{
	return mDeadEnemies;
}

void GameManager::SetNumberOfDeadEnemies(uint32_t number)
{
	mDeadEnemies = number;
}

bool GameManager::GetMultiPlayerMode() const
{
	return mIsMultiPlayerMode;
}

void GameManager::SetMultiPlayerMode(bool status) 
{
	mIsMultiPlayerMode = status;
}