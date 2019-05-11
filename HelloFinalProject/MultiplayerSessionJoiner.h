#ifndef INCLUDED_MULTIPLAYER_SESSIONJOINER_HEADER_H
#define INCLUDED_MULTIPLAYER_SESSIONJOINER_HEADER_H

#include "GameState.h"
#include "Button.h"

#include "NetworkAddress.h"
#include "Packet.h"

class Socket;
struct NetworkData;

class MultiplayerSessionJoiner : public GameState
{
public:
	MultiplayerSessionJoiner();
	~MultiplayerSessionJoiner() override;

	void Load(Socket& sock) override;
	void Unload() override;
	Transition Update(float deltaTime, Socket& sock, NetworkData& networkData) override;
	void Render();

private:
	X::TextureId mEnterIPMenuTexture;
	X::TextureId mWaitingForResponseTexture;
	X::TextureId mEmptyScreenTexture;

	Button mAcceptButton;

	unsigned short port = 8888;

	NetworkAddress hostAddress;

	InitPacket initPacketToSend;
	InitPacket initPacketToReceive;

	char bufferToSend[128];
	char bufferToReceive[128];

	std::string hostIP;
};

#endif // !INCLUDED_MULTIPLAYER_SESSIONJOINER_HEADER_H