#ifndef INCLUDED_NETWORKDATA_HEADER_H
#define INCLUDED_NETWORKDATA_HEADER_H

#include <string>

struct NetworkData
{
	//HOST
	std::string hostName;
	std::string hostIP;

	//CLIENT
	std::string clientName;
	std::string clientIP;
};

#endif // !INCLUDED_NETWORKDATA_HEADER_H
