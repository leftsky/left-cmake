
#include "P2PServer.h"

#include <iostream>
#include <time.h>

#include "CompleteConfidence\Socket.h"
#include "LeftMyCodes\MyCodes.h"

namespace {
	LeftThrReturn ServerConn(LeftThrArgv pCltIf) {
		ClientInfo CltInfo = *(pClientInfo)pCltIf;
		char ip[16];
		char time[20];
		unsigned short port = CltInfo.ClientAddress.sin_port;
		inet_ntop(AF_INET, IPFromAddr(CltInfo.ClientAddress), ip, 16);

		std::cout << "Accept " << ip << ":" << port << std::endl;
		std::cout << "Now time: " << leftName::GetTimeStr(time, 20) << std::endl;
		char buf[4096];
		int bytes;
		for (;;) {
			memset(buf, 0, sizeof(buf));
			bytes = recv(CltInfo.Socket, buf, sizeof(buf), 0);
			if (bytes == SOCKET_ERROR || !bytes)
				break;
			std::cout << ip << ":" << port << " >>> " << buf << std::endl;
			send(CltInfo.Socket, "Hi client", sizeof("Hi client"), 0);
			if (bytes == SOCKET_ERROR || !bytes)
				break;
		}
		std::cout << "lost " << ip << ":" << port << std::endl;
		std::cout << "Now time: " << leftName::GetTimeStr(time, 20) << std::endl;
		return 0;
	}

	LeftThrReturn ListenServer(LeftThrArgv pCltIf) {
		char* ip = ((pServerInfo)pCltIf)->ip;
		unsigned short port = ((pServerInfo)pCltIf)->port;
		using namespace std;
		int flags = 1;
		do {
			if (LeftSocket::InitializeSocket()) {
				cout << "InitializeSocket failed." << endl;
				break;
			}

			LeftSokt serverSocket;
			if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
				cout << "Server create socket failed." << endl;
				break;
			}
			if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&flags, sizeof(int)) == SOCKET_ERROR)
				cout << "Server setsockopt error" << endl;
			struct sockaddr_in serverAddress;
			memset(&serverAddress, 0, sizeof(sockaddr_in));
			serverAddress.sin_family = AF_INET;
			//inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
			serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
			serverAddress.sin_port = htons(port);

			if (bind(serverSocket, (sockaddr*)&serverAddress,
				sizeof(serverAddress)) == SOCKET_ERROR) {
				cout << "Server bind failed." << endl;
				break;
			}
			cout << "Bind at " << "" << ":" << port << endl;

			if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
				cout << "Listen failed." << endl;
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
					cout << "Accept failed." << endl;
					break;
				}
				CltInfo = new ClientInfo;
				CltInfo->ClientAddress = clientAddress;
				CltInfo->Socket = clientSocket;
				CURRENCY_StartThread(ServerConn, (LeftThrArgv)CltInfo, Id);

			}

		} while (false);
		LeftSocket::CleanSocket();
		return 0;
	}
}

int leftP2P::P2PServerMain(char* ip, int port) {
	using namespace std;
	int flags = 1;
	ServerInfo* CltInfo;
	LeftThrNo Id = 0;
	do {
		if (LeftSocket::InitializeSocket()) {
			cout << "InitializeSocket failed." << endl;
			break;
		}

		LeftSokt sock;
		if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			cout << "Create socket failed." << endl;
			break;
		}
		CltInfo = new ServerInfo;
		CltInfo->ip = NULL;
		CltInfo->port = 20000;
		CURRENCY_StartThread(ListenServer, (LeftThrArgv)CltInfo, Id);
		{
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&flags, sizeof(int)) == SOCKET_ERROR)
				cout << "Setsockopt error" << endl;
			struct sockaddr_in LocalPort;
			memset(&LocalPort, 0, sizeof(sockaddr_in));
			LocalPort.sin_family = AF_INET;
			inet_pton(AF_INET, "127.0.0.1", IPFromAddr(LocalPort));
			//LocalPort.sin_addr.s_addr = htonl(INADDR_ANY);
			LocalPort.sin_port = 20000;

			if (bind(sock, (sockaddr*)&LocalPort, sizeof(LocalPort)) == SOCKET_ERROR) {
				cout << "Bind failed." << endl;
				break;
			}
			else {
				char Localip[64];
				inet_ntop(AF_INET, IPFromAddr(LocalPort), Localip, 16);
				cout << "Bind client socket "
					<< Localip << ":" << LocalPort.sin_port << endl;
			}


		}
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			cout << "Connect failed." << endl;
			break;
		}
		char buf[4096];
		memset(buf, 0, sizeof(buf));
		char LinkName[] = "TestServer";
		while (1) {
			cout << ">";
			cin.getline(buf, 4069, '\n');
			//sprintf(buf, "Register:%s", LinkName);
			int bufLen = strlen(buf);
			buf[bufLen] = leftP2P::CountVerifyPacket(buf, bufLen);
			if (send(sock, buf, bufLen + 1, 0) == SOCKET_ERROR) {
				cout << "Send failed" << endl;
				break;
			}
			int bytes;
			if ((bytes = recv(sock, buf, sizeof(buf), 0)) == SOCKET_ERROR) {
				cout << "Recv failed" << endl;
				break;
			}
			cout << "<< " << buf << endl;
		}
	} while (false);
	LeftSocket::CleanSocket();
	return 0;
}

