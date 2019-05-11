#ifndef INCLUDED_SOCKET_HEADER
#define INCLUDED_SOCKET_HEADER

#include "Network.h"
#include <iostream>

struct NetworkAddress;

class Socket
{
public:
	Socket();
	~Socket();

	bool Open(unsigned short port);
	void Close();
	//bool IsOpen() const;
	bool Send(const NetworkAddress& destination, const void* data, int size);
	int Receive(void* data, int size, std::string& senderAddress);

	char* GetLocalMachineIP();
private:
	SOCKET mHandle;
	int mError;
};

#endif // !INCLUDED_SOCKET_HEADER
