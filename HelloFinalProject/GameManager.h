#ifndef INCLUDED_GAMEMANAGER_HEADER_H
#define INCLUDED_GAMEMANAGER_HEADER_H

#include "Player.h"
#include "Power.h"
#include "Exit.h"

class GameManager
{
public:
	static void StaticInitialize();
	static void StaticTerminate();
	static GameManager* Get();
public:
	GameManager();
	~GameManager();

	void Update(float deltaTime);
	void Render();

	X::Math::Vector2 GetPlayerPosition() const;
	void SetPlayerPosition(const X::Math::Vector2& position);

	PlayerState GetPlayerState() const;
	void SetPlayerState(PlayerState playerState);

	float GetPlayerSpeed() const;
	void SetPlayerSpeed(float playerSpeed);

	float GetPlayerHealth() const;
	void SetPlayerHealth(float playerHealth);

	uint32_t GetBackgroundScrollNumber() const;
	void SetBackgroundScrollNumber(uint32_t backgroundScrollNumber);

	uint32_t GetTotalBackgroundScrollNumber() const;

	float GetPlayerSpeedToReset() const;
	void SetPlayerSpeedToReset(float playerSpeedToReset);

	X::Math::Rect GetGroundCollider() const;
	void SetGroundCollider(const X::Math::Rect& groundCollider);

	float GetBackgroundSpeed() const;
	void SetBackgroundSpeed(float backgroundSpeed);

	bool GetShowColliders() const;

	bool GetIsPlayerDead() const;
	void SetIsPlayerDead(bool status);

	bool GetIsGamePaused() const;
	void SetIsGamePaused(bool status);

	bool GetReturnToMainMenu() const;
	void SetReturnToMainMenu(bool status);

	PowerMode GetPowerMode() const;
	void SetPowerMode(PowerMode powerMode);

	uint32_t GetTotalPlatforms() const;
	void SetTotalPlatforms(uint32_t platforms);

	float GetDistanceToEnd() const;
	void SetDistanceToEnd(float distanceToEnd);

	X::Math::Rect GetExitBoundingBox() const;

	bool GetHasCompletedLevel() const;
	void SetHasCompletedLevel(bool status);

	uint32_t GetNumberOfDeadEnemies() const;
	void SetNumberOfDeadEnemies(uint32_t number);

	bool GetMultiPlayerMode() const;
	void SetMultiPlayerMode(bool status);
private:
	X::Math::Vector2 mPlayerPosition;
	PlayerState mPlayerState;
	float mPlayerSpeed;
	float mPlayerHealth;
	float mPlayerSpeedToReset;

	uint32_t mBackgroundScrollNumber;
	uint32_t mTotalBackgroundScrollNumber;

	X::Math::Rect mGroundCollider;

	float mBackgroundSpeed;

	bool mShowColliders;
	uint32_t mShowCollidersInput;

	bool mIsPlayerDead;

	bool mIsGamePaused;
	uint32_t mIsGamePausedInput;

	bool mReturnToMainMenu;

	PowerMode mPower;

	uint32_t mTotalPlatforms;

	Exit mExit;

	float mDistanceToEnd;

	bool mHasCompletedLevel;

	uint32_t mDeadEnemies;

	bool mIsMultiPlayerMode;
};

#endif // !INCLUDED_GAMEMANAGER_HEADER_H
