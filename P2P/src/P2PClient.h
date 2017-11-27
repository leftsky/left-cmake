

#ifndef _LEFT_P2P_CLIENT_H_
#define _LEFT_P2P_CLIENT_H_

#include "P2PUtil.h"

namespace leftP2P {
	int P2PClientMain(char* ip, int port);
	bool GetServerIp(char* buf, int len);
}

#endif