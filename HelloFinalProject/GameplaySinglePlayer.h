#ifndef INCLUDED_GAMEPLAYSINGLEPLAYER_HEADER_H
#define INCLUDED_GAMEPLAYSINGLEPLAYER_HEADER_H

#include "GameState.h"
#include "Player.h"
#include "Background.h"
#include "Power.h"
#include "UI.h"

class Socket;
struct NetworkData;

class GameplaySinglePlayer :public GameState
{
public:
	GameplaySinglePlayer();
	~GameplaySinglePlayer() override;

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
};

#endif // !INCLUDED_GAMEPLAYSINGLEPLAYER_HEADER_H
