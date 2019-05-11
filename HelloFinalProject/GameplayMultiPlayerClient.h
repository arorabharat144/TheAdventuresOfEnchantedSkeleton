#ifndef INCLUDED_GAMEPLAYMULTIPLAYERCLIENT_HEADER_H
#define INCLUDED_GAMEPLAYMULTIPLAYERCLIENT_HEADER_H

#include "GameState.h"
#include "Player.h"
#include "Background.h"
#include "Power.h"
#include "UI.h"

#include "MultiPlayerManagerClient.h"

class Socket;
struct NetworkData;

class GameplayMultiPlayerClient :public GameState
{
public:
	GameplayMultiPlayerClient();
	~GameplayMultiPlayerClient() override;

	void Load(Socket& sock) override;
	void Unload() override;
	Transition Update(float deltaTime, Socket& sock, NetworkData& networkData) override;
	void Render() override;

	void CheckCollision();

private:
	Player player;
	Background background;
	Power power;
	UI ui;

	MultiPlayerManagerClient multiPlayerManagerClient;
};

#endif // !INCLUDED_GAMEPLAYMULTIPLAYERCLIENT_HEADER_H
