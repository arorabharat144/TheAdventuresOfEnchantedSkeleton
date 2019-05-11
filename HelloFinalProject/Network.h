#ifndef INCLUDED_NETWORK_HEADER
#define INCLUDED_NETWORK_HEADER

// platform detection

#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC     2
#define PLATFORM_UNIX    3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <WinDNS.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Dnsapi.lib")
#endif

#endif //INCLUDED_NETWORK_HEADER