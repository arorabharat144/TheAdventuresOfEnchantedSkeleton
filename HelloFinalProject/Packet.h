#ifndef INCLUDED_PACKET_HEADER_H
#define INCLUDED_PACKET_HEADER_H

#include <string>

struct InitPacket
{
	std::string name;

	bool hasPressedAccept = false;
};

struct GameplayPacket
{
	float skullFacePosX;
	int enemiesKilled = 0;
	bool hasReachedEnd = false;
	bool hasDied = false;
};

#endif // !INCLUDED_PACKET_HEADER_H
