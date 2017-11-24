// SocketCurrency.cpp: 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory.h>

#include "CompleteConfidence/Socket.h"
#include "LeftMyCodes/MyCodes.h"

#define SERVER 0
#define CLIENT 1

typedef struct _CLIENT_INFO {
	sockaddr_in ClientAddress;
	LeftSokt	Socket;
} ClientInfo, *pClientInfo;


LeftThrReturn OneConn(LeftThrArgv pCltIf) {
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
	LeftCloseSocket(CltInfo.Socket);
	std::cout << "lost " << ip << ":" << port << std::endl;
	std::cout << "Now time: " << leftName::GetTimeStr(time, 20) << std::endl;
	return 0;
}


int server(char* ip, short port) {
	using namespace std;
	do {
		if (LeftSocket::InitializeSocket()) {
			cout << "InitializeSocket failed." << endl;
			break;
		}

		LeftSokt serverSocket;
		if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
			cout << "Create socket failed." << endl;
			break;
		}
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		serverAddress.sin_family = AF_INET;
		inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
		serverAddress.sin_port = htons(port);

		if (bind(serverSocket, (sockaddr*)&serverAddress, 
			sizeof(serverAddress)) == SOCKET_ERROR) {
			cout << "Bind failed." << endl;
			break;
		}
		cout << "Bind at " << ip << ":" << port << endl;

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
			CURRENCY_StartThread(OneConn, (LeftThrArgv)CltInfo, Id);
			
		}

	} while (false);
	LeftSocket::CleanSocket();
	return 0;
}

int client(char* ip, int port) {
	using namespace std;
	do {
		if (LeftSocket::InitializeSocket()) {
			cout << "InitializeSocket failed." << endl;
			break;
		}

		LeftSokt sock;
		if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
			cout << "Create socket failed." << endl;
			break;
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
		while (1) {
			cout << ">";
			cin.getline(buf, 4069, '\n');
			if (send(sock, buf, (int)strlen(buf), 0) == SOCKET_ERROR) {
				cout << "Send failed" << endl;
				break;
			}
			memset(buf, 0, sizeof(buf));
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


int main(int argc, char* argv[]) {
	using namespace std;

	short port;
	int mode;
	char ip[20];
	switch (argc) {
	case 4:
		if (!strcmp(argv[1], "--s")) mode = SERVER;
		else if (!strcmp(argv[1], "--c")) mode = CLIENT;
		else break;
		snprintf(ip, 20, argv[2]);
		port = atoi(argv[3]);
		cout << "You chose " << ip << ":" << port << endl;
		cout << "I will try to build it like a ";
		if (mode == SERVER) { cout << "server" << endl; return server(ip, port); }
		else if (mode == CLIENT) { cout << "client" << endl; return client(ip, port); }
		else { cout << "Disable to here!Program error!" << endl; return 0; }
		return 0;
	default:
		cout << "TCPTest Version 1.0 (c) left" << endl;
		cout << "Use like TCPTest mode(--s/--c) ip port" << endl;
		return 0;
	}

}
