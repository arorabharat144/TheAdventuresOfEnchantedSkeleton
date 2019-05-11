#include "Menu.h"
#include "GameplaySinglePlayer.h"
#include "GameplayMultiPlayerHost.h"
#include "GameplayMultiPlayerClient.h"
#include "ControlScheme.h"
#include "Credits.h"
#include "MultiplayerSelector.h"
#include "MultiplayerSessionCreator.h"
#include "MultiplayerSessionJoiner.h"

#include "Socket.h"
#include "Packet.h"
#include "NetworkData.h"

namespace
{
	GameState* currentState = nullptr;
	Transition nextState = Transition::None;

	//NETWORKING STUFF
	Socket sock;
	InitPacket serverInitPacket;
	InitPacket clientInitPacket;

	NetworkData networkData;

	bool GameLoop(float deltaTime)
	{
		nextState = currentState->Update(deltaTime, sock, networkData);

		if (nextState == Transition::None)
		{
			currentState->Render();
		}
		else if (nextState == Transition::GoToMainMenu)
		{
			currentState->Unload();

			delete currentState;
			currentState = new Menu();
			currentState->Load(sock);
		}
		else if (nextState == Transition::GoToControlScheme)
		{
			currentState->Unload();

			delete currentState;
			currentState = new ControlScheme();
			currentState->Load(sock);
		}
		else if (nextState == Transition::GoToCredits)
		{
			currentState->Unload();

			delete currentState;
			currentState = new Credits();
			currentState->Load(sock);
		}
		else if (nextState == Transition::GoToGameplaySinglePlayer)
		{
			currentState->Unload();

			delete currentState;
			currentState = new GameplaySinglePlayer(); //CHANGE IT BACK TO SINGLEPLAYER
			currentState->Load(sock);
		}
		else if (nextState == Transition::GoToGameplayMultiPlayerHost)
		{
			currentState->Unload();

			delete currentState;
			currentState = new GameplayMultiPlayerHost();
			currentState->Load(sock);
		}
		else if (nextState == Transition::GoToGameplayMultiPlayerClient)
		{
			currentState->Unload();

			delete currentState;
			currentState = new GameplayMultiPlayerClient();
			currentState->Load(sock);
		}
		else if (nextState == Transition::GoToMultiplayerSelector)
		{
			currentState->Unload();

			delete currentState;
			currentState = new MultiplayerSelector();
			currentState->Load(sock);
		}
		else if (nextState == Transition::GoToMultiplayerCreateSession)
		{
			currentState->Unload();

			delete currentState;
			currentState = new MultiplayerSessionCreator();
			currentState->Load(sock);
		}
		else if (nextState == Transition::GoToMultiplayerJoinSession)
		{
			currentState->Unload();

			delete currentState;
			currentState = new MultiplayerSessionJoiner();
			currentState->Load(sock);
		}

		return nextState == Transition::QuitGame;
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	X::Start("xconfig.json");

	currentState = new Menu();
	currentState->Load(sock);

	X::Run(GameLoop);

	sock.Close();

	currentState->Unload();

	X::SafeDelete(currentState);

	

	X::Stop();

	return 0;
}