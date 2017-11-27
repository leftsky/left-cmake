
#include "WebSocket.h"
//
#include <stdio.h>
//#include <iostream>
#include <memory.h>
#ifdef CC_OS_LINUX
#include <arpa/inet.h>
#endif

//using namespace std;

#define PER_OF_HTTP "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "
#define SUF_OF_HTTP "\r\nUpgrade: websocket\r\n\r\n"


#include "sha1.h"
#include "base64.h"


char *CCWebSocket::CountMagicStr(char *AskKey, char *AnswerKey, int Len) {
	//std::cout << strlen(MagicKey) << std::endl;
	if (strlen(AskKey) != WEBSOCKET_KEY_LEN)
		return NULL;
	char request[WEBSOCKET_KEY_LEN + MAGIC_STR_LEN + 1];
	memcpy(request, AskKey, WEBSOCKET_KEY_LEN);
	memcpy(request + WEBSOCKET_KEY_LEN, MAGIC_STR, MAGIC_STR_LEN);
	request[WEBSOCKET_KEY_LEN + MAGIC_STR_LEN] = '\0';
	SHA1 sha;
	unsigned int message_digest[5];
	sha.Reset();
	sha << request;
	sha.Result(message_digest);
	for (int i = 0; i < 5; i++) {
		message_digest[i] = htonl(message_digest[i]);
	}
	std::string base20 = base64_encode(
		reinterpret_cast<const unsigned char*>(message_digest), 20);
	//std::cout << "Magic str: " << base20.c_str() << std::endl;
	snprintf(AnswerKey, Len, base20.c_str());
	return AnswerKey;
}

char *CCWebSocket::GetAnswerStr(char *PacketAsk, char *PacketAnswer, int Len) {
	char *webKey = (char*)strstr(PacketAsk, "Sec-WebSocket-Key");
	if (webKey == NULL)
		return NULL;
	char AskKey[WEBSOCKET_KEY_LEN + 1];
	char AnswerKey[128];
	memset(AskKey, 0, sizeof(AskKey));
	memset(AnswerKey, 0, sizeof(AnswerKey));
	memcpy(AskKey, webKey + sizeof("Sec-WebSocket-Key:"), WEBSOCKET_KEY_LEN);
	memset(PacketAnswer, 0, Len);
	snprintf(PacketAnswer, Len, "%s%s%s", PER_OF_HTTP,
		CCWebSocket::CountMagicStr(AskKey, AnswerKey, sizeof(AnswerKey)), SUF_OF_HTTP);
	return PacketAnswer;
}

char *CCWebSocket::DecipheringStr(
	const char *CipherText, int TextLen, char key[], char *DecipheringStr, int DhStrLen) {
	//std::cout << "CipherText: " << CipherText << std::endl;
	if (DhStrLen <= TextLen)
		return NULL;
	DecipheringStr[TextLen] = '\0';
	memcpy(DecipheringStr, CipherText, TextLen);
	for (int i = 0; i < TextLen; i++) {
		int j = i % 4;
		DecipheringStr[i] = CipherText[i] ^ key[j];
	}
	//std::cout << "DecipheringStr: " << DecipheringStr << std::endl;

	return DecipheringStr;
}


char *CCWebSocket::PackageHead(
	const char *words, int Len, char *Answer, int AnswerLen) {
		if (Len < 0)
			return NULL;
		memset(Answer, 0, AnswerLen);
		Answer[Len + 2 + 4 + 8 + 1] = '\0';
		unsigned int off = 0;
		Answer[off] = (char)0x81;
		off++;
		if (Len < 126) {
			Answer[off++] = Len;
		}
		else if (Len > 125 && Len < 0xFFFF) {
			Answer[off++] = 126;
			Answer[off++] = (Len & 0xFF00) / 0x10000;
			Answer[off++] = (Len & 0xFF);
		}
		else if (Len > 0xFFFF) {
			Answer[off++] = 127;
		}
		memcpy(Answer + off, words, Len);
		return Answer;
}

