

#ifndef _LEFT_MYCODES_CURRENCY_H_
#define _LEFT_MYCODES_CURRENCY_H_

#include "LeftDefs.h"

#define NONE4100(p) (p)

/* �̺߳�CURRENCY_StartThread��֧�� */
#ifdef _WIN32
#include <Windows.h>
#define CURRENCY_StartThread(thr, arg, id)	\
				CreateThread(NULL, NULL, thr, arg, NULL, id);
//typedef DWORD _stdcall	LeftThrReturn;
#define LeftThrReturn	DWORD _stdcall
typedef LPVOID			LeftThrArgv;
typedef LPDWORD			LeftThrNo;
#endif
#ifdef linux
#include <pthread.h>
#define CURRENCY_StartThread(thr, arg, id)	\
				pthread_create(&id, NULL, thr, arg);
typedef void*			LeftThrReturn;
typedef void*			LeftThrArgv;
typedef pthread_t		LeftThrNo;
#endif
/***************************************************************************************/

/* �ַ������ط���ת��ϣֵ""_hash */
constexpr unsigned long long prime = 0x100000001B3ull;
constexpr unsigned long long basis = 0xCBF29CE484222325ull;
// �ַ������ط���תhashֵ֧�ֺ���
unsigned long long hash_(char const * str);
constexpr unsigned long long hash_compile_time(
	char const* str, unsigned long long last_value = basis) {
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}
constexpr unsigned long long operator""_hash(char const * p, size_t) {
	return hash_compile_time(p);
}
/***************************************************************************************/

/* ָ���ļ�·����ʵ�ֿ���̨��·���ļ�ͬʱ����� */
#include <fstream>
#include <iostream>
class CoutExA {
public:
	CoutExA(char const *filename) :m_fout(filename, std::ios::out | std::ios::app) {}
	template <class T>
	CoutExA& operator<<(const T &text) {
		m_fout << text; std::cout << text; return *this;
	}
	CoutExA& operator<<(std::basic_ostream<char>& (*fp)(std::basic_ostream<char>&)) {
		m_fout << fp; std::cout << fp; return *this;
	}
private:
	std::ofstream m_fout;
};
class CoutExW {
public:
	CoutExW(WCHAR const *filename) :m_wfout(filename, std::wios::out | std::wios::app) {
		m_wfout.imbue(std::locale(std::locale("chs"), "", LC_CTYPE));
	}
	template <class T>
	CoutExW& operator<<(const T &text) {
		m_wfout << text; std::wcout << text; return *this;
	}
	CoutExW& operator<<(std::basic_ostream<wchar_t>& (*fp)(std::basic_ostream<wchar_t>&)) {
		m_wfout << fp; std::wcout << fp; return *this;
	}
private:
	std::wofstream m_wfout;
};
/***************************************************************************************/

// leftName�����ռ�
namespace leftName {
#ifdef _WIN32
	// չʾ��һ����������������ʾ��Ϣ
	extern void ShowLastError();
	// �����ù���ԱȨ��
	extern void GetToken();
	// ��������ģ����
	extern HMODULE GetSelfModuleHandle();
#endif
	// ��õ�ǰ��ʱ���ַ���
	char* GetTimeStr(char* TimeStr, int SizeOfBuf);
	// ���ַ�������ת���ɳ���Ϊһ���ʮ�������ַ���
	char* StrToHex(const char* buf, char* Answer, int LenOfAnswer);
	// ��ʮ�����Ƶ�(��д)�ַ���ת�����������ȵ��ַ���
	char* HexToStr(
		const char* order, int LenOfOrder, char* Answer, int LenOfAnswer);
}
#endif
/***************************************************************************************/