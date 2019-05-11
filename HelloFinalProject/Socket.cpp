#include "Socket.h"
#include "NetworkAddress.h"

Socket::Socket() :
	mHandle(0),
	mError(0)
{

}

Socket::~Socket()
{

}

bool Socket::Open(unsigned short port)
{
#if PLATFORM == PLATFORM_WINDOWS
	WSADATA wsaData;
	mError = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (mError != NO_ERROR)
	{
		std::cout << "(WSAStartup)Error while starting up the Winsock dll. Error code: " << WSAGetLastError() << std::endl;
		return false;
	}
#else 
	return true;
#endif

	mHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mHandle == INVALID_SOCKET)
	{
		std::cout << "(socket)Error while creating the socket. Error code:" << WSAGetLastError() << std::endl;
		return false;
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	mError = bind(mHandle, (const sockaddr*)&address, sizeof(address));
	if (mError == SOCKET_ERROR)
	{
		std::cout << "(bind)Error while binding the local address with the socket created. Error code: " << WSAGetLastError() << std::endl;
		Close();
		return false;
	}

#if PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
	mError = ioctlsocket(mHandle, FIONBIO, &nonBlocking);
	if (mError == SOCKET_ERROR)
	{
		std::cout << "(ioctlsocket)Error while setting the socket to non-blocking mode. Error code: " << WSAGetLastError() << std::endl;
		Close();
		return false;
	}
	return true;
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	int nonBlocking = 1;
	if (fcntl(mHandle, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
	{
		std::cout << "(fcntl)Error while setting the socket to non-blocking mode" << std::endl;
		Close();
		return false;
	}
	return true;
#endif
}

void Socket::Close()
{
#if PLATFORM == PLATFORM_WINDOWS
	closesocket(mHandle);
	WSACleanup();
#elif PLATFORM == PLATFORM_MAC || PLATFROM_UNIX
	close(mHandle);
#endif
}

//bool Socket::IsOpen() const
//{
//	
//}

bool Socket::Send(const NetworkAddress& destination, const void* data, int size)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(addr.sin_family, destination.address.c_str(), &(addr.sin_addr));
	addr.sin_port = htons(destination.port);

	int sent_bytes = sendto(mHandle, (const char*)data, size, 0, (sockaddr*)&addr, sizeof(addr));
	if (sent_bytes != size)
	{
		std::cout << "(sendto)Error while sending packet. Error code: " << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}

int Socket::Receive(void* data, int size, std::string& senderAddress)
{
	sockaddr_in from;
	int fromLength = sizeof(from);
	char fromAddress[64];

	int bytes = recvfrom(mHandle, (char*)data, size, 0, (sockaddr*)&from, &fromLength);
	if (bytes <= 0)
	{
		if (WSAGetLastError() != 10035)
		{
			std::cout << "(recvfrom)Error while receiving packet. Error code: " << WSAGetLastError() << std::endl;
		}
		return bytes;
	}
	else
	{
		inet_ntop(from.sin_family, &from.sin_addr, fromAddress, sizeof(fromAddress));
		senderAddress = fromAddress;
	}
	//process received packet
	return bytes;
}

char* Socket::GetLocalMachineIP()
{
	char hostName[256];
	gethostname(hostName, sizeof(hostName));
	PDNS_RECORD pDnsRecord;

	wchar_t wText[256];
	mbstowcs(wText, hostName, sizeof(hostName));

	DNS_STATUS status = DnsQuery(wText, DNS_TYPE_A, DNS_QUERY_STANDARD, nullptr, &pDnsRecord, nullptr);
	IN_ADDR ipaddr;
	ipaddr.S_un.S_addr = (pDnsRecord->Data.A.IpAddress);

	DnsRecordListFree(pDnsRecord, DnsFreeRecordList);

	return inet_ntoa(ipaddr);
}
