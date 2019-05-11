#ifndef INCLUDED_MULTIPLAYERMANAGERCLIENT_HEADER_H
#define INCLUDED_MULTIPLAYERMANAGERCLIENT_HEADER_H

#include <XEngine.h>
#include "NetworkAddress.h"
#include "Packet.h"

class Socket;
struct NetworkData;

class MultiPlayerManagerClient
{
public:
	MultiPlayerManagerClient();
	~MultiPlayerManagerClient();

	void Load();
	void Unload();

	void Update(float deltaTime, Socket& sock, NetworkData& networkData);
	void Render();

private:
	//Multiplayer Stuff
	std::string hostName;
	std::string clientName;

	NetworkAddress hostAddress;
	unsigned short port = 8888;

	X::TextureId hostSkullFace;
	X::Math::Vector2 hostSkullFacePosition;

	X::TextureId clientSkullFace;
	X::Math::Vector2 clientSkullFacePosition;

	X::TextureId progressBar;

	GameplayPacket gameplayPacketToSend;
	GameplayPacket gameplayPacketToReceive;

	char bufferToSend[128];
	char bufferToReceive[128];

	int hostEnemiesKilled;
	int clientEnemiesKilled;

	bool hostHasReachedEnd;
	bool clientHasReachedEnd;

	bool hostHasDied;
	bool clientHasDied;

	X::TextureId youWonTexture;
	X::TextureId youLostTexture;
};

#endif // !INCLUDED_MULTIPLAYERMANAGERCLIENT_HEADER_H
