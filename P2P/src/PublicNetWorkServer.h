

#ifndef _LEFT_PUBLIC_NET_WORK_SERVER_H_
#define _LEFT_PUBLIC_NET_WORK_SERVER_H_

#include "P2PUtil.h"


#define LINKWAY_IP			0x1
#define LINKWAY_PORT		0x2
#define LINKWAY_LINKNAME	0x3

namespace leftP2P {
	bool RunSocketOrder(char* buf, int len, char* ip, int ipLen, unsigned short* port);
	bool SaveServerLink(char* ip, short port, char* LinkName);
	bool GetServerLink(void* Key, short way);
	int PublicNetWorkServerMain();
}

#endif