#ifndef INCLUDED_MULTIPLAYER_SESSIONCREATOR_HEADER_H
#define INCLUDED_MULTIPLAYER_SESSIONCREATOR_HEADER_H

#include "GameState.h"
#include "Button.h"

#include "NetworkAddress.h"
#include "Packet.h"

class Socket;
struct NetworkData;

class MultiplayerSessionCreator : public GameState
{
public:

	MultiplayerSessionCreator();
	~MultiplayerSessionCreator() override;

	void Load(Socket& sock) override;
	void Unload() override;
	Transition Update(float deltaTime, Socket& sock, NetworkData& networkData) override;
	void Render();

private:
	X::TextureId mEnterYourNameTexture;
	X::TextureId mWaitingForPlayersTexture;
	X::TextureId mEmptyScreenTexture;

	Button mAcceptButton;

	//Network Information
	unsigned short port = 8888;
	NetworkAddress clientAddress;
	std::string clientIP;

	InitPacket initPacketToReceive;
	InitPacket initPacketToSend;

	char bufferToReceive[128];
	char bufferToSend[128];
};

#endif // !INCLUDED_MULTIPLAYER_SESSIONCREATOR_HEADER_H
