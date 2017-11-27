

#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <time.h>

#include "LeftMyCodes\MyCodes.h"
#include "CompleteConfidence\Socket.h"

#include "P2PClient.h"



int leftP2P::P2PClientMain(char* ip, int port) {
	do {
		if (LeftSocket::InitializeSocket()) {
			std::cout << "InitializeSocket failed." << std::endl;
			break;
		}
		LeftSokt ClientSocket;
		if ((ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			std::cout << "Create socket failed." << std::endl;
		}
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		if (connect(ClientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			std::cout << "Connect failed." << std::endl;
			break;
		}
		char buf[4096];
		memset(buf, 0, sizeof(buf));
		while (1) {
			std::cout << ">";
			std::cin.getline(buf, 4069, '\n');
			int bufLen = strlen(buf);
			buf[bufLen] = leftP2P::CountVerifyPacket(buf, bufLen);
			//std::cout << std::hex << "bufLen: " << bufLen + 1 << std::endl;
			//std::cout << "Verify: "
			//	<< (int)leftP2P::CountVerifyPacket(buf, bufLen) << std::dec << std::endl;
			//buf[bufLen + 1] = '\0';
			if (send(ClientSocket, buf, bufLen + 1, 0) == SOCKET_ERROR) {
				std::cout << "Send failed" << std::endl;
				break;
			}
			int bytes;
			memset(buf, 0, sizeof(buf));
			if ((bytes = recv(ClientSocket, buf, sizeof(buf), 0)) == SOCKET_ERROR) {
				std::cout << "Recv failed" << std::endl;
				break;
			}
			//buf[bytes] = '\0';
			std::cout << "<< " << buf << std::endl;
			leftP2P::GetServerIp(buf, bytes);
		}


	} while (false);


	LeftSocket::CleanSocket();
	system("pause");
	return 0;
}

bool leftP2P::GetServerIp(char* buf, int len) {
	char ip[64];
	int port;
	memset(ip, 0, sizeof(ip));
	char* sign = strstr(buf, ":");
	if (!sign) return false;
	*sign = '\0';
	sprintf(ip, buf);
	port = atoi(sign + 1);
	std::cout << "GetServerIp\t" << ip << ":" << port << std::endl;
	do {
		LeftSokt ClientSocket;
		if ((ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			std::cout << "Create socket failed." << std::endl;
		}
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		if (connect(ClientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			std::cout << "Connect failed." << std::endl;
			break;
		}
		char buf[4096];
		memset(buf, 0, sizeof(buf));
		while (1) {
			std::cout << ">";
			std::cin.getline(buf, 4069, '\n');
			int bufLen = strlen(buf);
			buf[bufLen] = leftP2P::CountVerifyPacket(buf, bufLen);
			//std::cout << std::hex << "bufLen: " << bufLen + 1 << std::endl;
			//std::cout << "Verify: "
			//	<< (int)leftP2P::CountVerifyPacket(buf, bufLen) << std::dec << std::endl;
			//buf[bufLen + 1] = '\0';
			if (send(ClientSocket, buf, bufLen + 1, 0) == SOCKET_ERROR) {
				std::cout << "Send failed" << std::endl;
				break;
			}
			int bytes;
			if ((bytes = recv(ClientSocket, buf, sizeof(buf), 0)) == SOCKET_ERROR) {
				std::cout << "Recv failed" << std::endl;
				break;
			}
			std::cout << "<< " << buf << std::endl;

		}


	} while (false);

	system("pause");
	return true;
}

