

#include "CompleteConfidence/Socket.h"
#include <stdio.h>
#include <iostream>
#include <time.h>

#include "LeftMyCodes/MyCodes.h"
#include "easylogging/easylogging++.h"

#include "P2PClient.h"



int leftP2P::P2PClientMain(char* ip, int port) {
	do {
		if (LeftSocket::InitializeSocket()) {
			CLOG(INFO, "P2P") << "InitializeSocket failed.";;
			break;
		}
		LeftSokt ClientSocket;
		if ((ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			CLOG(INFO, "P2P") << "Create socket failed.";;
		}
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		if (connect(ClientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			CLOG(INFO, "P2P") << "Connect failed.";;
			break;
		}
		char buf[4096];
		memset(buf, 0, sizeof(buf));
		while (1) {
			CLOG(INFO, "P2P") << ">";
			std::cin.getline(buf, 4069, '\n');
			int bufLen = strlen(buf);
			buf[bufLen] = leftP2P::CountVerifyPacket(buf, bufLen);
			//CLOG(INFO, "P2P") << std::hex << "bufLen: " << bufLen + 1;;
			//CLOG(INFO, "P2P") << "Verify: "
			//	<< (int)leftP2P::CountVerifyPacket(buf, bufLen) << std::dec;;
			//buf[bufLen + 1] = '\0';
			if (send(ClientSocket, buf, bufLen + 1, 0) == SOCKET_ERROR) {
				CLOG(INFO, "P2P") << "Send failed";;
				break;
			}
			int bytes;
			memset(buf, 0, sizeof(buf));
			if ((bytes = recv(ClientSocket, buf, sizeof(buf), 0)) == SOCKET_ERROR) {
				CLOG(INFO, "P2P") << "Recv failed";;
				break;
			}
			//buf[bytes] = '\0';
			CLOG(INFO, "P2P") << "<< " << buf;;
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
	snprintf(ip, 64, buf);
	port = atoi(sign + 1);
	CLOG(INFO, "P2P") << "GetServerIp\t" << ip << ":" << port;;
	do {
		LeftSokt ClientSocket;
		if ((ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS)) == INVALID_SOCKET) {
			CLOG(INFO, "P2P") << "Create socket failed.";;
		}
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		inet_pton(AF_INET, ip, IPFromAddr(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		if (connect(ClientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			CLOG(INFO, "P2P") << "Connect failed.";;
			break;
		}
		char buf[4096];
		memset(buf, 0, sizeof(buf));
		while (1) {
			CLOG(INFO, "P2P") << ">";
			std::cin.getline(buf, 4069, '\n');
			int bufLen = strlen(buf);
			buf[bufLen] = leftP2P::CountVerifyPacket(buf, bufLen);
			//CLOG(INFO, "P2P") << std::hex << "bufLen: " << bufLen + 1;;
			//CLOG(INFO, "P2P") << "Verify: "
			//	<< (int)leftP2P::CountVerifyPacket(buf, bufLen) << std::dec;;
			//buf[bufLen + 1] = '\0';
			if (send(ClientSocket, buf, bufLen + 1, 0) == SOCKET_ERROR) {
				CLOG(INFO, "P2P") << "Send failed";;
				break;
			}
			int bytes;
			if ((bytes = recv(ClientSocket, buf, sizeof(buf), 0)) == SOCKET_ERROR) {
				CLOG(INFO, "P2P") << "Recv failed";;
				break;
			}
			CLOG(INFO, "P2P") << "<< " << buf;;

		}


	} while (false);

	system("pause");
	return true;
}

