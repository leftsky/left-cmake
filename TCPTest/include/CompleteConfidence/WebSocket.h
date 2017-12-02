

#ifndef _LEFT_WEBSOCKET_H
#define _LEFT_WEBSOCKET_H


#define MAGIC_STR "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define MAGIC_STR_LEN 37
#define WEBSOCKET_KEY_LEN 24

#include "Socket.h"

typedef struct _WEBSOCKET_HEAD_STRUCT {
	bool FIN;
	unsigned short Opcode;
	bool Mask;
	unsigned short PaylodLen;
	char Masking_key[4];
	unsigned int moreLenHigh;
	unsigned int moreLenLow;
	int wordOff;
} webSkt, *pwebSkt;

namespace CCWebSocket {
	char *CountMagicStr(
		char *AskKey, char *AnswerKey, int Len);
	char *GetAnswerStr(
		char *PacketAsk, char *PacketAnswer, int Len);
	char *DecipheringStr(
		const char *CipherText, int TextLen, char key[], char *DecipheringStr, int DhStrLen);
	char *PackageHead(
		const char *words, int Len, char *Answer, int AnswerLen);
};

#endif