#include "GameplayMultiPlayerClient.h"
#include "GameManager.h"
#include "EnemyManager.h"
#include "PlatformManager.h"

#include "Socket.h"
#include "NetworkData.h"

GameplayMultiPlayerClient::GameplayMultiPlayerClient()
{

}

GameplayMultiPlayerClient::~GameplayMultiPlayerClient()
{

}

void GameplayMultiPlayerClient::Load(Socket& sock)
{
	GameManager::StaticInitialize();

	EnemyManager::StaticInitialize();

	PlatformManager::StaticInitialize();

	power.Load();

	ui.Load();

	multiPlayerManagerClient.Load();

	background.Load("Forest(Background)", 5);

	player.Load(100.0f, { 10.0f,650.0f });

	EnemyManager::Get()->RetrieveEnemyData("enemy_data.txt");
	EnemyManager::Get()->Load();

	uint32_t totalPlatforms = 5;
	PlatformManager::Get()->Load(totalPlatforms);
	GameManager::Get()->SetTotalPlatforms(totalPlatforms);
}

void GameplayMultiPlayerClient::Unload()
{
	power.Unload();

	ui.Unload();

	player.Unload();

	background.Unload();

	multiPlayerManagerClient.Unload();

	PlatformManager::StaticTerminate();

	EnemyManager::Get()->Unload();

	EnemyManager::StaticTerminate();

	GameManager::StaticTerminate();
}

Transition GameplayMultiPlayerClient::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{
	GameManager::Get()->Update(deltaTime);

	if (GameManager::Get()->GetIsGamePaused() != true)
	{
		CheckCollision();

		background.Update(deltaTime);

		player.Update(deltaTime);

		power.Update(deltaTime);

		EnemyManager::Get()->Update(deltaTime);

		PlatformManager::Get()->Update(deltaTime);
	}


	ui.Update(deltaTime);

	multiPlayerManagerClient.Update(deltaTime, sock, networkData);

	Transition nextState = Transition::None;
	if (GameManager::Get()->GetReturnToMainMenu() == true)
	{
		nextState = Transition::GoToMainMenu;
	}

	return nextState;
}

void GameplayMultiPlayerClient::Render()
{
	background.Render();

	PlatformManager::Get()->Render();

	GameManager::Get()->Render();

	ui.Render();

	player.Render();

	multiPlayerManagerClient.Render();

	EnemyManager::Get()->Render();
}

void GameplayMultiPlayerClient::CheckCollision()
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
