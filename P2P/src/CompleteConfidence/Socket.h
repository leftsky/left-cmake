
#ifndef _LEFT_SOCKET_CURRENCY_H_
#define _LEFT_SOCKET_CURRENCY_H_

#include "CCDefs.h"


class _LEFT_SOCKET_CURRENCY {
protected:
	_LEFT_SOCKET_CURRENCY() {}
	virtual int Initialize() = 0;
	short						port;
	long						ip;
};

#ifdef CC_OS_WIN
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")  
#define IPFromAddr(addr)		&addr.sin_addr.S_un
#define LeftCloseSocket			closesocket
class LeftSocketWin :public _LEFT_SOCKET_CURRENCY {
public:
	static int	InitializeSocket() {
		WSADATA	wsa; return WSAStartup(MAKEWORD(2, 2), &wsa);
	}
	static void CleanSocket() { WSACleanup(); }
};
typedef LeftSocketWin			LeftSocket;
typedef SOCKET					LeftSokt;
typedef int						LeftSoktLen;
#endif
#ifdef CC_OS_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET_ERROR			-1
#define INVALID_SOCKET			-1
#define IPFromAddr(addr)		&addr.sin_addr
#define LeftCloseSocket			close
class LeftSocketLinux :public _LEFT_SOCKET_CURRENCY {
public:
	static int	InitializeSocket() { return 0; }
	static void CleanSocket() {  }
};
typedef LeftSocketLinux	LeftSocket;
typedef int				LeftSokt;
typedef socklen_t		LeftSoktLen;
#endif

#endif


