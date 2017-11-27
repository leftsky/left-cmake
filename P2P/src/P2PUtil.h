

#ifndef _LEFT_P2P_UTIL_H_
#define _LEFT_P2P_UTIL_H_

#include "CompleteConfidence\Socket.h"

#define NSPORT 19911
#define NSIP "127.0.0.1"//"47.74.146.159"

typedef struct _SERVER_INFO {
	char*				ip;
	unsigned short		port;
} ServerInfo, *pServerInfo;

typedef struct _CLIENT_INFO {
	sockaddr_in ClientAddress;
	LeftSokt	Socket;
} ClientInfo, *pClientInfo;

typedef struct _SERVER_LINK_INFO_LIST {
	_SERVER_LINK_INFO_LIST* Next;
	char*			LinkIp;
	unsigned short	Port;
	char*			LinkName;
} ServerList, *pServerList;

namespace leftP2P {
	char CountVerifyPacket(char* packet, int len);
}


#endif

