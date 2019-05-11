#include "MultiPlayerManagerHost.h"

#include "GameManager.h"

#include "Socket.h"
#include "NetworkData.h"

namespace
{
	bool hasSetInitialData = false;
	float totalDistance = 0.0f;
	float distanceFactorToMove = 0.0f;

	float elapsedDistance = 0.0f;

	std::string clientIP;

	char hostEnemiesBuffer[128];
	char clientEnemiesBuffer[128];
}

MultiPlayerManagerHost::MultiPlayerManagerHost()
{

}

MultiPlayerManagerHost::~MultiPlayerManagerHost()
{

}

void MultiPlayerManagerHost::Load()
{
	hostSkullFace = X::LoadTexture("GameUI/Host_Skull_Face.png");
	hostSkullFacePosition = { 0.0f, 135.0f };

	clientSkullFace = X::LoadTexture("GameUI/Client_Skull_Face.png");
	clientSkullFacePosition = { 0.0f, 175.0f };

	progressBar = X::LoadTexture("GameUI/Progress_Bar.png");

	totalDistance = GameManager::Get()->GetDistanceToEnd();
	distanceFactorToMove = (float)X::GetSpriteWidth(progressBar) / GameManager::Get()->GetDistanceToEnd();

	gameplayPacketToReceive.skullFacePosX = 0.0f;

	hostEnemiesKilled = 0;
	clientEnemiesKilled = 0;

	hostHasReachedEnd = false;
	clientHasReachedEnd = false;

	hostHasDied = false;
	clientHasDied = false;

	youWonTexture = X::LoadTexture("GameUI/You_Won_Window.png");
	youLostTexture = X::LoadTexture("GameUI/You_Lost_Window.png");

	GameManager::Get()->SetMultiPlayerMode(true);
}

void MultiPlayerManagerHost::Unload()
{
	ZeroMemory(&gameplayPacketToSend, sizeof(GameplayPacket));
	ZeroMemory(&gameplayPacketToReceive, sizeof(GameplayPacket));
}

void MultiPlayerManagerHost::Update(float deltaTime, Socket& sock, NetworkData& networkData)
{

	memset(bufferToSend, 0, sizeof(bufferToSend));
	memset(bufferToReceive, 0, sizeof(bufferToReceive));

	//RECEIVING DATA FROM CLIENT PLAYER

	int bytesReceived = sock.Receive(&bufferToReceive, sizeof(bufferToReceive), clientIP);
	if (bytesReceived > 0)
	{
		memcpy(&gameplayPacketToReceive, bufferToReceive, sizeof(GameplayPacket));
		clientSkullFacePosition.x = gameplayPacketToReceive.skullFacePosX;
		clientEnemiesKilled = gameplayPacketToReceive.enemiesKilled;
		clientHasReachedEnd = gameplayPacketToReceive.hasReachedEnd;
		clientHasDied = gameplayPacketToReceive.hasDied;

		if (gameplayPacketToReceive.skullFacePosX > X::GetSpriteWidth(progressBar) * 2.0f)
		{
			clientSkullFacePosition.x = 0.0f;
		}
	}

	elapsedDistance = totalDistance - GameManager::Get()->GetDistanceToEnd();

	if (!hasSetInitialData)
	{
		hostName = networkData.hostName;
		clientName = networkData.clientName;

		clientAddress.address = networkData.clientIP;
		clientAddress.port = port;

		hasSetInitialData = true;
	}

	if (elapsedDistance > 0.0f)
	{
		hostSkullFacePosition.x += elapsedDistance * distanceFactorToMove; //ME
		totalDistance = GameManager::Get()->GetDistanceToEnd();
	}

	hostEnemiesKilled = GameManager::Get()->GetNumberOfDeadEnemies();
	if (hostEnemiesKilled < 0)
	{
		hostEnemiesKilled = 0;
	}
	sprintf_s(hostEnemiesBuffer, "Enemies Killed:%d", hostEnemiesKilled);

	hostHasReachedEnd = GameManager::Get()->GetHasCompletedLevel();
	hostHasDied = GameManager::Get()->GetIsPlayerDead();

	//SENDING DATA TO CLIENT PLAYER
	gameplayPacketToSend.skullFacePosX = hostSkullFacePosition.x;
	gameplayPacketToSend.enemiesKilled = hostEnemiesKilled;
	gameplayPacketToSend.hasReachedEnd = hostHasReachedEnd;
	gameplayPacketToSend.hasDied = hostHasDied;

	memcpy(&bufferToSend, &gameplayPacketToSend, sizeof(GameplayPacket));
	sock.Send(clientAddress, bufferToSend, sizeof(bufferToSend));

	//RECEIVING DATA FROM CLIENT PLAYER
	bytesReceived = sock.Receive(&bufferToReceive, sizeof(bufferToReceive), clientIP);
	if (bytesReceived > 0)
	{
		memcpy(&gameplayPacketToReceive, bufferToReceive, sizeof(GameplayPacket));
		clientSkullFacePosition.x = gameplayPacketToReceive.skullFacePosX;
		clientEnemiesKilled = gameplayPacketToReceive.enemiesKilled;
		clientHasReachedEnd = gameplayPacketToReceive.hasReachedEnd;
		clientHasDied = gameplayPacketToReceive.hasDied;

		if (clientEnemiesKilled < 0)
		{
			clientEnemiesKilled = 0;
		}


		if (gameplayPacketToReceive.skullFacePosX > X::GetSpriteWidth(progressBar) * 2.0f)
		{
			clientSkullFacePosition.x = 0.0f;
		}
	}


	sprintf_s(clientEnemiesBuffer, "Enemies Killed:%d", clientEnemiesKilled);
}

void MultiPlayerManagerHost::Render()
{
	X::DrawSprite(progressBar, { 25.0f, 135.0f });
	X::DrawSprite(hostSkullFace, hostSkullFacePosition);
	X::DrawSprite(progressBar, { 25.0f, 175.0f });
	X::DrawSprite(clientSkullFace, clientSkullFacePosition);

	X::DrawScreenText(hostName.c_str(), X::GetScreenWidth()*0.5f - X::GetSpriteWidth(hostSkullFace) - 200.0f, X::GetScreenHeight()*0.10f, 25.0f, X::Math::Vector4::Red());
	X::DrawScreenText("VS", X::GetScreenWidth()*0.5f - X::GetSpriteWidth(hostSkullFace), X::GetScreenHeight()*0.10f, 25.0f, X::Math::Vector4::Green());
	X::DrawScreenText(clientName.c_str(), X::GetScreenWidth()*0.5f - X::GetSpriteWidth(hostSkullFace) + 200.0f, X::GetScreenHeight()*0.10f, 25.0f, X::Math::Vector4::Blue());

	X::DrawScreenText(hostEnemiesBuffer, 35.0f, X::GetScreenHeight() - 35.0f, 25.0f, X::Math::Vector4::Red());
	X::DrawScreenText(clientEnemiesBuffer, X::GetScreenWidth() - 250.0f, X::GetScreenHeight() - 35.0f, 25.0f, X::Math::Vector4::Blue());

	if (hostHasReachedEnd || clientHasDied)
	{
		X::DrawSprite(youWonTexture, { (float)X::GetScreenWidth()*0.5f - 225.0f, (float)X::GetScreenHeight()*0.5f });
	}
	else if (clientHasReachedEnd || hostHasDied)
	{
		X::DrawSprite(youLostTexture, { (float)X::GetScreenWidth()*0.5f - 225.0f, (float)X::GetScreenHeight()*0.5f });
	}
}
