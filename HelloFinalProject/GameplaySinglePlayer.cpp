#include "GameplaySinglePlayer.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "PlatformManager.h"

GameplaySinglePlayer::GameplaySinglePlayer()
{

}

GameplaySinglePlayer::~GameplaySinglePlayer()
{

}

void GameplaySinglePlayer::Load(Socket& sock)
{
	GameManager::StaticInitialize();

	EnemyManager::StaticInitialize();

	PlatformManager::StaticInitialize();

	power.Load();

	ui.Load();

	background.Load("Forest(Background)", 5);

	player.Load(100.0f, { 10.0f,650.0f });

	EnemyManager::Get()->RetrieveEnemyData("enemy_data.txt");
	EnemyManager::Get()->Load();

	uint32_t totalPlatforms = 5;
	PlatformManager::Get()->Load(totalPlatforms);
	GameManager::Get()->SetTotalPlatforms(totalPlatforms);
}

void GameplaySinglePlayer::Unload()
{
	power.Unload();

	ui.Unload();

	player.Unload();

	background.Unload();

	PlatformManager::StaticTerminate();

	EnemyManager::Get()->Unload();

	EnemyManager::StaticTerminate();

	GameManager::StaticTerminate();
}

Transition GameplaySinglePlayer::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{
	GameManager::Get()->Update(deltaTime);

	if (GameManager::Get()->GetIsGamePaused() != true && GameManager::Get()->GetHasCompletedLevel() != true)
	{
		CheckCollision();

		background.Update(deltaTime);

		player.Update(deltaTime);

		power.Update(deltaTime);

		EnemyManager::Get()->Update(deltaTime);

		PlatformManager::Get()->Update(deltaTime);
	}

	ui.Update(deltaTime);
	Transition nextState = Transition::None;
	if (GameManager::Get()->GetReturnToMainMenu() == true)
	{
		nextState = Transition::GoToMainMenu;
	}
	return nextState;
}

void GameplaySinglePlayer::Render()
{
	background.Render();

	PlatformManager::Get()->Render();

	GameManager::Get()->Render();

	ui.Render();

	player.Render();

	EnemyManager::Get()->Render();
}

void GameplaySinglePlayer::CheckCollision()
{
	for (uint32_t i = 0; i < EnemyManager::Get()->GetMaxEnemies(); ++i)
	{
		if (EnemyManager::Get()->IsEnemyActive(i))
		{
			if (X::Math::Intersect(player.GetPlayerAxeBox(), EnemyManager::Get()->GetEnemyBodyBox(i)))
			{
				EnemyManager::Get()->TakeDamage(i, 25.0f);
			}

			if (X::Math::Intersect(player.GetPlayerBodyBox(), EnemyManager::Get()->GetEnemyBodyBox(i)))
			{
				player.TakeDamage();
			}
		}
	}

	for (uint32_t i = 0; i < PlatformManager::Get()->GetMaxPlatforms(); ++i)
	{
		for (uint32_t j = 0; j < EnemyManager::Get()->GetMaxEnemies(); ++j)
		{
			if (PlatformManager::Get()->IsPlatformActive(i) == true && EnemyManager::EnemyManager::Get()->IsEnemyActive(j) == true)
			{
				if (X::Math::Intersect(PlatformManager::Get()->GetBoundingBox(i), EnemyManager::Get()->GetEnemyBodyBox(j)))
				{
					EnemyManager::Get()->TakeDamage(j, 100.0f);
				}
			}
		}
	}

	if (X::Math::Intersect(player.GetPlayerBodyBox(), GameManager::Get()->GetExitBoundingBox()))
	{
		GameManager::Get()->SetHasCompletedLevel(true);
	}
}
