
#include "P2PUtil.h"

char leftP2P::CountVerifyPacket(char* packet, int len) {
	char Verify = 0;
	for (int i = 0; i < len; i++) {
		Verify ^= packet[i];
	}
	return Verify;
}