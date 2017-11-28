// P2P.cpp: 定义控制台应用程序的入口点。
//

#include "CompleteConfidence/Socket.h"
#include "LeftMyCodes/MyCodes.h"
#include "easylogging/easylogging++.h"

#include <iostream>

#include "P2PUtil.h"
#include "PublicNetWorkServer.h"
#include "P2PClient.h"
#include "P2PServer.h"

INITIALIZE_EASYLOGGINGPP


#define USERSERVER 0
#define USERCLIENT 1
#define P2PSERVER 2

void EasyloggingInit(void) {
	el::Loggers::getLogger("P2P");
}



LeftThrReturn OneConn(LeftThrArgv pCltIf) {
	ClientInfo CltInfo = *(pClientInfo)pCltIf;
	char ip[16];
	char time[20];
	unsigned short port = CltInfo.ClientAddress.sin_port;
	inet_ntop(AF_INET, IPFromAddr(CltInfo.ClientAddress), ip, 16);

	CLOG(INFO, "P2P") << "Accept " << ip << ":" << port;
	CLOG(INFO, "P2P") << "Now time: " << leftName::GetTimeStr(time, 20);
	char buf[4096];
	int bytes;
	for (;;) {
		memset(buf, 0, sizeof(buf));
		bytes = recv(CltInfo.Socket, buf, sizeof(buf), 0);
		if (bytes == SOCKET_ERROR || !bytes)
			break;
		CLOG(INFO, "P2P") << ip << ":" << port << " >>> " << buf;
		send(CltInfo.Socket, "Hi client", sizeof("Hi client"), 0);
		if (bytes == SOCKET_ERROR || !bytes)
			break;
	}
	CLOG(INFO, "P2P") << "lost " << ip << ":" << port;
	CLOG(INFO, "P2P") << "Now time: " << leftName::GetTimeStr(time, 20);
	return 0;
}

LeftThrReturn ListenServer(LeftThrArgv pCltIf) {
	char* ip = ((pServerInfo)pCltIf)->ip;
	unsigned short port = ((pServerInfo)pCltIf)->port;
	int flags = 1;
	do {
		if (LeftSocket::InitializeSocket()) {
			CLOG(INFO, "P2P") << "InitializeSocket failed.";
			break;
		}

		LeftSokt serverSocket;
		if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			CLOG(INFO, "P2P") << "Server create socket failed.";
			break;
		}
		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
			(const char*)&flags, sizeof(int)) == SOCKET_ERROR)
			CLOG(INFO, "P2P") << "Server setsockopt error";
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		serverAddress.sin_family = AF_INET;
		//inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);

		int rt = bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
		if (rt == SOCKET_ERROR) {
			CLOG(INFO, "P2P") << "Server bind failed.";
			break;
		}
		CLOG(INFO, "P2P") << "Bind at " << "" << ":" << port;

		if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
			CLOG(INFO, "P2P") << "Listen failed.";
			break;
		}

		LeftSokt clientSocket;
		struct sockaddr_in clientAddress;
		memset(&clientAddress, 0, sizeof(clientAddress));
		LeftSoktLen addrlen = sizeof(clientAddress);
		LeftThrNo Id = 0;
		ClientInfo* CltInfo = NULL;
		for (;;) {
			if ((clientSocket = accept(serverSocket,
				(sockaddr*)&clientAddress, &addrlen)) == INVALID_SOCKET) {
				CLOG(INFO, "P2P") << "Accept failed.";
				break;
			}
			CltInfo = new ClientInfo;
			CltInfo->ClientAddress = clientAddress;
			CltInfo->Socket = clientSocket;
			CURRENCY_StartThread(OneConn, (LeftThrArgv)CltInfo, Id);

		}

	} while (false);
	LeftSocket::CleanSocket();
	return 0;
}


int client(char* ip, int port) {
	int flags = 1;
	ServerInfo* CltInfo;
	LeftThrNo Id = 0;
	do {
		if (LeftSocket::InitializeSocket()) {
			CLOG(INFO, "P2P") << "InitializeSocket failed.";
			break;
		}

		LeftSokt sock;
		if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			CLOG(INFO, "P2P") << "Create socket failed.";
			break;
		}
		CltInfo = new ServerInfo;
		CltInfo->ip = NULL;
		CltInfo->port = 20000;
		CURRENCY_StartThread(ListenServer, (LeftThrArgv)CltInfo, Id);
		{
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&flags, sizeof(int)) == SOCKET_ERROR)
				CLOG(INFO, "P2P") << "Setsockopt error";
			struct sockaddr_in LocalPort;
			memset(&LocalPort, 0, sizeof(sockaddr_in));
			LocalPort.sin_family = AF_INET;
			//inet_pton(AF_INET, "192.168.58.76", IPFromAddr(LocalPort));
			LocalPort.sin_addr.s_addr = htonl(INADDR_ANY);
			LocalPort.sin_port = htons(20000);
			int rt = bind(sock, (sockaddr*)&LocalPort, sizeof(LocalPort));
			if (rt == SOCKET_ERROR) {
				CLOG(INFO, "P2P") << "Bind failed.";
				break;
			}
			//CLOG(INFO, "P2P") << "Bind at " << ip << ":" << port;

		}
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			CLOG(INFO, "P2P") << "Connect failed.";
			break;
		}
		char buf[4096];
		memset(buf, 0, sizeof(buf));
		while (1) {
			CLOG(INFO, "P2P") << ">";
			std::cin.getline(buf, 4069, '\n');
			if (send(sock, buf, (int)strlen(buf), 0) == SOCKET_ERROR) {
				CLOG(INFO, "P2P") << "Send failed";
				break;
			}
			int bytes;
			if ((bytes = recv(sock, buf, sizeof(buf), 0)) == SOCKET_ERROR) {
				CLOG(INFO, "P2P") << "Recv failed";
				break;
			}
			CLOG(INFO, "P2P") << "<< " << buf;

		}
	} while (false);
	LeftSocket::CleanSocket();
	return 0;
}


int main(int argc, char* argv[]) {

	EasyloggingInit();

	short port;
	int mode;
	char ip[20];
	switch (argc) {
	case 4:
		if (!strcmp(argv[1], "--ns")) mode = P2PSERVER;
		else if (!strcmp(argv[1], "--s")) mode = USERSERVER;
		else if (!strcmp(argv[1], "--c")) mode = USERCLIENT;
		else break;
		snprintf(ip, 20, argv[2]);
		port = atoi(argv[3]);
		CLOG(INFO, "P2P") << "You chose " << ip << ":" << port;
		CLOG(INFO, "P2P") << "I will try to build it like a ";
		if (mode == USERSERVER) {
			CLOG(INFO, "P2P") << "server";
			return leftP2P::P2PServerMain(ip, port);
		}
		else if (mode == USERCLIENT) {
			CLOG(INFO, "P2P") << "client";
			return leftP2P::P2PClientMain(ip, port);
		}
		else if (mode == P2PSERVER) {
			CLOG(INFO, "P2P") << "P2PServer";
			return leftP2P::PublicNetWorkServerMain();
		}
		else {
			CLOG(INFO, "P2P") << "Disable to here!Program error!";
			return 0;
		}
		return 0;
	default:
		std::cout << "P2PForwardInterpreter Version 1.0 (c) left" << std::endl;
		std::cout << "Use like LeftP2P mode(--ns/--s/--c) ip port" << std::endl;
		return 0;
	}

}


