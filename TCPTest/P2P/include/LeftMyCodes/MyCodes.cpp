
#include "MyCodes.h"

#include <stdio.h>
#include <time.h>
#ifdef LEFT_OS_WIN
#include <ShlObj.h>
#endif

unsigned long long hash_(char const * str) {
 unsigned long long ret{ basis };
 while (*str) { ret ^= *str; ret *= prime; str++; }
 return ret;
}

namespace leftName {
#ifdef LEFT_OS_WIN

	void ShowLastError() {
		HLOCAL lpMsgBuf;
		setlocale(LC_ALL, "");
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(),
			MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED),
			(LPTSTR)&lpMsgBuf, 0, NULL);
		std::wcout << (WCHAR*)lpMsgBuf << std::endl;
		LocalFree(lpMsgBuf);
	}

	void GetToken() {
		if (IsUserAnAdmin())
			return;
		else {
			SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
			sei.lpVerb = TEXT("runas");
			WCHAR path[512] = { 0 };
			GetModuleFileName(NULL, (LPSTR)path, sizeof(path));
			sei.lpFile = (LPSTR)path;
			sei.nShow = SW_SHOWNORMAL;
			ShellExecuteEx(&sei);
			exit(0);
		}
	}

	HMODULE GetSelfModuleHandle() {
		MEMORY_BASIC_INFORMATION mbi;
		return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0)
			? (HMODULE)mbi.AllocationBase : NULL);
	}
#endif
	char* GetTimeStr(char* TimeStr, int SizeOfBuf) {
		time_t tt = time(NULL);
#ifdef LEFT_OS_WIN
		tm tmt;
		tm* t = &tmt;
		localtime_s(t, &tt);
#endif
#ifdef LEFT_OS_LINUX
		tm* t = localtime(&tt);
#endif
		snprintf(TimeStr, SizeOfBuf - 1, "%d-%02d-%02d %02d:%02d:%02d",
			t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);
		return TimeStr;
	}

	char* StrToHex(const char* buf, char* Answer, int LenOfAnswer) {
		if (strlen(buf) % 2 != 0 || strlen(buf) / 2 >= LenOfAnswer)
			return NULL;
		for (size_t i = 0; i < strlen(buf) / 2; i++) {
			if (buf[i * 2] >= '0' && buf[i * 2] <= '9') {
				Answer[i] = (buf[i * 2] - '0') * 0x10;
				if (buf[i * 2 + 1] >= '0' && buf[i * 2 + 1] <= '9')
					Answer[i] += buf[i * 2 + 1] - '0';
				else if (buf[i * 2 + 1] >= 'A' && buf[i * 2 + 1] <= 'F')
					Answer[i] += buf[i * 2 + 1] - 'A' + 0xA;
				else if (buf[i * 2 + 1] >= 'a' && buf[i * 2 + 1] <= 'f')
					Answer[i] += buf[i * 2 + 1] - 'a' + 0xA;
				else
					break;
			}
			else if (buf[i * 2] >= 'A' && buf[i * 2] <= 'F') {
				Answer[i] = (buf[i * 2] - 'A') * 0x10 + 0xA0;
				if (buf[i * 2 + 1] >= '0' && buf[i * 2 + 1] <= '9')
					Answer[i] += buf[i * 2 + 1] - '0';
				else if (buf[i * 2 + 1] >= 'A' && buf[i * 2 + 1] <= 'F')
					Answer[i] += buf[i * 2 + 1] - 'A' + 0xA;
				else if (buf[i * 2 + 1] >= 'a' && buf[i * 2 + 1] <= 'f')
					Answer[i] += buf[i * 2 + 1] - 'a' + 0xA;
				else
					break;
			}
			else if (buf[i * 2] >= 'a' && buf[i * 2] <= 'f') {
				Answer[i] = (buf[i * 2] - 'a') * 0x10 + 0xA0;
				if (buf[i * 2 + 1] >= '0' && buf[i * 2 + 1] <= '9')
					Answer[i] += buf[i * 2 + 1] - '0';
				else if (buf[i * 2 + 1] >= 'A' && buf[i * 2 + 1] <= 'F')
					Answer[i] += buf[i * 2 + 1] - 'A' + 0xA;
				else if (buf[i * 2 + 1] >= 'a' && buf[i * 2 + 1] <= 'f')
					Answer[i] += buf[i * 2 + 1] - 'a' + 0xA;
				else
					break;
			}
			else
				break;
			if (i == (strlen(buf) / 2 - 1)) {
				Answer[LenOfAnswer - 1] = '\0';
				return Answer;
			}
		}
		return NULL;
	}

	char* HexToStr(
		const char* order, int LenOfOrder, char* Answer, int LenOfAnswer) {
		if (LenOfOrder * 2 >= LenOfAnswer)
			return NULL;
		char low, high;
		for (int i = 0; i < LenOfOrder; i++) {
			high = (order[i] & 0xF0) / 0x10;
			if (high >= 0 && high <= 9)
				Answer[i * 2] = high + '0';
			else
				Answer[i * 2] = high + 'A' - 0xA;
			low = order[i] & 0xF;
			if (low >= 0 && low <= 9)
				Answer[i * 2 + 1] = low + '0';
			else
				Answer[i * 2 + 1] = low + 'A' - 0xA;
		}
		Answer[LenOfOrder * 2] = '\0';
		return Answer;
	}

}

