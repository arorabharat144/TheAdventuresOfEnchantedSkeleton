#ifndef INCLUDED_NETWORKADDRESS_HEADER
#define INCLUDED_NETWORKADDRESS_HEADER

#include <string>

struct NetworkAddress
{
	NetworkAddress() :
		port(0) {}
	NetworkAddress(std::string addressParam, unsigned short portParam) :
		address(addressParam),
		port(portParam) {}

	std::string address;
	unsigned short port;
};

#endif // !INCLUDED_ADDRESS_HEADER
