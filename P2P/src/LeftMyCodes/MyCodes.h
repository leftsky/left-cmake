

#ifndef _LEFT_MYCODES_CURRENCY_H_
#define _LEFT_MYCODES_CURRENCY_H_

#include "LeftDefs.h"

#define NONE4100(p) (p)

/***************************************************************************************/
/* 易用通用宏 */
/* ms延时 */
#ifdef LEFT_OS_WIN
#define LeftSleep(n) Sleep(n)
#endif
#ifdef LEFT_OS_LINUX
#include <unistd.h>
#define LeftSleep(n) usleep(n * 1000)
#endif	
/* 获得程序当前路径 */
#ifdef LEFT_OS_WIN
#define GetSelfModulePath(path,len) \
			GetModuleFileName(NULL, path, len); *(strrchr(path, '\\') + 1) = 0
#endif
#ifdef LEFT_OS_LINUX
#define GetSelfModulePath(path,len) getcwd(path,len)
#endif
/***************************************************************************************/

/***************************************************************************************/
/* 线程和CURRENCY_StartThread宏支持 */
#ifdef LEFT_OS_WIN
#include <Windows.h>
#define CURRENCY_StartThread(thr, arg, id)	\
				CreateThread(NULL, NULL, thr, arg, NULL, id)
//typedef DWORD _stdcall	LeftThrReturn;
#define EndThread
#define LeftThrReturn	DWORD _stdcall
typedef LPVOID			LeftThrArgv;
typedef LPDWORD			LeftThrNo;
#endif
#ifdef LEFT_OS_LINUX
#include <pthread.h>
#define CURRENCY_StartThread(thr, arg, id)	\
				pthread_create(&id, NULL, thr, arg)
#define EndThread		pthread_detach(pthread_self())
#define LeftThrReturn	void*
#define LeftThrArgv		void*
typedef pthread_t		LeftThrNo;
#endif
/***************************************************************************************/

/***************************************************************************************/
/* 字符串重载符号转哈希值""_hash */
constexpr unsigned long long prime = 0x100000001B3ull;
constexpr unsigned long long basis = 0xCBF29CE484222325ull;
// 字符串重载符号转hash值支持函数
unsigned long long hash_(char const *str);
constexpr unsigned long long hash_compile_time(
	char const *str, unsigned long long last_value = basis) {
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) *prime) : last_value;
}
constexpr unsigned long long operator"" _hash(char const *p, size_t) {
	return hash_compile_time(p);
}
/***************************************************************************************/

/***************************************************************************************/
/* 指定文件路径后，实现控制台和路径文件同时输出流 */
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
#ifdef LEFT_OS_WIN
class CoutExW {
public:
	CoutExW(const wchar_t *filename) :m_wfout(filename, std::wios::out | std::wios::app) {
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
#endif
/***************************************************************************************/

// leftName命名空间
namespace leftName {
#ifdef LEFT_OS_WIN
	// 展示上一个错误代码的中文提示信息
	extern void ShowLastError();
	// 请求获得管理员权限
	extern void GetToken();
	// 获得自身的模块句柄
	extern HMODULE GetSelfModuleHandle();
#endif
	// 获得当前的时间字符串
	char *GetTimeStr(char *TimeStr, int SizeOfBuf);
	char *GetTimeStr(char *TimeStr, int SizeOfBuf, int off);
	// 将字符串命令转换成长度为一半的十六进制字符串
	char *StrToHex(const char *buf, char *Answer, int LenOfAnswer);
	// 将十六进制的(大写)字符串转换成两倍长度的字符串
	char *HexToStr(
		const char *order, int LenOfOrder, char *Answer, int LenOfAnswer);
	// 解析INI文件,提供查询,填写和删除链表操作.删除链表请保证Head可被delete[],即是new出来的
	typedef struct _INI_INFO_LIST {
		_INI_INFO_LIST*		Next;
		char*				Key;
		char*				Value;
	} IniInfo, *pIniInfo;
	LEFT_ERROR AnalysisIniFile(char *path, pIniInfo InfoHead);
	LEFT_ERROR GetIniInfo(
		const char *key, char *value, int valueLen, pIniInfo infoHead);
	LEFT_ERROR DelIniInfoList(pIniInfo infoList);
	// 根据间隔符获得内部字符串CutStr
	char *getNextOrder(char **buf, char *order, int Len, char SignChar);
	LEFT_ERROR AddSign(const char *orderBuff, char *buff, char SignChar);
}
#endif
/***************************************************************************************/

