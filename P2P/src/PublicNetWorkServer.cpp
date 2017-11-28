
#include "PublicNetWorkServer.h"

#include <stdlib.h>
#include <stdio.h>
//#include <mysql.h>
#include <time.h>

#include "CompleteConfidence\Socket.h"
#include "LeftMyCodes\MyCodes.h"
#include "easylogging/easylogging++.h"

ServerList Head = { NULL, NULL, 0, NULL };

namespace {
	LeftThrReturn NServerConn(LeftThrArgv pCltIf) {
		ClientInfo CltInfo = *(pClientInfo)pCltIf;
		char ip[16];
		char TimeStr[30];
		unsigned short port = CltInfo.ClientAddress.sin_port;
		time_t TSec = time(NULL);
		inet_ntop(AF_INET, IPFromAddr(CltInfo.ClientAddress), ip, 16);

		CLOG(INFO, "P2P") << "Accept " << ip << ":" << port;;
		CLOG(INFO, "P2P") << "Now time: " << leftName::GetTimeStr(TimeStr, 30);;
		int nNetTimeout = 1;// Socket³¬Ê±Ê±³¤
		setsockopt(CltInfo.Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
		char buf[4096];
		int bytes;
		time_t past = 0;
		for (;;) {
			if (time(NULL) - TSec >= 60)
				break;
			memset(buf, 0, sizeof(buf));
			bytes = recv(CltInfo.Socket, buf, sizeof(buf), 0);
			if (bytes == 0) {
				CLOG(INFO, "P2P") << "Cut because bytes is zero";;
				break;
			}
			if (bytes == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAETIMEDOUT) { Sleep(1); continue; }
				else {
					CLOG(INFO, "P2P") << "Cut because err is not WSAETIMEDOUT";;
					break;
				}
			}
			if (!leftP2P::CountVerifyPacket(buf, bytes)) {
				CLOG(INFO, "P2P") << "Verify right";;
				buf[bytes - 1] = '\0';
				CLOG(INFO, "P2P") << ip << ":" << port << " <<< " << buf;;
				if (bytes > 10)
					leftP2P::RunSocketOrder(buf, bytes - 1, ip, 16, &port);
				char rt[512];
				snprintf(rt, 512, "%s:%d", ip, port);
				CLOG(INFO, "P2P") << "Send msg: " << rt;;
				if (send(CltInfo.Socket, rt, strlen(rt), 0) == SOCKET_ERROR) {
					CLOG(INFO, "P2P") << "Send failed";;
					break;
				}
			}
			else {
				CLOG(INFO, "P2P") << std::hex << "Read: " << (int)buf[bytes - 1];;
				CLOG(INFO, "P2P") << "Verify: " <<
					(int)leftP2P::CountVerifyPacket(buf, bytes - 1);;
				CLOG(INFO, "P2P") << "bytes: " << bytes << std::dec;;
				CLOG(INFO, "P2P") << "Verify not";;
			}
			//if (send(CltInfo.Socket, "Hi client", (int)strlen("Hi client"), 0) == SOCKET_ERROR) {
			//	CLOG(INFO, "P2P") << "Send failed";;
			//	break;
			//}
		}
		closesocket(CltInfo.Socket);
		CLOG(INFO, "P2P") << "lost " << ip << ":" << port;;
		CLOG(INFO, "P2P") << "Now time: " << leftName::GetTimeStr(TimeStr, 30);;
		return 0;
	}
}

bool leftP2P::RunSocketOrder(
	char* buf, int bufLen, char* ip, int ipLen, unsigned short* port) {
	pServerList p = &Head;
	char* LinkName = new char[bufLen];
	snprintf(LinkName, bufLen, strstr(buf, ":") + 1);
	if (strstr(buf, "Register") == buf) {
		CLOG(INFO, "P2P") << "Get a Regisger";;
		char* ipSave = new char[ipLen + 1];
		snprintf(ipSave, ipLen + 1, ip);
		pServerList SerInfo = new ServerList;
		SerInfo->LinkIp = ipSave;
		SerInfo->LinkName = LinkName;
		SerInfo->Port = *port;
		while (p->Next) { p = p->Next; }
		p->Next = SerInfo;
	}
	else if (strstr(buf, "AskLink") == buf) {
		CLOG(INFO, "P2P") << "Get a AskLink";;
		p = p->Next;
		while (p && p->LinkName) {
			if (!strcmp(LinkName, p->LinkName)) {
				CLOG(INFO, "P2P") << "Get serverInfo ip: " << p->LinkIp
					<< "\t port: " << p->Port;;
				snprintf(ip, ipLen, p->LinkIp);
				*port = p->Port;
				return true;
			}
			p = p->Next;
		}
	}
	return true;
}

bool leftP2P::SaveServerLink(char* ip, short port, char* LinkName) {
	return true;
}

bool leftP2P::GetServerLink(void* Key, short way) {
	return true;
}

int leftP2P::PublicNetWorkServerMain() {
	CLOG(INFO, "P2P") << "Entry leftP2P::PublicNetWorkServerMain()";;
	do {
		if (LeftSocket::InitializeSocket()) {
			CLOG(INFO, "P2P") << "InitializeSocket failed.";;
			break;
		}
		LeftSokt ServerSocket;
		if ((ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			CLOG(INFO, "P2P") << "Create socket failed.";;
		}
		struct sockaddr_in ServerAddress;
		memset(&ServerAddress, 0, sizeof(sockaddr_in));
		ServerAddress.sin_family = AF_INET;
		//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		inet_pton(AF_INET, NSIP, IPFromAddr(ServerAddress));
		ServerAddress.sin_port = htons(NSPORT);

		if (bind(ServerSocket, (sockaddr*)&ServerAddress,
			sizeof(ServerAddress)) == SOCKET_ERROR) {
			CLOG(INFO, "P2P") << "Server bind failed.";;
		}

		if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) {
			CLOG(INFO, "P2P") << "Listen failed.";;
			break;
		}

		CLOG(INFO, "P2P") << "Bind at " << NSIP << ":" << NSPORT;;

		LeftSokt ClientSocket;
		struct sockaddr_in ClientAddress;
		memset(&ClientAddress, 0, sizeof(ClientAddress));
		LeftSoktLen AddrLen = sizeof(ClientAddress);
		LeftThrNo Id = 0;
		ClientInfo* CltInfo = NULL;
		for (;;) {
			if ((ClientSocket = accept(ServerSocket,
				(sockaddr*)&ClientAddress, &AddrLen)) == INVALID_SOCKET) {
				CLOG(INFO, "P2P") << "Accept failed.";;
				break;
			}
			CltInfo = new ClientInfo;
			CltInfo->ClientAddress = ClientAddress;
			CltInfo->Socket = ClientSocket;
			CURRENCY_StartThread(NServerConn, (LeftThrArgv)CltInfo, Id);
		}

	} while (false);


	LeftSocket::CleanSocket();
	system("pause");
	return 0;
}