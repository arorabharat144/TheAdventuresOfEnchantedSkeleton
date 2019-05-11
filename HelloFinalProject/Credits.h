#ifndef INCLUDED_CREDITS_HEADER_H
#define INCLUDED_CREDITS_HEADER_H

#include "GameState.h"
#include "Button.h"
#include <XEngine.h>

class Socket;
struct NetworkData;

class Credits:public GameState
{
public:
	Credits();
	~Credits() override;

	void Load(Socket& sock) override;
	void Unload() override;
	Transition Update(float deltaTime, Socket& sock, NetworkData& networkData) override;
	void Render() override;
private:
	X::TextureId mCreditsScreenTexture;
	Button mGoBackButton;
	
};

#endif // !INCLUDED_CREDITS_HEADER_H
