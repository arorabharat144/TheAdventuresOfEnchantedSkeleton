#ifndef INCLUDED_MENU_HEADER_H
#define INCLUDED_MENU_HEADER_H

#include "GameState.h"
#include "Button.h"

class Socket;
struct NetworkData;

class Menu :public GameState
{
public:

	Menu();
	~Menu() override;

	void Load(Socket& sock) override;
	void Unload() override;
	Transition Update(float deltaTime, Socket& sock, NetworkData& networkData) override;
	void Render() override;

private:
	X::TextureId mMenuScreenTexture;

	Button mPlayButton;
	Button mMultiplayerButton;

	Button mOptionsButton;
	Button mExitButton;

	X::TextureId mQuitConfirmationTexture;
	Button mAcceptButton;
	Button mRejectButton;
	bool mIsQuitting;
};

#endif // !INCLUDED_MENU_HEADER_H
