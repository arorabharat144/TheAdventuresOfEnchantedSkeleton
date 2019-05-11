#ifndef INCLUDED_GAMESTATE_HEADER_H
#define INCLUDED_GAMESTATE_HEADER_H

class Socket;
struct NetworkData;

enum class Transition
{
	None,
	GoToMainMenu,
	GoToControlScheme,
	GoToMultiplayerSelector,
	GoToMultiplayerCreateSession,
	GoToMultiplayerJoinSession,
	GoToCredits,
	GoToGameplaySinglePlayer,
	GoToGameplayMultiPlayerHost,
	GoToGameplayMultiPlayerClient,
	QuitGame
};

class GameState
{
public:

	virtual ~GameState() {}

	virtual void Load(Socket& sock) = 0;
	virtual void Unload() = 0;
	virtual Transition Update(float deltaTime, Socket& sock, NetworkData& networkData) = 0;
	virtual void Render() = 0;
};

#endif // !INCLUDED_GAMESTATE_HEADER_H
