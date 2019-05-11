#ifndef INCLUDED_CONTROLSCHEME_HEADER_H
#define INCLUDED_CONTROLSCHEME_HEADER_H

#include "Button.h"
#include "GameState.h"

class Socket;
struct NetworkData;

class ControlScheme: public GameState
{
public:
	ControlScheme();
	~ControlScheme() override;

	void Load(Socket& sock) override;
	void Unload() override;
	Transition Update(float deltaTime, Socket& sock, NetworkData& networkData) override;
	void Render() override;

private:
	X::TextureId mControlSchemeTexture;

	Button mAcceptButton;
};
#endif // !INCLUDED_CONTROLSCHEME_HEADER_H
