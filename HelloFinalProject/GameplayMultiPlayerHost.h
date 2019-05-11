#ifndef INCLUDED_GAMEPLAYMULTIPLAYERHOST_HEADER_H
#define INCLUDED_GAMEPLAYMULTIPLAYERHOST_HEADER_H

#include "GameState.h"
#include "Player.h"
#include "Background.h"
#include "Power.h"
#include "UI.h"

#include "MultiPlayerManagerHost.h"

class Socket;
struct NetworkData;

class GameplayMultiPlayerHost :public GameState
{
public:
	GameplayMultiPlayerHost();
	~GameplayMultiPlayerHost() override;

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

	MultiPlayerManagerHost multiPlayerManagerHost;
};

#endif // !INCLUDED_GAMEPLAYMULTIPLAYERHOST_HEADER_H
