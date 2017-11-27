
#include "PublicNetWorkServer.h"

#include <stdlib.h>
#include <stdio.h>
#include <mysql.h>
#include <time.h>

#include "CompleteConfidence\Socket.h"
#include "LeftMyCodes\MyCodes.h"

ServerList Head = {NULL, NULL, 0, NULL};

namespace {
	LeftThrReturn NServerConn(LeftThrArgv pCltIf) {
		ClientInfo CltInfo = *(pClientInfo)pCltIf;
		char ip[16];
		char TimeStr[30];
		unsigned short port = CltInfo.ClientAddress.sin_port;
		time_t TSec = time(NULL);
		inet_ntop(AF_INET, IPFromAddr(CltInfo.ClientAddress), ip, 16);

		std::cout << "Accept " << ip << ":" << port << std::endl;
		std::cout << "Now time: " << leftName::GetTimeStr(TimeStr, 30) << std::endl;
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
				std::cout << "Cut because bytes is zero" << std::endl;
				break;
			}
			if (bytes == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAETIMEDOUT) { Sleep(1); continue; }
				else {
					std::cout << "Cut because err is not WSAETIMEDOUT" << std::endl;
					break;
				}
			}
			if (!leftP2P::CountVerifyPacket(buf, bytes)) {
				std::cout << "Verify right" << std::endl;
				buf[bytes - 1] = '\0';
				std::cout << ip << ":" << port << " <<< " << buf << std::endl;
				if(bytes > 10)
					leftP2P::RunSocketOrder(buf, bytes - 1, ip, 16, &port);
				char rt[512];
				sprintf(rt, "%s:%d", ip, port);
				std::cout << "Send msg: " << rt << std::endl;
				if (send(CltInfo.Socket, rt, strlen(rt), 0) == SOCKET_ERROR) {
					std::cout << "Send failed" << std::endl;
					break;
				}
			}
			else {
				std::cout << std::hex << "Read: " << (int)buf[bytes - 1] << std::endl;
				std::cout << "Verify: " << 
					(int)leftP2P::CountVerifyPacket(buf, bytes - 1) << std::endl;
				std::cout << "bytes: " << bytes << std::dec << std::endl;
				std::cout << "Verify not" << std::endl;
			}
			//if (send(CltInfo.Socket, "Hi client", (int)strlen("Hi client"), 0) == SOCKET_ERROR) {
			//	std::cout << "Send failed" << std::endl;
			//	break;
			//}
		}
		closesocket(CltInfo.Socket);
		std::cout << "lost " << ip << ":" << port << std::endl;
		std::cout << "Now time: " << leftName::GetTimeStr(TimeStr, 30) << std::endl;
		return 0;
	}
}

bool leftP2P::RunSocketOrder(
	char* buf, int bufLen, char* ip, int ipLen, unsigned short* port) {
	pServerList p = &Head;
	char* LinkName = new char[bufLen];
	sprintf(LinkName, strstr(buf, ":") + 1);
	if (strstr(buf, "Register") == buf) {
		std::cout << "Get a Regisger" << std::endl;
		char* ipSave = new char[ipLen + 1];
		sprintf(ipSave, ip);
		pServerList SerInfo = new ServerList;
		SerInfo->LinkIp = ipSave;
		SerInfo->LinkName = LinkName;
		SerInfo->Port = *port;
		while (p->Next) { p = p->Next; }
		p->Next = SerInfo;
	}
	else if (strstr(buf, "AskLink") == buf) {
		std::cout << "Get a AskLink" << std::endl;
		p = p->Next;
		while (p && p->LinkName) {
			if (!strcmp(LinkName, p->LinkName)) {
				std::cout << "Get serverInfo ip: " << p->LinkIp
					<< "\t port: " << p->Port << std::endl;
				sprintf(ip, p->LinkIp);
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
	std::cout << "Entry leftP2P::PublicNetWorkServerMain()" << std::endl;
	do {
		if (LeftSocket::InitializeSocket()) {
			std::cout << "InitializeSocket failed." << std::endl;
			break;
		}
		LeftSokt ServerSocket;
		if ((ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			std::cout << "Create socket failed." << std::endl;
		}
		struct sockaddr_in ServerAddress;
		memset(&ServerAddress, 0, sizeof(sockaddr_in));
		ServerAddress.sin_family = AF_INET;
		//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		inet_pton(AF_INET, NSIP, IPFromAddr(ServerAddress));
		ServerAddress.sin_port = htons(NSPORT);

		if (bind(ServerSocket, (sockaddr*)&ServerAddress,
			sizeof(ServerAddress)) == SOCKET_ERROR) {
			std::cout << "Server bind failed." << std::endl;
		}

		if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) {
			std::cout << "Listen failed." << std::endl;
			break;
		}

		std::cout << "Bind at " << NSIP << ":" << NSPORT << std::endl;

		LeftSokt ClientSocket;
		struct sockaddr_in ClientAddress;
		memset(&ClientAddress, 0, sizeof(ClientAddress));
		LeftSoktLen AddrLen = sizeof(ClientAddress);
		LeftThrNo Id = 0;
		ClientInfo* CltInfo = NULL;
		for (;;) {
			if ((ClientSocket = accept(ServerSocket,
				(sockaddr*)&ClientAddress, &AddrLen)) == INVALID_SOCKET) {
				std::cout << "Accept failed." << std::endl;
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