
#ifndef _GLOBAL_INFO_H_
#define _GLOBAL_INFO_H_

#include "LeftMyCodes\LeftDefs.h"

class global {
public:
	bool		SetIp(char* ip);
	LEFT_ERROR	GetIp(char* ip, int len);
	bool		SetPort(short port);
	bool		GetPort(short* port);
private:
	char*	Ip;
	short	Port;
	int		OneConnTimeOut;
	//Event	lock;
};

#endif
