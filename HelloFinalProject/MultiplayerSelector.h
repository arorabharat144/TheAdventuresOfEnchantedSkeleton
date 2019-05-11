#ifndef INCLUDED_MULTIPLAYER_HEADER_H
#define INCLUDED_MULTIPLAYER_HEADER_H

#include "GameState.h"
#include "Button.h"

class Socket;
struct NetworkData;

class MultiplayerSelector : public GameState
{
public:
	MultiplayerSelector();
	~MultiplayerSelector() override;

	void Load(Socket& sock) override;
	void Unload() override;
	Transition Update(float deltaTime, Socket& sock, NetworkData& networkData) override;
	void Render() override;

private:
	X::TextureId mMenuScreenTexture;

	Button mCreateSessionButton;
	Button mJoinSessionButton;
};

#endif // !INCLUDED_MULTIPLAYER_HEADER_H
